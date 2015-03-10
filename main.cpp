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

#include "ocvstd.hpp"
#include "cli.hpp"

#include "VideoProcess.hpp"
#include <time.h>

// ........................................................................ main

int
main( int argc, const char * argv[] )
{    
    LOG(LEVEL_INFO) << "Hello World!";

    print_argv( argc, argv );
    
    err_t err = OK;
    cli_parser cli( argc, argv );
    
    if ( OK != ( err = last_err() ) ){
        print_err();
        // NOTICE: print errs but reset them and try to continue..
    }
    
    vector<argv_t> *v_argv = cli.get_v_argv();
    
    // .............................. open input stream
    // TODO: decide where to capture from based args
    // right now its default camera
    
    VideoProcess vp(0);
    
    if (vp.is_ready()){
        vp.setup( v_argv );
    }
    
    if ( vp.is_ready() ){
         vp.print_desc();
         vp.process();
    }
    
    // all done : emit errors
    if ( OK != ( err = last_err() ) ){
        print_err();
    }

    return (int)err;
}
