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
#include "FeatureDetectorFPN.hpp"

// ========================================================= FrameProcessFactory

// ........................................................................ make
// make by class name
FrameProcessNode *FrameProcessNodeFactory::make( const char* name )
{
    FrameProcessNode *fp = nullptr;

    if ( STR_EQ( name, "houghLine"   )  )
        fp = (FrameProcessNode *) new HoughLineFPNode();
    else
    if ( STR_EQ( name, "canny"       )  )
        fp = (FrameProcessNode *) new CannyFPNode();
    else
    if ( STR_EQ( name, "featureDetect"       )  )
        fp = (FrameProcessNode *) new FeatureDetectorFPNode();
    else
    if ( STR_EQ( name, "nullProcess" )  ){
        fp = new FrameProcessNode();
    }
    
    return fp;
}
