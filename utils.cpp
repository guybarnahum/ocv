// =============================================================================
//
//  utils.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/4/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes

#include "utils.hpp"
#include <sys/stat.h>

// ================================================================== file utils
// Can one be more corny than that?! :).
// How many times a man has to write those?

// ................................................................. file_exists
bool file_exists( string path )
{
    struct stat st;
    return ( 0 == stat( path.c_str(), &st) );
}

// ................................................................ file_to_path
bool file_to_path( string &path )
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

// ................................................................ path_to_file
// get base name from long ugly path names, don't ask me why..
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

// ================================================================= print utils

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
