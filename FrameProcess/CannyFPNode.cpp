// =============================================================================
//
//  CannyFPNode.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes

#include "CannyFPNode.hpp"

// ================================================================= CannyFPNode

// ........................................................... process_one_frame
bool
CannyFPNode::process_one_frame()
{
    Canny( *in, out, 50, 200, 3);
    return true;
}
