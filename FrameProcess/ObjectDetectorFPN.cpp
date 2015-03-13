// =============================================================================
//
//  ObjectDetectorFPN.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/12/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// .................................................................... includes
#include "ObjectDetectorFPN.hpp"

// ....................................................................... setup
bool ObjectDetectorFPNode::setup( argv_t *argv )
{
    bool ok = (argv != nullptr );
    if (!ok){
        FAIL << "no setup argv provided for " << get_name();
        return false;
    }
        
    // call the parent setup for base class setup options
    // do this first so we have `dbg` option set, etc
        
    ok = FrameProcessNode::setup( argv );

    if (!ok){
        FAIL << "invalid argv provided for " << get_name();
        return false;
    }
    
    // ------------------------------- Required arguments
    const char *val;
    string msg = "";

    // ................................... obj_path option
    ok = ( nullptr != ( val = get_val( argv, "obj_path" ) ) );

    if ( ok ){
        obj_path   = val;
        ok = file_to_path( obj_path );
    }
    
    if ( ok ){
        cascade    = makePtr<CascadeClassifier>( obj_path );
        i_detector = makePtr<CascadeDetectorAdapter>( cascade );
        ok = !cascade.empty();
    }
    
    if( ok ){
        cascade    = makePtr<CascadeClassifier>(obj_path);
        i_tracker  = makePtr<CascadeDetectorAdapter>( cascade );
        ok = !cascade.empty();
    }
    
    if (ok){
        DetectionBasedTracker::Parameters params;
        detector = new DetectionBasedTracker( i_detector, i_tracker, params);
        ok = detector != nullptr;
    }
    
    if (!ok){
        msg += "could not construct detector from '";
        msg += obj_path;
        msg += "'";
    }
    
    // train the classifier
    if ( ok ){
         ok = detector->run();
    
        if (!ok){
            msg += "failed to train detector with '";
            msg += obj_path;
            msg += "'";
        }
    }
    
    if (!ok){
        set_err( INVALID_ARGS, msg.c_str() );
    }
    
    return ok;
}

// ........................................................... process_one_frame

bool ObjectDetectorFPNode::process_one_frame()
{
    // Sanity checks
    if ( detector == nullptr ){
        // don't call again!
        return false;
    }
        
    cvtColor(*in, gray, COLOR_RGB2GRAY);
    detector->process( gray );
    detector->getObjects( FoundObjects );
        
    if ( window ){
        
        base->copyTo( out );
        
        for (size_t ix = 0; ix < FoundObjects.size(); ix++ ){
            rectangle( out, FoundObjects[ ix ], Scalar(0,255,0));
        }
        
        window_show( window, out );
    }
    
    return true;
}
