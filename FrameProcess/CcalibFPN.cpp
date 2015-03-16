// =============================================================================
//
//  CcalibFPN.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/15/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#include "CcalibFPN.hpp"

#define CCALIB_NAME "CcalibFPNode"
#define CCALIB_DESC "Calibrate camera with chessboard pattern"

// ================================================================ CcalibFPNode

// ................................................................. constructor

CcalibFPNode::CcalibFPNode():FrameProcessNode()
{
    set_name( CCALIB_NAME );
    set_desc( CCALIB_DESC );
    
    min_capture_frames = 4;
    
    flags = CALIB_FIX_PRINCIPAL_POINT   |
            CALIB_ZERO_TANGENT_DIST     |
            CALIB_FIX_ASPECT_RATIO      ;
    
    board_size  = Size( 10, 7);
    square_size = 1;
    
    cam_mat     = Mat::eye  (3, 3, CV_64F);
    dist_coeffs = Mat::zeros(8, 1, CV_64F);
    
    xml = "./ccalib.xml";
    use_frame = true;
}

// ....................................................................... setup

bool CcalibFPNode::setup( argv_t *argv )
{
    // call the parent setup for base class setup options
    // do this first so we have `dbg` option set, etc
    bool ok = FrameProcessNode::setup( argv );
    if (!ok) return false;
    
    // .......................................... xml out
    const char *val = get_val( argv, "out" );
    if (val != nullptr) xml = strdup( val );
    LOG( LEVEL_INFO ) << "xml out : " << xml;
    
    // ....................................... board size
    int num = 0;
    ok = get_val_int( argv, "board_size", num  ) && num;
    if ( ok ) board_size = Size( num, num );
    
    LOG( LEVEL_INFO ) << "board_size: " << board_size.width << "x"
                                        << board_size.height ;
    
    // ...................................... square size
    num = 0;
    ok = get_val_int( argv, "square_size", num  ) && num;
    if ( ok ) square_size = num;
    LOG( LEVEL_INFO ) << "square_size: " << square_size << " units";

    return ok;
}

// ................................................................. process_key

bool
CcalibFPNode::process_key(int key )
{
    switch( key ){
        // space bar attempts to grab chessboard, use_frame is reset by
        // process_frame, so this is done once..
        case KEY_SPACE : use_frame = true; break;

        case 'c' : calc(); break;
        case 's' : save( xml ); break;
    }
    
    return true;
}

// ........................................................... process_one_frame
bool
CcalibFPNode::process_one_frame()
{
    if ( window ){
        base->copyTo( out );
    }
    
    // also draws on out if detected
    detect_chessboard( out );
    
    if ( window ){
        window_show( window, out );
    }
    
    return true;
}

// ........................................................... detect_chessboard

