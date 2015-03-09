// =============================================================================
//
//  FrameProcessNodeFactory.h
//  ocv
//
//  Created by Guy Bar-Nahum on 2/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_FrameProcessNodeFactory_hpp
#define ocv_FrameProcessNodeFactory_hpp

// ==================================================================== includes
#include "ocvstd.hpp"
#include "FrameProcessNode.hpp"

// =================================================== class FrameProcessFactory

class FrameProcessNodeFactory{
    
public:
    static FrameProcessNode *make( const char* cname );
};


#endif /* defined(ocv_FrameProcessNodeFactory_hpp) */
