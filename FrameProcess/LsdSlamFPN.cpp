// =============================================================================
//
//  LsdSlamFPN.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/27/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes

#include "LsdSlamFPN.hpp"
#include "utils.hpp"

#define LSDSLAM_NAME "LsdSlamFPNode"
#define LSDSLAM_DESC "Lsd Slam algorithm"

using namespace lsd_slam;

LsdSlamOutput::LsdSlamOutput(int width, int height)
{
}

LsdSlamOutput::~LsdSlamOutput()
{
}


void LsdSlamOutput::publishKeyframe(Frame* f)
{
    cout << "publishKeyframe" << endl;
}

void LsdSlamOutput::publishTrackedFrame(Frame* kf)
{
    cout << "publishTrackedFrame" << endl;
}

void LsdSlamOutput::publishKeyframeGraph(KeyFrameGraph* graph)
{
    cout << "publishKeyframeGraph" << endl;
}

void LsdSlamOutput::publishTrajectory(std::vector<Eigen::Matrix<float, 3, 1>> trajectory, std::string identifier)
{
    cout << "publishTrajectory" << endl;
}

void LsdSlamOutput::publishTrajectoryIncrement(Eigen::Matrix<float, 3, 1> pt, std::string identifier)
{
    // unimplemented ... do i need it?
}

void LsdSlamOutput::publishDebugInfo(Eigen::Matrix<float, 20, 1> data)
{
    cout << "publishDebugInfo" << endl;
}

// ================================================================= HoughFPNode

// ................................................................. constructor

LsdSlamFPNode::LsdSlamFPNode():FrameProcessNode()
{
    set_name( LSDSLAM_NAME );
    set_desc( LSDSLAM_DESC );
    
    ready   = false;
    ls      = nullptr;
    output  = nullptr;
}

LsdSlamFPNode::~LsdSlamFPNode()
{
    delete ls;
    delete output;
}

bool LsdSlamFPNode::init()
{
    Eigen::Matrix3f K;
    K  << 1., 0., 0.,  0., 1., 0. , 0., 0., 1.;
    int width  = gray_mat.cols;
    int height = gray_mat.rows;
    
    delete ls;     ls      = new SlamSystem   (width,height, K, true);
    delete output; output  = new LsdSlamOutput(width,height);
    
    seq_num = 0;
    
    // do sanity checks
    bool ok = (ls != nullptr);
    if ( ok ){
        ls->setVisualization( output );
    }
    return ok;
}

bool LsdSlamFPNode::resetAll()
{
    ready = false;
    return true;
}

// ....................................................................... setup

bool LsdSlamFPNode::setup( argv_t *argv )
{
    // call the parent setup for base class setup options
    // do this first so we have `dbg` option set, etc
    bool ok = FrameProcessNode::setup( argv );
    if (!ok) return false;
    
    // NOTICE : Optional settings ahead - reset ok state
    //          and start preserving errors with && ok
    
    const char *val = get_val( argv, "camera" );
    if ( val ){
        camera_xml = val;
        ok = import_camera_mat( camera_xml, K );
    }
    
    LOG( LEVEL_INFO ) << "Camera matrix: " << K;
    
    return ok;
}

// ................................................................. process_key

bool LsdSlamFPNode::process_key(int key )
{
    handleKey((char)key);
    return FrameProcessNode::process_key(key);
}

// ........................................................... process_one_frame

bool LsdSlamFPNode::process_one_frame()
{
    seq_num++;
    gray( *in, gray_mat );
    double timestamp = (double)(now_ms()) / 1000.;
    
    // need to initialize
    if(!ready || fullResetRequested ){
        ready = init();
        if ( ready ) ls->randomInit( gray_mat.data, timestamp , 1);
        fullResetRequested = false;
    }
    else{
        ls->trackFrame(gray_mat.data,seq_num,false,timestamp);
    }

    if ( window ){
        base->copyTo( out );
        window_show( window, out );
    }
    
    return ready;
}
