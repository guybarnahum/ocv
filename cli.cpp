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

err_t cli_parser::set_file_io_err( string path )
{
    string err_msg;
    err_msg  = "Could not open file '" ;
    err_msg += path;
    err_msg += "'";
    
    return set_err( FILE_IO, err_msg );
}

err_t cli_parser::set_incopatible_err( string path )
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

err_t cli_parser::set_filenode_err( FileNode *fn, string msg )
{
    uchar buff[ 256 ];
    fn->readRaw("", buff, sizeof(buff) );
    msg += "line '";
    msg += (const char *)buff;
    msg += "'";
    
    return set_err( XML_ERR, msg );
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
        ok = import_from_file( get<string>( 0 ) );
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

// .............................................................. import_one_arg
bool cli_parser::import_one_arg( FileNode &fn, string &key, string &val )
{
    key = fn.isNamed()? fn.name() : "";
    
    // valid key?
    bool ok = !key.empty();
    if (!ok){
        set_filenode_err( &fn, "argument key has to be set" );
    }
    
    if ( ok ){
        // convert by type to val string
        switch( fn.type() ){
                
            // supported types
            case FileNode::NONE : val = ""; break;
            case FileNode::INT  : val = to_string((int   )(fn)); break;
            case FileNode::FLOAT: val = to_string((double)(fn)); break;
            case FileNode::STR  : val = (string)fn; break;

            // unsupported types
            default : ok   = false;
                      set_filenode_err( &fn, "invalid value type" );
                      break;
        }
    }
    
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
        string skey; const char *key;
        string sval; const char *val;
        
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
                    
                    FileNode fn = *it_keyval;
                    ok = import_one_arg( fn, skey, sval );
                    
                    if ( ok ){
                        key = strdup( skey.c_str() );
                        val = strdup( sval.c_str() );
                        args[ key ] = val;
                    
                        LOG( LEVEL_DEBUG )
                        << "\t\t<"<< key << ">" << val << "</" << key << ">";
                    }
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
