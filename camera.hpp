// =============================================================================
//
//  camera.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/21/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_camera_hpp
#define ocv_camera_hpp

#include "ocvstd.hpp"

// ====================================================================== Camera

class Camera{

public:
    Camera( string xml );
    Camera();
    
    bool calibrate( string xml );
    
    string xml;

    Mat K;          // Calibration matrix
    Mat Kinv;
    Mat dist_coeff; // disturtion coefficients
    
    Mat F;          // Fundamental matrix
    Mat E;          // Essential matrix
    
    Matx34d P0, P1; // Camera movement!
    
    bool calc_fundamental_mat();
    bool is_coherent_rotation( Mat &R);
    bool triangulate();
};

#endif /* defined(ocv_camera_hpp) */
