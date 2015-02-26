//
//  ocv.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/26/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//

#ifndef ocv_ocv_hpp
#define ocv_ocv_hpp

#include <iostream>
#include <opencv/cv.hpp>
#include <map>

using namespace cv;
using namespace std;

// ====================================================================== macros
#define STR_EQ(s,t) (0 == strcmp(s,t))

typedef map<const char *, const char *> argv_t;

#endif