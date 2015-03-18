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
    bool ok = FeatureDetectorFPNode::setup( argv );
    if (!ok) return false;
    
    return ok;
}

// ........................................................... process_one_frame

bool
OpticalFlowFPNode::process_one_frame()
{
    bool ok = src->ready;
    
    // new frame into dst
    gray( *in, dst->mat );
    invalidate(dst);
    
    // detect matches last frame in `dst` and the new frame in src..
    // NOTICE: dst should always be ready unless its the first frame..
    if ( ok ) ok = detect();
    else      prepare( dst ); // prepared `dst` as first frame..
    
    if ( ok ) ok = matched_keypoints();
    
    if ( window ){
        base->copyTo( out );
        if ( ok ) drawArrows( out, dst->good_kpts, src->good_kpts, OCV_WHITE );
        window_show( window, out );
    }
    
    // save this frame from dst into src, making src ready..
    swap_src_dst();
    
    return FeatureDetectorFPNode::process_one_frame();
}



