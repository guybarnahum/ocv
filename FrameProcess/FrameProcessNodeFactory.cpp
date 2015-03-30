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
#include "HoughFPNode.hpp"
#include "CannyFPNode.hpp"
#include "CascadeDetectorFPN.hpp"
#include "CcalibFPN.hpp"
#include "FindContour.hpp"
#include "HoughCirclesFPN.hpp"
#include "ObjectDetectorFPN.hpp"
#include "OpticalFlowFPN.hpp"
#include "LsdSlamFPN.hpp"

// ========================================================= FrameProcessFactory

// ........................................................................ make
// make by class name

FrameProcessNode *FrameProcessNodeFactory::make( const char* name )
{
    if ( name == nullptr ) return nullptr;

    FrameProcessNode *fp = nullptr;
    
    if ( STR_EQ( name, "hough"  ) )
        fp = (FrameProcessNode *) new HoughFPNode();
    else
    if ( STR_EQ( name, "canny"  ) )
        fp = (FrameProcessNode *) new CannyFPNode();
    else
    if ( STR_EQ( name, "cascadeDetect" ) )
        fp = (FrameProcessNode *) new CascadeDetectorFPNode();
    else
    if ( STR_EQ( name, "ccalib" ) )
        fp = (FrameProcessNode *) new CcalibFPNode();
    else
    if ( STR_EQ( name, "contour"   )  )
        fp = (FrameProcessNode *) new FindContour();
    else
    if ( STR_EQ( name, "objectDetect" ) )
        fp = (FrameProcessNode *) new ObjectDetectorFPNode();
    else
    if ( STR_EQ( name, "opticalFlow"   )  )
        fp = (FrameProcessNode *) new OpticalFlowFPNode();
    else
    if ( STR_EQ( name, "houghCircles"   )  )
        fp = (FrameProcessNode *) new HoughCirclesFPN();
    else
    if ( STR_EQ( name, "nullProcess" )  ){
        fp = new FrameProcessNode();
    }
    else
    if ( STR_EQ( name, "lsdSlam" ) ){
        fp = new LsdSlamFPNode();
    }
    return fp;
}
