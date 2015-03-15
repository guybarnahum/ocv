//
//  Calibrate.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/14/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#include "Calibrate.hpp"

// Feed frames into camera calibration
bool clibrateCamera::process_frame( Mat &in, bool use )
{
    vector<Point2f> points;
    
    bool ok = findChessboardCorners( in, board_size, points,
                                     CALIB_CB_ADAPTIVE_THRESH   |
                                     CALIB_CB_FAST_CHECK        |
                                     CALIB_CB_NORMALIZE_IMAGE   );
    if (ok){
        Mat gray;
        cvtColor(in, gray, COLOR_BGR2GRAY);
        cornerSubPix( gray, points, Size(11,11), Size(-1,-1),
                      TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
    
        if ( use ) cal_points.push_back( points );
        frame_size = in.size();
        drawChessboardCorners( in, board_size, Mat(points), ok );
    }

    return ok;
}

bool clibrateCamera::calc_BoardCornerPositions( vector<Point3f>& corners )
{
    corners.clear();
    
    for( int ix = 0; ix < board_size.height; ix++ )
        for( int jx = 0; jx < board_size.width; jx++ )
            corners.push_back( Point3f(float( jx *square_size ),
                                       float( ix *square_size ), 0));
    return true;
}

double clibrateCamera::calc_ReprojectionErrors(
                                const vector<vector<Point3f> >& obj_points )
{
    vector<Point2f> imagePoints2;
    
    size_t totalPoints = 0;
    double totalErr = 0, err;
    reprojErrs.resize( obj_points.size() );
    
    for( size_t ix = 0; ix < obj_points.size(); ix++ )
    {
        projectPoints( Mat(obj_points[ix]),
                      rvecs[ix], tvecs[ix], cam_mat,
                      dist_coeffs, imagePoints2);
        
        err = norm( Mat(obj_points[ix]), Mat( imagePoints2 ), CV_L2);
        
        size_t n = obj_points[ix].size();
        reprojErrs[ix] = (float) sqrt( err * err / n );
        totalErr     += err * err;
        totalPoints  += n;
    }
    
    return sqrt( totalErr / totalPoints );
}

bool clibrateCamera::calc()
{        
    if( flag & CALIB_FIX_ASPECT_RATIO )
        cam_mat.at<double>(0,0) = 1.0;
    
    vector< vector<Point3f> >  obj_pts(1)  ;
    calc_BoardCornerPositions( obj_pts[0] );
        
    obj_pts.resize( cal_points.size(), obj_pts[0] );
        
    //Find intrinsic and extrinsic camera parameters
    double rms = calibrateCamera( obj_pts,
                                  cal_points, frame_size,
                                  cam_mat   , dist_coeffs,
                                  rvecs     , tvecs,
                                  flag | CALIB_FIX_K4 | CALIB_FIX_K5 );
        
    cout << "Re-projection error reported by calibrateCamera: "<< rms << endl;
        
    bool ok = checkRange( cam_mat ) && checkRange( dist_coeffs );
   
    totalAvgErr = calc_ReprojectionErrors( obj_pts );
    
    cout << "Calibration " << (ok ? "succeeded" : "failed")
         << " (avg re projection error:"  << totalAvgErr << ")" << endl;

    return ok;
}

bool clibrateCamera::save( string xml)
{
    FileStorage fs;
    bool ok = false;
    
    try{
        ok = fs.open( xml, FileStorage::WRITE );
    }
    catch( Exception e ){
        cout << e.what() << endl;
        cout << "Could not save into " << xml << endl ;
        ok = false;
    }
    
    time_t tm;
    time( &tm );
        
    struct tm *t2 = localtime( &tm );
    char buf[1024];
    strftime( buf, sizeof(buf)-1, "%c", t2 );
        
    fs << "calibration_Time" << buf;
        
    if( !rvecs.empty() || !reprojErrs.empty() )
        fs << "nrOfFrames" << (int)std::max( rvecs.size(), reprojErrs.size() );
    fs << "image_Width"  << frame_size.width;
    fs << "image_Height" << frame_size.height;
    fs << "board_Width"  << board_size.width;
    fs << "board_Height" << board_size.height;
    fs << "square_Size"  << square_size;
        
    if( flag & CALIB_FIX_ASPECT_RATIO )
        fs << "FixAspectRatio" << aspect_ratio;
        
    if( flag ){
        sprintf( buf, "flags: %s%s%s%s",
            flag & CALIB_USE_INTRINSIC_GUESS ? " +use_intrinsic_guess" : "",
            flag & CALIB_FIX_ASPECT_RATIO ? " +fix_aspectRatio" : "",
            flag & CALIB_FIX_PRINCIPAL_POINT ? " +fix_principal_point" : "",
            flag & CALIB_ZERO_TANGENT_DIST ? " +zero_tangent_dist" : "" );
        
        cvWriteComment( *fs, buf, 0 );
    }
        
    fs << "flagValue" << flag;
        
    fs << "Camera_Matrix"           << cam_mat;
    fs << "Distortion_Coefficients" << dist_coeffs;
        
    fs << "Avg_Reprojection_Error" << totalAvgErr;
    if( !reprojErrs.empty() )
        fs << "Per_View_Reprojection_Errors" << Mat(reprojErrs);
    
    if( !rvecs.empty() && !tvecs.empty() )
    {
        CV_Assert(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
        for( int i = 0; i < (int)rvecs.size(); i++ )
        {
            Mat r = bigmat(Range(i, i+1), Range(0,3));
            Mat t = bigmat(Range(i, i+1), Range(3,6));
            
            CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
            //*.t() is MatExpr (not Mat) so we can use assignment operator
            r = rvecs[i].t();
            t = tvecs[i].t();
        }
        cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
        fs << "Extrinsic_Parameters" << bigmat;
    }
    
    if( !cal_points.empty() )
    {
        Mat imagePtMat((int)cal_points.size(), (int)cal_points[0].size(), CV_32FC2);
        
        for( size_t ix = 0; ix < cal_points.size(); ix++ ){
            Mat r = imagePtMat.row((int)ix ).reshape( 2, imagePtMat.cols );
            Mat imgpti(cal_points[ix]);
            imgpti.copyTo(r);
        }
        
        fs << "Image_points" << imagePtMat;
    }

    fs.release();

    return ok;
    
}