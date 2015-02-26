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

#include "ocv.hpp"
#include "VideoProcess.hpp"
#include "utils.hpp"

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
    cout << "running from " << argv[ 0 ] << endl;
    print_usage();
    
    // .............................. open input stream
    // TODO: decide where to capture from based args
    // right now its default camera
    
    VideoProcess vp(0);
    argv_t args;

#if 0
    args = { {"window", "Canny"} , {"window" , "Canny"} };
    vp.setup( "canny" , &args  );
    
    args = { {"window", "HoughLines"} };
    vp.setup( "houghLine", &args  );

#else
    args = { { "window" , "OpenCV.3.0.0"   },
             { "tgt"    , "/Users/guyb/Desktop/tgt.png" },
        };
    
    vp.setup( "sift", &args );
#endif
    
    vp.print_desc( cout );
    
    vp.process();
    
    // ....................................... all done
    return error( err );
}
