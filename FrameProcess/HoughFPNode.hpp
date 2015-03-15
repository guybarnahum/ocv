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

#include "ocvstd.hpp"
#include "FrameProcessNode.hpp"

#define HOUGHLINE_NAME "HoughFPNode"
#define HOUGHLINE_DESC "Identify lines and circles with Hough algorithm"

// ======================================================= class HoughLineFPNode

class HoughFPNode : public FrameProcessNode {
    
private:
    
    bool do_lines;
    Mat  gray_mat;
    
public:
    
    vector<Vec4i> lines;
    vector<Vec3f> circles;
    
     HoughFPNode();
    ~HoughFPNode(){}
    
    // ....................................................... overriden methods
    bool process_key( int key );
    bool process_one_frame();
};

#endif /* defined(ocv_houghLineProcess_hpp) */
