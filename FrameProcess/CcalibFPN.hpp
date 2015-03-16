// =============================================================================
//
//  CcalibFPN.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/15/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_CcalibFPN_hpp
#define ocv_CcalibFPN_hpp

// ===================================================================== include

#include "ocvstd.hpp"
#include "FrameProcessNode.hpp"
#include <opencv2/ccalib.hpp>

// ========================================================== class CcalibFPNode

class CcalibFPNode : public FrameProcessNode {
    
private:
    
    // settings
    Size    board_size;          // number of sqaures by width and height
    float   square_size;         // The size of a square in your defined unit (point, millimeter,etc).
    int     flags;
    
    // samples
    vector< vector<Point2f> > cal_points;

    // output
    string  xml;                 // calibration file
    Size    frame_size;          // frame size
    float   aspect_ratio;        // The aspect ratio
    
    // state
    bool   use_frame;
    int    min_capture_frames;
    
    vector<Mat> rvecs;
    vector<Mat> tvecs;
    
    vector<float> reprojErrs;
    double        totalAvgErr;
    
    Mat           cam_mat;
    Mat           dist_coeffs;
    
    
    bool   detect_chessboard( Mat &in );
    bool   calc_boardCornerPositions( vector<Point3f>& corners );
    double calc_reprojectionErrors( const vector<vector<Point3f> >& obj_points);
    
    bool calc_and_save();
    bool calc();
    bool save( string xml );

public:

     CcalibFPNode();
    ~CcalibFPNode(){}
    
    // ....................................................... overriden methods
    
    bool setup( argv_t *argv );
    bool process_key( int key );
    bool process_one_frame();
};

#endif /* defined(ocv_CcalibFPN_hpp) */
