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

// ................................................................ is_printable
bool
is_printable( int key )
{
    return ( key > 32 ) && ( key < 127 );
}

// ....................................................... pre_process_one_frame
void
to_key( int key, string &s_key )
{
    
    switch( key ){
        case KEY_ESCAPE : s_key = "<escape>"; break;
        case KEY_SPACE  : s_key = "<space>" ; break;
        default         :
            if ( is_printable( key )){
                s_key = "'"; s_key += (char)key; s_key += "'";
            }
            else{
                s_key  = "<"; s_key += key; s_key += ">";
            }
            break;
    }
}

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
    string cli_str;
    
    // argv[0] is especially ugly.. clean it up
    cli_str = path_to_file( (char *)argv[ 0 ] );
    
    for( int ix = 1 ; ix < argc ; ix++ ){
        
        // has whitespace? put qoutes around it
        char ch;
        bool qoutes = false;
        const char *p = argv[ ix ];
        
        while( ( ch  = *p++ ) != '\0' )
            if ( ch <= ' '  ){ qoutes = true; break; }
        
        // print one arg
                      cli_str += " ";
        if ( qoutes ) cli_str += "\"";
                      cli_str += argv[ ix ];
        if ( qoutes ) cli_str += "\"";
    }
    
    LOG( LEVEL_INFO ) << cli_str;
}

// ==================================================================== ocvError
namespace OcvError
{

static vector<string>  err_codes;
static vector<string>  err_msg;
static err_t           err_last;

// built in errors
err_t OK;
err_t UNKNOWN;
err_t INVALID_ARGS;
err_t FILE_IO;
err_t INCOMPATIBLE;
err_t NOT_READY;
err_t OCV_FILE_STORAGE;
    
// force the following code to be executed first!
static bool init_ok = init_err();
    
bool  init_err()
{
    err_codes.clear();
    err_msg.clear();
    
    // setup common errors
    OK               = make_err( "OK" );
    UNKNOWN          = make_err( "Unknown" );
    INVALID_ARGS     = make_err( "Inavlid args" );
    FILE_IO          = make_err( "File io" );
    INCOMPATIBLE     = make_err( "Incompatible" );
    NOT_READY        = make_err( "Not ready" );
    OCV_FILE_STORAGE = make_err( "OCV FileStorage" );
    
    err_last = OK;
    return true;
}

err_t make_err( string err_msg, size_t err_code )
{
    size_t code = err_codes.size();
    
    // Can't change item 0
    if ( err_code && ( code > err_code ) ){
        code = err_code ;
        err_codes[ code ] = err_msg;
    }
    else{
        err_codes.push_back( err_msg );
    }
    
   return (err_t)(code);
}

err_t last_err()
{
    return err_last;
}

err_t set_err( err_t err, string desc )
{
    size_t err_ix = (size_t)err;
    
    if ( err_ix < err_codes.size() ){
        desc += " (" + err_codes[ err_ix ] + ")";
    }
    else{
        
        desc += " (Unknown Error:";
        desc += err_ix;
        desc += ")";
    }
    
    string *msg = new string( desc );
    err_msg.push_back( *msg );
    
    err_last = err;
    
    return err;
}

void print_err( bool reset_errs )
{
    for( size_t ix = 0 ; ix < err_msg.size(); ix++ ){
        LOG( LEVEL_ERROR ) <<  err_msg[ ix ] ;
    }
    
    if ( reset_errs ){
        err_msg.clear();
        err_last = OK;
    }
}
    
}
