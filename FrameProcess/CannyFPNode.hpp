// =============================================================================
//
//  CannyFPNode.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================


#ifndef ocv_CannyFPNode_hpp
#define ocv_CannyFPNode_hpp

// ===================================================================== include
#include "ocvstd.hpp"
#include "FrameProcessNode.hpp"

#define CANNY_NAME "CannyFPNode"
#define CANNY_DESC "Detect edges with canny filter"

// =========================================================== class CannyFPNode

class CannyFPNode : public FrameProcessNode {
    
public:
    
     CannyFPNode():FrameProcessNode()
     {
        set_name( CANNY_NAME );
        set_desc( CANNY_DESC );
     }
    
    ~CannyFPNode(){}
    
    // ....................................................... overriden methods
    bool process_one_frame();
};

#endif /* defined(ocv_CannyFPNode_hpp) */
