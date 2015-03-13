// =============================================================================
//
//  cli.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/6/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes
#include "ocvstd.hpp"
#include "cli.hpp"

// ===================================================================== statics

const string cli_parser::ver_key = "xml_v_argv";
const string cli_parser::ver_val = "1.0";

// Simple usage, provide a config file that holds the setup of the
// v_argv setup information. Or invoke hardcoded v_argv by name

const string cli_parser::keys =
    "{help h usage ? |      | print this message                   }"
    "{@cfg           |      | an xml description of video pipeline }"
    "{@path          |""    | an optional path argument            }"
    "{x xml          |      | xml file to save video pipeline into }"
    "{debug d dbg    |      | run in debug mode                    }";


// "{image img      |      | an argument for hardcoded pipelines  }"

// ======================================================================== errs

OcvError::err_t cli_parser::set_file_io_err( string path )
{
    string err_msg;
    err_msg  = "Could not open file '" ;
    err_msg += path;
    err_msg += "'";
    
    return set_err( FILE_IO, err_msg );
}

OcvError::err_t cli_parser::set_incopatible_err( string path )
{
    string err_msg;
    err_msg  = "Incompatible file format `";
    err_msg += path;
    err_msg += "` - expected ";
    err_msg += cli_parser::ver_key;
    err_msg += " ver ";
    err_msg += cli_parser::ver_val;
    
    return set_err( INCOMPATIBLE, err_msg );
}

// ........................................................................ init

bool cli_parser::init()
{
    bool ok = check();
    debug = has( "debug" );
    
    // handle usage
    if ( has( "help" ) ){
        printMessage();
        return ok;
    }
    
    if (ok){
        ok = setup_hardcoded();
        
        // not found cfg hardcoded pipeline -- try to treat cfg as a xml file
        if (!ok){
            ok = import_from_file( get<string>( 0 ) );
        }
    }
    
    // if we are valid respect request to save v_argv into an xml file
    if ( ok && has( "xml" ) ){
        
        // LAME
        // TODO/FIXME: How do you pass a file name for the -xml option?!
        //      (-xml=/path/to/file does not work!!)
        //       lame workaround is to save it by the ocv executable as
        //       <cfg>.xml file.. shameful..
        //
        string path = get<string>( 0 ) + ".xml";
        ok = export_to_file( path );
    }
    
    if (!ok){
        
        printErrors();
        printMessage();
        
        set_err( INVALID_ARGS );
    }
    
    return ok;
}

// ................................................................ print_v_argv
bool cli_parser::print_v_argv()
{
    bool ok = true;
    string v_argv_str = "";
    
    for( int ix = 0; ix < v_argv.size(); ix++ ){
        argv_t *args = &v_argv[ ix ];
        
        v_argv_str += "\n\n";
        
        for( auto it  = args->begin();
                  it != args->end()  ;it++ ){
            
            v_argv_str += it->first ;
            v_argv_str += ":"       ;
            v_argv_str += it->second;
            v_argv_str += "\n";
        }
    }
    
    LOG( LEVEL_INFO ) << v_argv_str;
    
    return ok;
}

