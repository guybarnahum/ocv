// =============================================================================
//
//  OpticalFlowFPN.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/15/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#include "OpticalFlowFPN.hpp"

#define OPTICALFLOW_NAME "OpticalFlowFPNode"
#define OPTICALFLOW_DESC "Identify optical flow between frames"

// =========================================================== OpticalFlowFPNode

// ................................................................. constructor

OpticalFlowFPNode::OpticalFlowFPNode():FeatureDetectorFPNode()
{
    set_name( OPTICALFLOW_NAME );
    set_desc( OPTICALFLOW_DESC );
}

OpticalFlowFPNode::~OpticalFlowFPNode()
{
}

// ....................................................................... setup

bool OpticalFlowFPNode::setup( argv_t *argv )
{
    // call the parent setup for base class setup options
    // do this first so we have `dbg` option set, etc
    bool ok = FrameProcessNode::setup( argv );
    if (!ok) return false;
    
    return ok;
}

// ........................................................... process_one_frame
bool
OpticalFlowFPNode::process_one_frame()
{
    if ( window ){
        base->copyTo( out );
    }
    
    if ( window ){
        window_show( window, out );
    }
    
    return true;
}



