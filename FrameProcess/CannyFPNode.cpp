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

#define CANNY_NAME "CannyFPNode"
#define CANNY_DESC "Detect edges with canny filter"
#define CANNY_LOW_THRESHOLD     100
#define CANNY_HIGH_THRESHOLD    (CANNY_LOW_THRESHOLD * 3)
#define CANNY_KERNEL_SIZE       3

// ================================================================= CannyFPNode

CannyFPNode::CannyFPNode():FrameProcessNode()
{
    set_name( CANNY_NAME );
    set_desc( CANNY_DESC );
    
    low = CANNY_LOW_THRESHOLD;
    high= CANNY_HIGH_THRESHOLD;
    ker = CANNY_KERNEL_SIZE;
}

// ....................................................................... setup

bool CannyFPNode::setup( argv_t *argv )
{
    // call the parent setup for base class setup options
    // do this first so we have `dbg` option set, etc
    
    bool ok = FrameProcessNode::setup( argv );
    if (!ok) return false;
    
    // .................................... low threshold
    ok = get_val_int( argv, "low", low  );
    LOG( LEVEL_INFO ) << "low threshold: " << low ;
    
    // ................................... high threshold
    int num = 0;
    
    if ( ok ) ok = get_val_int( argv, "high", num  );
    if ( ok ){
         ok = num > low;
        
         if ( !ok ){
            string msg  = "high threshold ("; msg+= num;
                   msg +=")should be bigger than low threshold("; msg += low;
                   msg += ")!";
            
            set_err( INVALID_ARGS, msg );
         }
         high = num;
    }
    string msg = ( high < low * 3 )? "(not recommended!)" : "";
    LOG( LEVEL_INFO ) << "high threshold: " << high << msg;
    
    // .................................... low threshold
    if (ok) ok = get_val_int( argv, "kernel", ker  );
    if (ok){
        ok = is_odd_num( ker );
        if (!ok){
            set_err( INVALID_ARGS, "kernel has to be an odd number!" );
        }
    }
    LOG( LEVEL_INFO ) << "kernel: " << ker ;

    return ok;
}

// ........................................................... process_one_frame
bool
CannyFPNode::process_one_frame()
{
    Canny( *in, out, low, high, ker);
    window_show( window, out );
    
    return true;
}
