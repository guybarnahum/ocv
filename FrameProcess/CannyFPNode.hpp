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


// =========================================================== class CannyFPNode

class CannyFPNode : public FrameProcessNode {
    
private:
    
    int low;
    int high;
    int ker;
    
public:
    
     CannyFPNode();
    ~CannyFPNode(){}
    
    // ....................................................... overriden methods
    bool setup( argv_t *argv );
    bool process_one_frame();
};

#endif /* defined(ocv_CannyFPNode_hpp) */
