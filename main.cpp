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

#include <sys/stat.h>

// ================================================================== file utils
// TBD: Move into some util file?!
// Can one be more corny than that?! :).
// How many times a man has to write those?
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
        
        if ( file_exists( full_path ) ){
            path = full_path;
            return true;
        }
    }
    
    return false;
}

void print_argv( int argc, const char * argv[] )
{
    // argv[0] is especially ugly.. clean it up
    cout << path_to_file( (char *)argv[ 0 ] );
    
    for( int ix = 1 ; ix < argc ; ix++ ){

        // has whitespace? put qoutes around it
        char ch;
        bool qoutes = false;
        const char *p = argv[ ix ];
        
        while( ( ch  = *p++ ) != '\0' )
            if ( ch <= ' '  ){ qoutes = true; break; }
        
        // print one arg
        if ( qoutes ) cout << " \"" << argv[ ix ] << "\"";
        else          cout << " "   << argv[ ix ];
    }
    
    cout << endl;
}

// ........................................................................ main

int
main( int argc, const char * argv[] )
{
    print_argv( argc, argv );
    
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
