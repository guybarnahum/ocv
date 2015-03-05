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

// =============================================================================
//                                  misc routines
// =============================================================================

char *path_to_file( char* path )
{
    char *file = path;
    char  ch;
    
    // set file as the charachter after the last '/'
    while( ( ch = *path++ ) != '\0' ){
        if ( ch == '/'    ) file = path; // notice: path was ++ already
    }
    
    return file;
}

// ....................................................................... usage
static void
usage( int argc, const char * argv[], int err = ERR_OK )
{
    char *file = path_to_file( (char *)argv[0] );
    cout << file << " video processing pipeline" << endl;
    
    // options are reported next
    
    // error specific usage
    switch( err ){
        default     :
        case ERR_OK : break;
    }
}

// ................................................................ process_args

// LAME HACK! LAME HACK! LAME HACK! LAME HACK! LAME HACK! LAME HACK! LAME HACK!
//
// TODO: unimplemented yet! Should process argv into building a VideoProcessing
// pipeline consisting of FrameProcessNodes and their arguments.
//
// LAME HACK! LAME HACK! LAME HACK! LAME HACK! LAME HACK! LAME HACK! LAME HACK!

static vector<argv_t>
process_args( int argc, const char * argv[], int &err )
{
    vector<argv_t> v_argv;
    argv_t args;
    
#if 0
    
    args[ "fpn"    ] = "canny";
    args[ "window" ] = "Canny";

    v_argv.push_back( args );

    args[ "fpn"    ] = "houghLine";
    args[ "window" ] = "HoughLines";

    v_argv.push_back( args );

#else
    
    args[ "fpn"      ] = "featureDetect" ;
    args[ "window"   ] = "OpenCV.3.0.0"  ;

    // NOTICE: obj_path is relative to executible in argv[0]!
    // The build process copies it from data folder to a data subfolder in the
    // product directory..
    
    args[ "obj_path" ] = "./data/20 USD note.png";
    args[ "dbg"      ] = "" ;
    
    v_argv.push_back( args );

#endif
    
    return v_argv;
}

// ........................................................................ main

int
main( int argc, const char * argv[] )
{
    cout << "running " << argv[0] << endl;
    
    // ................................... process args
    int err = ERR_OK;
    vector<argv_t> v_argv = process_args( argc, argv, err );
    
    if ( err != ERR_OK ){
         usage( argc, argv, err );
         return err;
    }
    
    // .............................. open input stream
    // TODO: decide where to capture from based args
    // right now its default camera
    
    VideoProcess vp(0);
    
    if (vp.is_ready()){
        vp.setup( v_argv );
    }
    
    if ( vp.is_ready() ){
         vp.print_desc( cout );
         vp.process();
    }
    
    // all done : emit errors
    if ( ERR_OK != (err = vp.get_err()) ){
        cout << vp.get_err_msg() << " (" << err << ")" << endl;
        usage( argc, argv, err );
    }

    return err;
}
