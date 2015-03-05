// =============================================================================
//
//  FeatureDetectorFPN.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/21/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes
#ifndef ocv__FeatureDetectorFPN_hpp
#define ocv__FeatureDetectorFPN_hpp

#include "ocv.hpp"
#include "FrameProcessNode.hpp"

// ================================================= class FeatureDetectorFPNode


class FeatureDetectorFPNode : public FrameProcessNode {
    
private:
    
    // ................................................................. members
    
    Ptr<FeatureDetector>     detector;
    Ptr<DescriptorExtractor> extractor;
    Ptr<DescriptorMatcher>   matcher;
    
    // settings
    bool             brute_force_matcher;
    bool             draw_features;
    int              min_inliers;
    
    // object
    const char      *obj_path;
    Mat              obj_mat;
    vector<KeyPoint> obj_keypoints;
    Mat              obj_descriptors;
    
    // scene or frame
    Mat              scn_mat;
    vector<KeyPoint> scn_keypoints;
    Mat              scn_descriptors;

    // homography
    vector<Point2f> obj_good_kpts;
    vector<Point2f> scn_good_kpts;
    
    // output : location of object in scene
    vector<Point2f> scn_rect;
    
    // .................................................................. method
 //   bool init( const char *name){ return init( name, nullptr, nullptr ); }
    
    bool init( const char *dtct_name  = nullptr ,
               const char *xtrct_name = nullptr ,
               const char *match_name = nullptr );
    
    bool init_detector ( const char *name = nullptr );
    bool init_extractor( const char *name = nullptr );
    bool init_matcher  ( const char *name = nullptr );
    
    bool match();
    
    bool is_valid_rect( vector<Point2f> &poly, double min_area = 0 );
    bool find_homography();
    
public:
    
    FeatureDetectorFPNode( char *name ):FrameProcessNode()
    { init( name, nullptr, nullptr ); }
    
    FeatureDetectorFPNode():FrameProcessNode()
    { init( nullptr, nullptr, nullptr ); }
    
    ~FeatureDetectorFPNode()
    { delete obj_path; }
    
    // ......................................................... virtual methods
    virtual bool process_one_frame();
    virtual bool setup( argv_t *argv );
};


#endif /* defined(ocv__FeatureDetectorFPN_hpp) */
