// =============================================================================
//
//  cli.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/6/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_cli_hpp
#define ocv_cli_hpp

// ==================================================================== includes

#include "ocvstd.hpp"

// ================================================================== cli_parser

class cli_parser : public CommandLineParser {

private:
    
    static const string ver_key;
    static const string ver_val;
    static const string keys;
    
    vector<argv_t>  v_argv;
    string          path;
    
    bool            debug;

    // .................................................................... errs
    
    err_t set_file_io_err    ( string   path );
    err_t set_incopatible_err( string   version );
    err_t set_filenode_err   ( FileNode *fn, string msg );

public:
    
    // ............................................................. constructor
    cli_parser( int argc, const char * argv[] ) :
    CommandLineParser( argc, argv, cli_parser::keys ){ init(); };
    
    bool init();
    bool setup_from_file();

    // .............................................................. destructor
    // TODO: FIXME: mem leak free v_argv!
    ~cli_parser(){}
    
    bool import_from_file( string path );
    bool export_to_file  ( string path );
    
    bool import_one_arg( FileNode &fn, string &key, string &val );

    vector<argv_t> *get_v_argv(){ return &v_argv; }
    bool print_v_argv();
    
};

#endif /* defined(ocv_cli_hpp) */