bool CcalibFPNode::detect_chessboard( Mat &in )
{
    vector<Point2f> points;
    
    bool ok = findChessboardCorners( in, board_size, points,
                                    CALIB_CB_ADAPTIVE_THRESH   |
                                    CALIB_CB_FAST_CHECK        |
                                    CALIB_CB_NORMALIZE_IMAGE   );
    if (ok){
        Mat      gray_mat ;
        gray(in, gray_mat);
        
        cornerSubPix( gray_mat, points, Size(11,11), Size(-1,-1),
                     TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
        
        if ( use_frame ) cal_points.push_back( points );
        
        frame_size = in.size();
        drawChessboardCorners( in, board_size, Mat(points), ok );
    }
    
    if ( ok && use_frame ){
        
        LOG( LEVEL_INFO ) << "capture frame ok!" <<
                            " (has " << cal_points.size() << " out of "
                                     << min_capture_frames << " )";
        use_frame = false;
    }
    
    return ok;
}

// ........................................................................ calc

bool CcalibFPNode::calc()
{
    bool ok = ( cal_points.size() >= min_capture_frames );
    
    if (!ok){
        LOG( LEVEL_INFO ) << "calc needs at least " << min_capture_frames <<
                        " captured frames (has " << cal_points.size() << " )";
        return false;
    }
        
    if ( flags & CALIB_FIX_ASPECT_RATIO ) cam_mat.at<double>(0,0) = 1.0;
    
    vector< vector<Point3f> >  obj_pts(1)  ;
    calc_boardCornerPositions( obj_pts[0] );
    
    obj_pts.resize( cal_points.size(), obj_pts[0] );
    
    //Find intrinsic and extrinsic camera parameters
    double rms = calibrateCamera( obj_pts,
                                 cal_points, frame_size,
                                 cam_mat   , dist_coeffs,
                                 rvecs     , tvecs,
                                 flags | CALIB_FIX_K4 | CALIB_FIX_K5 );
    
    LOG( LEVEL_INFO ) << "Re-projection error reported by calibrateCamera: "
                      << rms;
    
    ok = checkRange( cam_mat ) && checkRange( dist_coeffs );
    
    totalAvgErr = calc_reprojectionErrors( obj_pts );
    
    LOG( LEVEL_INFO ) << "Calibration " << (ok ? "succeeded" : "failed")
                      << " (avg re projection error:"  << totalAvgErr << ")" ;
    
    return ok;
}

bool CcalibFPNode::calc_boardCornerPositions( vector<Point3f>& corners )
{
    corners.clear();
    
    for( int ix = 0; ix < board_size.height; ix++ )
        for( int jx = 0; jx < board_size.width; jx++ )
            corners.push_back( Point3f(float( jx *square_size ),
                                       float( ix *square_size ), 0));
    return true;
}

double CcalibFPNode::calc_reprojectionErrors(
                            const vector<vector<Point3f> >& obj_points )
{
    vector<Point2f> pts_2f;
    
    size_t totalPoints = 0;
    double totalErr = 0, err;
    reprojErrs.resize( obj_points.size() );
    
    for( size_t ix = 0; ix < obj_points.size(); ix++ )
    {
        projectPoints( Mat(obj_points[ix]),
                      rvecs[ix],
                      tvecs[ix],
                      cam_mat,
                      dist_coeffs,
                      pts_2f);
        
        vector<Point3f> pts_3f( pts_2f.size() );
        for( size_t jx = 0 ; jx < pts_2f.size(); jx++ ){
            pts_3f[ ix ].x = pts_2f[ ix ].x;
            pts_3f[ ix ].y = pts_2f[ ix ].y;
            pts_3f[ ix ].z = 0.;
        }
        
        err = norm( obj_points[ix] , pts_3f, CV_L2);
        
        size_t n = obj_points[ix].size();
        reprojErrs[ix] = (float) sqrt( err * err / n );
        totalErr     += err * err;
        totalPoints  += n;
    }
    
    return sqrt( totalErr / totalPoints );
}

// ........................................................................ save

bool CcalibFPNode::save( string xml)
{
    FileStorage fs;
    bool ok = false;
    
    try{
        ok = fs.open( xml, FileStorage::WRITE );
    }
    catch( Exception e ){
        LOG( LEVEL_ERROR ) << "Could not save into '"
                           << xml << "'\n" << e.what() ;
        ok = false;
    }
    
    LOG( LEVEL_INFO ) << "Saving into '" << xml << "'";
    
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
    
    if( flags & CALIB_FIX_ASPECT_RATIO ) fs << "FixAspectRatio" << aspect_ratio;
    
    if( flags ){
        sprintf( buf, "flags: %s%s%s%s",
                flags & CALIB_USE_INTRINSIC_GUESS ? " +use_intrinsic_guess" : "",
                flags & CALIB_FIX_ASPECT_RATIO ? " +fix_aspectRatio" : "",
                flags & CALIB_FIX_PRINCIPAL_POINT ? " +fix_principal_point" : "",
                flags & CALIB_ZERO_TANGENT_DIST ? " +zero_tangent_dist" : "" );
        
        cvWriteComment( *fs, buf, 0 );
    }
    
    fs << "flagValue"               << flags;
    fs << "Camera_Matrix"           << cam_mat;
    fs << "Distortion_Coefficients" << dist_coeffs;
    fs << "Avg_Reprojection_Error"  << totalAvgErr;
    
    if( !reprojErrs.empty() ){
        fs << "Per_View_Reprojection_Errors" << Mat(reprojErrs);
    }
    
    if( !rvecs.empty() && !tvecs.empty() ){
        OCV_ASSERT(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
        for( int i = 0; i < (int)rvecs.size(); i++ )
        {
            Mat r = bigmat(Range(i, i+1), Range(0,3));
            Mat t = bigmat(Range(i, i+1), Range(3,6));
            
            OCV_ASSERT(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            OCV_ASSERT(tvecs[i].rows == 3 && tvecs[i].cols == 1);
            //*.t() is MatExpr (not Mat) so we can use assignment operator
            r = rvecs[i].t();
            t = tvecs[i].t();
        }
        cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
        fs << "Extrinsic_Parameters" << bigmat;
    }
    
    if( !cal_points.empty() ){
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
