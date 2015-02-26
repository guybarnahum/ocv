// =============================================================================
//
//  utils.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/20/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#include "utils.hpp"

AppOptions::AppOptions()
{
    add( '?' );
    
}

AppOptions::~AppOptions()
{
    // deep free oi structures from map..
    option_info.clear();
}

void
AppOptions::add( const char   name   , const char   *lname ,
                 option_type_e type  , option_req_e  req   ,
                 const char   *val   )
{
    option_info_t oi;
    oi.name      =  name;
    oi.lname     = (lname != nullptr)? strdup( lname ) : nullptr;
    oi.type      =  type;
    oi.req       =  req;
    oi.val       = (val != nullptr)? strdup( val ) : nullptr;;
    
    oi.argc = -1;
    oi.err  = OPTION_ERR_OK;
    
    // TODO: Check for existing? Assert already defined
    option_info[ name ] = oi;
}

bool        set( const char name , const char   *val );
const char *get( const char name );

int   process( int argc, const char *argv[] );
void  print_errors( ostream &out_stream, int argc, const char *argv[] );

namespace utils {


}
