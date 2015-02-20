// =============================================================================
//
//  FrameProcessNodeFactory.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes

#include "FrameProcessNodeFactory.hpp"
#include "HoughLineFPNode.hpp"
#include "CannyFPNode.hpp"


// ========================================================= FrameProcessFactory

// ........................................................................ make
// make by class name
FrameProcessNode *FrameProcessNodeFactory::make( const char* cname )
{
    FrameProcessNode *fp = nullptr;
    
    if ( 0 == strcmp( cname, "houghLine"   )  ){
        fp = (FrameProcessNode *) new HoughLineFPNode();
    }
    if ( 0 == strcmp( cname, "canny"   )  ){
        fp = (FrameProcessNode *) new CannyFPNode();
    }
    else
    if ( 0 == strcmp( cname, "nullProcess" )  ){
        fp = new FrameProcessNode();
    }
    
    return fp;
}
