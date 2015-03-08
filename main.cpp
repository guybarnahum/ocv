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
#include "cli.hpp"

#include "VideoProcess.hpp"
#include <time.h>

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

#include <sys/stat.h>

bool file_exists( string path )
{
    struct stat st;
    return ( 0 == stat( path.c_str(), &st) );
}

bool full_path( string &path )
{
    // TODO: expand ~ and .
    string user_home = getenv( "HOME" );
    
    // if relative look for it in few places
    vector<string> base_dir = { "", "./", user_home };
    
    for( size_t ix = 0 ; ix < base_dir.size() ; ix++ ){
        
        string full_path = base_dir[ ix ] + path;
        
        cout << "looking at " << full_path;
        
        if ( file_exists( full_path ) ){
            path = full_path;
            cout << " .. found!" << endl;
            return true;
        }
        
        cout << " .. not found!" << endl;
    }
    
    return false;
}

// ........................................................................ main

int
main( int argc, const char * argv[] )
{
    int err = ERR_OK;
    cli_parser cli( argc, argv );
    
    if ( ERR_OK != ( err = cli.get_err() ) ){
        cout << cli.get_err_msg() << " (" << err << ")" << endl;
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
         vp.print_desc( cout );
         vp.process();
    }
    
    // all done : emit errors
    if ( ERR_OK != ( err = vp.get_err() ) ){
        cout << vp.get_err_msg() << " (" << err << ")" << endl;
    }

    return err;
}
