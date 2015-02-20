// =============================================================================
//
//  HoughLineFPNode.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================


#ifndef ocv_HoughLineFPNode_hpp
#define ocv_HoughLineFPNode_hpp

// ===================================================================== include
#include <iostream>
#include "FrameProcessNode.hpp"

#define HOUGHLINE_NAME "HoughLineFPNode"
#define HOUGHLINE_DESC "Identify lines with HoughLines algorithm"

// ======================================================= class HoughLineFPNode

class HoughLineFPNode : public FrameProcessNode {
    
public:
    
    vector<Vec4i> lines;

     HoughLineFPNode()
     {
        set_name( HOUGHLINE_NAME );
        set_desc( HOUGHLINE_DESC );
     }
    
    ~HoughLineFPNode(){}
    
    bool process_one_frame();
};

#endif /* defined(ocv_houghLineProcess_hpp) */
