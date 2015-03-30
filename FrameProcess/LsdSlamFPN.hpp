// =============================================================================
//
//  LsdSlamFPN.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/27/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_LsdSlamFPN_hpp
#define ocv_LsdSlamFPN_hpp

#include "ocvstd.hpp"
#include "FrameProcessNode.hpp"
#include "SlamSystem.h"
#include "IOWrapper/Output3DWrapper.h"

// ========================================================= class LsdSlamFPNode

class LsdSlamOutput : public lsd_slam::Output3DWrapper {
    
public:
    
    // initializes cam-calib independent stuff
     LsdSlamOutput(int width, int height);
    ~LsdSlamOutput();
    
    virtual void publishKeyframeGraph( lsd_slam::KeyFrameGraph* graph);
    
    // publishes a keyframe. if that frame already existis, it is overwritten, otherwise it is added.
    virtual void publishKeyframe( lsd_slam::Frame* f);
    
    // published a tracked frame that did not become a keyframe (i.e. has no depth data)
    virtual void publishTrackedFrame( lsd_slam::Frame* f);
    
    // publishes graph and all constraints, as well as updated KF poses.
    virtual void publishTrajectory(std::vector<Eigen::Matrix<float, 3, 1>> trajectory, std::string identifier);
    
    virtual void publishTrajectoryIncrement(Eigen::Matrix<float, 3, 1> pt, std::string identifier);
    
    virtual void publishDebugInfo(Eigen::Matrix<float, 20, 1> data);

};

class LsdSlamFPNode : public FrameProcessNode {
    
private:
    
    // init state
    bool ready;
    Mat gray_mat;
    long seq_num;
    
    // slam implementation
    lsd_slam::SlamSystem *ls;
    
    // camera
    string   camera_xml;
    Mat      K; // camera_mat

    // output
    lsd_slam::Output3DWrapper *output;
    
public:
    
     LsdSlamFPNode();
    ~LsdSlamFPNode();
    bool init();
    bool resetAll();
    
    // ....................................................... overriden methods
    
    bool setup( argv_t *argv );
    bool process_one_frame();
    bool process_key( int key );

};

#endif /* defined(ocv_LsdSlamFPN_hpp) */
