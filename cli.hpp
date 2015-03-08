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

#include <stdio.h>

#define XML_V_ARGV_COMP_NAME    "xml_v_argv"
#define XML_V_ARGV_COMP_VERSION "1.0"

class cli_parser : public CommandLineParser {

private:
    
    static const string keys;
    
    vector<argv_t>  v_argv;
    string          path;
    
    int             err;
    string          err_msg;
    bool            debug;
    
public:
    
    // ............................................................. constructor
    cli_parser( int argc, const char * argv[] ) :
    CommandLineParser( argc, argv, cli_parser::keys ){ init(); };
    
    bool init();
    bool setup_from_file();
    bool setup_hardcoded();

    // .............................................................. destructor
    // TODO: FIXME: mem leak free v_argv!
    ~cli_parser(){}
    
    bool import_from_file( string path );
    bool export_to_file  ( string path );
    
    vector<argv_t> *get_v_argv(){ return &v_argv; }
    bool print_v_argv();
    
    // .................................................................... errs
    int     set_err( int err, string err_msg );
    int     get_err    (){ return err    ; }
    string  get_err_msg(){ return err_msg; }
    
    int     set_file_io_err( string path );
    int     set_incopatible_err( string version );
};

#endif /* defined(ocv_cli_hpp) */
