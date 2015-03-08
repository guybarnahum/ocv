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

#include "ocv.hpp"
#include "cli.hpp"

// ===================================================================== statics

// Simple usage, provide a config file that holds the setup of the
// v_argv setup information. Or invoke hardcoded v_argv by name

const string cli_parser::keys =
    "{help h usage ? |      | print this message                   }"
    "{@cfg           |      | an xml description of video pipeline }"
    "{xml            |      | xml file to save video pipeline into }"
    "{debug d dbg    |      | run in debug mode                    }";


// "{image img      |      | an argument for hardcoded pipelines  }"

// ======================================================================== errs

int cli_parser::set_err( int err, string err_msg )
{
    this->err     = err;
    this->err_msg = err_msg;
    return err;
}

int cli_parser::set_file_io_err( string path )
{
    string err_msg;
    err_msg  = "Could not open file `" ;
    err_msg += path;
    err_msg += "` for write";
    
    return set_err( ERR_FILE_IO, err_msg );
}

int cli_parser::set_incopatible_err( string path )
{
    string err_msg;
    err_msg  = "Incompatible file format `";
    err_msg += path;
    err_msg += "` - expected ";
    err_msg += XML_V_ARGV_COMP_NAME;
    err_msg += " ver ";
    err_msg += XML_V_ARGV_COMP_VERSION;
    
    return set_err( ERR_INCOMPATIBLE, err_msg );
}

// ........................................................................ init

bool cli_parser::init()
{
    bool ok = check();
    debug = has( "debug" );
    
    set_err( ERR_OK, "" );
    
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
        
        set_err(ERR_INVALID_ARGS, "invalid args" );
    }
    
    return ok;
}

// ................................................................ print_v_argv
bool cli_parser::print_v_argv()
{
    bool ok = true;
    
    for( int ix = 0; ix < v_argv.size(); ix++ ){
        argv_t *args = &v_argv[ ix ];
        
        cout << endl << endl;
        
        for( auto it  = args->begin();
                  it != args->end()  ;it++ ){
            cout << it->first << ":" << it->second << endl;
        }
    }
    
    return ok;
}

// ............................................................ import_from_file
bool cli_parser::import_from_file( string path )
{
    FileStorage fs;
    try{
        fs.open( path, FileStorage::READ );
    }
    catch( cv::Exception& e )
    {
        set_err( ERR_FILE_STORAGE, e.what() );
    }
    
    bool ok = fs.isOpened();
    
    if (!ok ) set_file_io_err( path );
    
    if ( ok ){
        string version;
        fs[ XML_V_ARGV_COMP_NAME ] >> version;
        ok = ( version == XML_V_ARGV_COMP_VERSION);
    }
    
    if (!ok ) set_incopatible_err( path );
    
    argv_t args;
    
    if ( ok ){
        
        if ( debug ) cout << "build from " << path << endl;
        
        FileNode fn = fs[ "v_argv" ];
        
        if ( debug ) cout << "<v_argv>" << endl;
        
        // iterate through a sequence using FileNodeIterator
        for( auto it  = fn.begin(); it != fn.end  (); it++ ){
            
            // construct one arg
            if ( debug ) cout << "\t<->" << endl;
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
                    
                    if ( debug ){
                        cout << "\t\t<" << key << ">"
                        << val
                        << "</" << key << ">"
                        << endl;
                    }
                }
            }
            
            if ( debug ) cout << "\t</->" << endl;
            
            v_argv.push_back( args );
        }
        
        if ( debug ) cout << "</v_argv>" << endl;
    }
    
    fs.release();
    
    return ok;
}

// .............................................................. export_to_file

bool cli_parser::export_to_file( string path )
{
    if ( debug ) cout << "generating config as `" << path << "`" << endl;
    
    FileStorage fs;
    try{
        fs.open( path, FileStorage::WRITE );
    }
    catch( cv::Exception& e )
    {
        set_err( ERR_FILE_STORAGE, e.what() );
    }
    
    bool ok = fs.isOpened();
    if (!ok ) set_file_io_err( path );
    
    if ( ok ){
        
        fs << XML_V_ARGV_COMP_NAME << XML_V_ARGV_COMP_VERSION;
        
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
