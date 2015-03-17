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

#include "ocvstd.hpp"
#include "FeatureFactory.hpp"
#include "FrameProcessNode.hpp"

// ================================================= class FeatureDetectorFPNode

class FeatureDetectorFPNode : public FrameProcessNode {
    
protected:
    // ................................................................... const
    const int    MIN_INLINERS_DEFAULT = 8;
    
    // ................................................................. members
    
    // ......................... ocv algorithms
    
    Ptr<FeatureDetector>     detector;
    Ptr<DescriptorExtractor> extractor;
    Ptr<DescriptorMatcher>   matcher;
    Ptr<Tracker>             tracker;
    
    // settings
    int              min_inliers;
    bool             enable_tracking;   // TODO: Remove once tracking is working
    bool             do_knn_match;
    bool             do_refine_homography;
    const char*      dbg_window;
    
    Mat              src_mat;
    vector<KeyPoint> src_keypoints;
    Mat              src_descriptors;
    
    // scene values are calculated per frame
    Mat              dst_mat;
    vector<KeyPoint> dst_keypoints;
    Mat              dst_descriptors;
    
    // homography
    bool            is_trained;
    vector<DMatch>  matches;

    vector<Point2f> src_good_kpts;
    vector<Point2f> dst_good_kpts;
    Mat             matches_mat;

    Mat             H_rough;
    Mat             H;
        
    // ................................................................. methods
    
    bool init( const char *dtct_name  = nullptr ,
               const char *xtrct_name = nullptr ,
               const char *match_name = nullptr ,
               const char *trckr_name = nullptr );
    
    bool init_detector ( const char *name = nullptr );
    bool init_extractor( const char *name = nullptr );
    bool init_matcher  ( const char *name = nullptr );
    bool init_tracker  ( const char *name = nullptr );

    // match .. detect .. track ..
    bool match();
    bool knn_match();
    bool detect();
    bool track();

    bool matched_keypoints();
    bool find_homography();
        
public:
    
    // ................................................ constractor / destractor
    
    FeatureDetectorFPNode( char *name ):FrameProcessNode()
    { init( name, nullptr, nullptr ); }
    
    FeatureDetectorFPNode():FrameProcessNode()
    { init( nullptr, nullptr, nullptr ); }
    
    ~FeatureDetectorFPNode(){}
    
    // ......................................................... virtual methods
    virtual bool process_one_frame();
    virtual bool setup( argv_t *argv );
};

#endif /* defined(ocv__FeatureDetectorFPN_hpp) */
