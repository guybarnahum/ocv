//
//  utils.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/20/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//

#ifndef ocv_utils_hpp
#define ocv_utils_hpp

#include <stdio.h>
#include <ostream>

#include <map>

using namespace std;

// ============================================================ class AppOptions
class AppOptions{

private:
    
    // Flags that indicate if an option and its value are optional or required
    typedef enum{
        OPTION_NOT_REQUIRED   = 1,
        OPTION_REQUIRED       = 2,
        OPTION_VALUE_REQUIRED = 4,
    }option_req_e;
    
    // Various errors for options
    typedef enum{
        
        OPTION_ERR_OK            =  0,
        OPTION_ERR_ARG_TYPE      = -1,
        OPTION_ERR_REQUIRED      = -2,
        OPTION_ERR_VALUE_MISSING = -3,
        
    }option_err_e;
    
    // Option value types
    typedef enum{
        
        OPTION_TYPE_VAL_NONE    = 0,
        OPTION_TYPE_VAL_STRING  ,
        OPTION_TYPE_VAL_INT     ,
        OPTION_TYPE_VAL_FLOAT
        
    }option_type_e;
    
    // Descriptor structure for each option
    typedef struct {
        
        char          name;
        char*         lname;
        option_type_e type;
        char*         val;
        option_req_e  req;
        
        int           argc;
        option_err_e  err;
        
    } option_info_t;

    // Map between letters to option descriptors
    map<char,option_info_t> option_info;

public:
    
     AppOptions();
    ~AppOptions();
    
    void add( const char    name                        ,
              const char   *long_name = nullptr         ,
              option_type_e type  = OPTION_TYPE_VAL_NONE,
              option_req_e  req   = OPTION_NOT_REQUIRED ,
              const char   *val   = nullptr             );
    
    bool        set( const char name , const char   *val );
    const char *get( const char name );
    
    int   process( int argc, const char *argv[] );
    void  print_errors( ostream &out_stream, int argc, const char *argv[] );
};

namespace utils {
    
}

#endif /* defined(ocv_utils_hpp) */
