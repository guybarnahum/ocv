// =============================================================================
//
//  main.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/13/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes

#include "VideoProcess.hpp"

// =============================================================================
//                                  misc routines
// =============================================================================

// ....................................................................... usage
static void
print_usage()
{
    cout <<
    "\nThis program demonstrates line finding with the Hough transform."
    << endl;
}

// ....................................................................... error
static int
error( int err )
{
    string msg;
    
    switch( err ){
        case ERR_CAPTURE_FAILURE : msg = "Video capture failure"; break;
    }
    
    if ( !msg.empty() ){
        cout << "Error:" << msg << endl;
    }
    
    return err;
}

// ........................................................................ main

int
main( int argc, const char * argv[] )
{
    int err = ERR_OK;
    
    // ................................... process args
    print_usage();
    
    // .............................. open input stream
    // TODO: decide where to capture from based args
    // right now its default camera
    
    VideoProcess vp(0);
    
    vp.setup( "canny"    , (void *)"Canny" );
    vp.setup( "houghLine", (void *)"OpenCV.3.0.0" );

    vp.print_desc( cout );
    
    vp.process();
    
    // ....................................... all done
    return error( err );
}
