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
#define OPTICALFLOW_DETECTOR    "surf"

// =========================================================== OpticalFlowFPNode

// ................................................................. constructor

OpticalFlowFPNode::OpticalFlowFPNode():FeatureDetectorFPNode( (char *)OPTICALFLOW_DETECTOR )
{
    // setup name / desc
    string str  = OPTICALFLOW_NAME;
    str += ":";
    str += FeatureDetectorFPNode::get_name();
    
    set_name( str );
    
    // desc
    str  = OPTICALFLOW_DESC;
    str += "\n";
    str += FeatureDetectorFPNode::get_desc();
    set_desc( str );

    do_rich_features = false;
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
    
    // NOTICE : Optional settings ahead - reset ok state
    //          and start preserving errors with && ok

    ok = true;
    
    // do_rich_features option
    ok = get_val_bool( argv, "do_rich_features", do_rich_features ) && ok ;
    LOG( LEVEL_INFO ) << "Using "
                      << ( do_rich_features? "rich features" : "optical flow");
    
    return ok;
}

// ........................................................ detect_rich_features

bool OpticalFlowFPNode::detect_rich_features()
{
    // save last frame from dst into src, making src ready..
    swap_src_dst();
    
    // overwrite dst with new frame
    gray( *in, dst->mat );
    invalidate(dst);

    // we have both src and dst!
    // detect matches last frame in `dst` and the new frame in src..
    // NOTICE: dst should always be ready unless its the first frame..
    bool ok = detect();
    if ( ok ) ok = matched_keypoints();

    return ok;
}

// ......................................................... detect_optical_flow

bool OpticalFlowFPNode::detect_optical_flow()
{
    // save last frame from `dst` into `src`, making src ready..
    // unless its the first frame!
    swap_src_dst();
    
    // overwrite dst with new frame
    gray( *in, dst->mat );
    
    // look for keypoints in src, the last frame..
    // if first frame src is not ready!
    // TODO: maybe we should inherit these from dst?
    invalidate(src);
    bool ok =  prepare(src) && ( src->keypoints.size() != 0 );
    
    // calc flow from last frame in `src` and the new frame in `dst`..
    
    if ( ok ){
        convert_keypoints_to_point2f( src->keypoints, src->good_kpts );
        
        calcOpticalFlowPyrLK( src->mat      , dst->mat      ,
                              src->good_kpts, dst->good_kpts,
                              v_status      , v_error       );
    }
    
    return ok;
}

// ........................................................... process_one_frame

bool
OpticalFlowFPNode::process_one_frame()
{
    bool ok = do_rich_features? detect_rich_features() : detect_optical_flow();
    
    if ( window ){
        base->copyTo( out );
        
        if ( ok ) drawArrows( out,
                              src->good_kpts,
                              dst->good_kpts,
                              v_status,
                              v_error,
                              OCV_WHITE );
        
        window_show( window, out );
    }
    
    return FeatureDetectorFPNode::process_one_frame();
}