// ............................................................ import_from_file
bool cli_parser::import_from_file( string path )
{
    file_to_path( path );
    
    FileStorage fs;
    try{
        fs.open( path, FileStorage::READ );
    }
    catch( Exception& e )
    {
        set_err( OCV_FILE_STORAGE, e.what() );
    }
    
    // can we read the file?
    bool ok = fs.isOpened();
    if (!ok ){
        set_file_io_err( path );
    }

    // is it compatible?
    if ( ok ){
        string version;
        fs[ cli_parser::ver_key ] >> version;
        ok = ( version == cli_parser::ver_val );
        if (!ok ) set_incopatible_err( path );
    }
    
    if ( ok ){
        argv_t args;
    
        LOG( LEVEL_DEBUG ) << "build from " << path ;
        
        FileNode fn = fs[ "v_argv" ];
        
        LOG( LEVEL_DEBUG ) << "<v_argv>" ;
        
        // iterate through a sequence using FileNodeIterator
        for( auto it  = fn.begin(); it != fn.end  (); it++ ){
            
            // construct one arg
            LOG( LEVEL_DEBUG ) << "\t<->" ;
            args.clear();

            // build an arg into v_argv
            
            for( auto it_arg  = (*it).begin();
                      it_arg != (*it).end  (); it_arg++ ){
                
                for( auto it_keyval  = (*it_arg).begin();
                          it_keyval != (*it_arg).end  (); it_keyval++ ){
                    
                    String name ((*it_keyval).name());
                    String str  ( *it_keyval );
                    
                    const char *key = strdup( name.c_str() );
                    const char *val = strdup( str.c_str () );
                    
                    args[ key ] = val;
                    
                    LOG( LEVEL_DEBUG )
                        << "\t\t<"<< key << ">" << val << "</" << key << ">";
                }
            }
            
            LOG( LEVEL_DEBUG ) << "\t</->" ;
            
            v_argv.push_back( args );
        }
        
        LOG( LEVEL_DEBUG ) << "</v_argv>" ;
    }
    
    fs.release();
    
    return ok;
}

// .............................................................. export_to_file

bool cli_parser::export_to_file( string path )
{
    LOG( LEVEL_DEBUG ) << "generating config as `" << path << "`" ;
    
    FileStorage fs;
    try{
        fs.open( path, FileStorage::WRITE );
    }
    catch( Exception& e )
    {
        set_err( OCV_FILE_STORAGE, e.what() );
    }
    
    bool ok = fs.isOpened();
    if (!ok ) set_file_io_err( path );
    
    if ( ok ){
        
        fs << cli_parser::ver_key << cli_parser::ver_val;
        
        // save vector args
        fs << "v_argv" << "[";
        for( size_t ix = 0 ; ix < v_argv.size(); ix++ ){
            argv_t *arg = &v_argv[ ix ];
            // save arg map key value pairs
            fs << "{";
            for( auto it = arg->begin(); it != arg->end(); it++ ){
                fs << it->first << it->second;
            }
            fs << "}";
        }
        
        fs << "]";
    }
    
    fs.release();
    
    return ok;
}

// ............................................................. setup_hardcoded

bool cli_parser::setup_hardcoded()
{
    path  = get<string>(0);
    debug = has( "debug" );
    argv_t args;
    
    // assume that cfg is a valid hardcoded option
    bool ok = true;
    
    // assume that cfg is hardcoded video pipeline
    if ( path == "hough-lines" ){
        
        args[ "fpn"    ] = "canny";
        args[ "window" ] = "Canny";
        
        if ( debug ) args[ "dbg" ] = "";
        v_argv.push_back( args );
        
        args[ "fpn"    ] = "houghLine";
        args[ "window" ] = "HoughLines";
        
        if ( debug ) args[ "dbg" ] = "";
        v_argv.push_back( args );
    }
    else
    if ( path == "feature-detect" ){
            
        args[ "fpn"      ] = "featureDetect" ;
        args[ "window"   ] = "OpenCV.3.0.0"  ;
            
        if ( has( "image" ) ){
            string obj_path = get<string>( "image" );
            // TODO: fix memory leak below!
            args[ "obj_path" ] = strdup( obj_path.c_str() );
        }
        // use default?
        else{
        // NOTICE: obj_path is relative to executible in argv[0]!
        // The build process copies it from data folder to a data
        // subfolder in the product directory..
            args[ "obj_path" ] = "data/20 USD note.png";
        // args[ "obj_path" ] = "./data/black diamond.png";
        // args[ "obj_path" ] = "./data/iittala_owl.jpg";
        // args[ "obj_path" ] = "./data/flor.jpg";
        // args[ "obj_path" ] = "./data/space monkey.jpg";
        }
        
        if ( debug ) args[ "dbg" ] = "";
            v_argv.push_back( args );
        }
        else{
            ok = false;
        }
    
    // ok means that harcode setup is a success
    return ok;
}
