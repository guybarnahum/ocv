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
#include "CascadeDetectorFPN.hpp"

#define CASCADE_DETECTOR_NAME "CascadeDetector"
#define CASCADE_DETECTOR_DESC "Cascade classifier detector based tracker"

CascadeDetectorFPNode::CascadeDetectorFPNode():FrameProcessNode()
{
    set_name( CASCADE_DETECTOR_NAME );
    set_desc( CASCADE_DETECTOR_DESC );
}

// ....................................................................... setup
bool CascadeDetectorFPNode::setup( argv_t *argv )
{
    // call the parent setup for base class setup options
    // do this first so we have `dbg` option set, etc
        
    bool ok = FrameProcessNode::setup( argv );
    if (!ok) return false;
    
    // obj_path is required argument
    obj_path = get_val( argv, "obj_path" );

    ok = file_to_path( obj_path );
    
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
    
    string msg = "";

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
    
    if (ok){
        LOG( LEVEL_INFO ) << "obj_path : " << obj_path;
    }
    else{
        set_err( INVALID_ARGS, msg );
    }
    return ok;
}

// ........................................................... process_one_frame

bool CascadeDetectorFPNode::process_one_frame()
{
    // Sanity checks
    if ( detector == nullptr ){
        // don't call again!
        return false;
    }
        
    cvtColor(*in, gray, COLOR_RGB2GRAY);
    detector->process( gray );
    detector->getObjects( found_objects );
    
    if ( window ){
        
        base->copyTo( out );
        
        bool ok = found_objects.size() && select_focus();
        if ( ok ){
            for (size_t ix = 0; ix < found_objects.size(); ix++ ){
                Scalar color = ( found_objects[ ix ] == focus )?
                                OCV_WHITE :  OCV_GREEN;
                rectangle( out, found_objects[ ix ], color );
            }
        }
        
        window_show( window, out );
    }
    
    return true;
}
