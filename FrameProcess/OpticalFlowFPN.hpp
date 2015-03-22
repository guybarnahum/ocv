// =============================================================================
//
//  OpticalFlowFPN.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_OpticalFlowFPN_hpp
#define ocv_OpticalFlowFPN_hpp

// ==================================================================== includes

#include "ocvstd.hpp"
#include "FeatureDetectorFPN.hpp"

// ===================================================== class OpticalFlowFPNode

class OpticalFlowFPNode : public FeatureDetectorFPNode {

private:
    
    typedef enum{
        
        METHOD_NONE,
        
        // valid methods
        METHOD_RICH_FEATURES,
        METHOD_OPTICAL_FLOW_PYR_LK,

        // default
        METHOD_OPTICAL_FLOW = METHOD_OPTICAL_FLOW_PYR_LK,
        METHOD_DEFAULT      = METHOD_OPTICAL_FLOW,
        
    }method_e;
    
    method_e  from_string( const char *str );
    const char *to_string( method_e    m   );

    method_e method;
    
    string   camera_xml;
    Mat      K; // camera_mat
    
    vector<uchar> v_status;
    vector<float> v_error;
    
    bool detect_rich_features();
    bool detect_optical_flow();
    void prune_keypoints();
    bool calc();

public:
    
     OpticalFlowFPNode();
    ~OpticalFlowFPNode();
    
    // ....................................................... overriden methods

    bool setup( argv_t *argv );
    bool process_one_frame();
};

#endif /* defined(ocv_OpticalFlowFPN_hpp) */
