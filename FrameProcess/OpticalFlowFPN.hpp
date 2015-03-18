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

public:
    
     OpticalFlowFPNode();
    ~OpticalFlowFPNode();
    
    // ....................................................... overriden methods
    
    bool setup( argv_t *argv );
    bool process_one_frame();
};

#endif /* defined(ocv_OpticalFlowFPN_hpp) */
