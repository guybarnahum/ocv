// =============================================================================
//
//  ocvstd.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/26/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_ocvstd_hpp
#define ocv_ocvstd_hpp

// ==================================================================== includes

#include <iostream>
#include <opencv/cv.hpp>
#include <map>

using namespace cv;
using namespace std;

#include "utils.hpp"

// ======================================================================= enums

enum{
    KEY_ESCAPE = 27,
    KEY_SPACE  = 32,
};

// ======================================================================= types

typedef map<const char *, const char *> argv_t;

// ====================================================================== macros

#define STR_EQ(s,t)          ( 0 == strcmp(s,t) )
#define OCV_ASSERT           CV_Assert
#define UNUSED(v)            (void)(v)

#endif
