// =============================================================================
//
//  ocv.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/26/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_ocv_hpp
#define ocv_ocv_hpp

#include <iostream>
#include <opencv/cv.hpp>
#include <map>

using namespace cv;
using namespace std;

// =================================================================== utilities

bool full_path( string &path );

// ====================================================================== macros
#define STR_EQ(s,t) (0 == strcmp(s,t))
#define DBG_ASSERT( cond, msg ) if ( !(cond) ) cout << msg << endl;

typedef map<const char *, const char *> argv_t;


#define ENABLE_DBG_CODE         (true)

// LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO
//
// TODO: Expolore more modern way to emit error codes and messages, maybe in a
// singlton error / logging class?
//
// In the meantime this primitive enum + msg hardcoding would do?!
//
// LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO  LAMO
enum{
    ERR_OK = 0,
    
    ERR_CAPTURE_FAILURE             = -1,
    ERR_INVALID_ARGS                = -2,
    ERR_SETUP_FRAME_PROCESS_NODE    = -3,
    ERR_FILE_IO                     = -4,
    ERR_INCOMPATIBLE                = -5,
    ERR_NOT_READY                   = -6,
    ERR_FILE_STORAGE                = -7,
};

#endif
