// =============================================================================
//
//  Calibrate.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/14/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_calibrate_hpp
#define ocv_calibrate_hpp

#include "ocvstd.hpp"
#include <opencv2/ccalib.hpp>

#include <stdio.h>

class clibrateCamera{
    
    vector< vector<Point2f> > cal_points;
    
    Size    board_size;          // number of sqaures by width and height
    Size    frame_size;          // frame size
    float   square_size;         // The size of a square in your defined unit (point, millimeter,etc).
    float   aspect_ratio;        // The aspect ratio
    int     flag;
    
    vector<Mat> rvecs;
    vector<Mat> tvecs;
    
    vector<float> reprojErrs;
    double        totalAvgErr;
    Mat           cam_mat;
    Mat           dist_coeffs;
    
    clibrateCamera()
    {
        flag =  CALIB_FIX_PRINCIPAL_POINT   |
                CALIB_ZERO_TANGENT_DIST     |
                CALIB_FIX_ASPECT_RATIO      ;
        
        board_size  = Size( 8, 8);
        square_size = 1;
        
        cam_mat     = Mat::eye  (3, 3, CV_64F);
        dist_coeffs = Mat::zeros(8, 1, CV_64F);
    }
    
    bool   process_frame( Mat &in, bool use );
    bool   calc_BoardCornerPositions( vector<Point3f>& corners );
    double calc_ReprojectionErrors( const vector<vector<Point3f> >& obj_points);

    bool calc();
    bool save( string xml);
};

#endif /* defined(__ocv__calibrate__) */
