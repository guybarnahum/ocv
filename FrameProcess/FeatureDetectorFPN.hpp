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
typedef struct {
    
    bool             ready;
    bool             query; // is it the query in the match operation?
    
    Mat              mat;
    vector<KeyPoint> keypoints;
    Mat              descriptors;
    vector<Point2f>  good_kpts;

} FeatureDetector_ctx;

class FeatureDetectorFPNode : public FrameProcessNode {
    
protected:
    // ................................................................... const
    const int    MIN_INLINERS_DEFAULT = 8;
    
    // ................................................................. members
    
    // ocv algos
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
    
    // context
    FeatureDetector_ctx ctx1;
    FeatureDetector_ctx ctx2;
    
    FeatureDetector_ctx *src;
    FeatureDetector_ctx *dst;
    
    void swap_src_dst(){ FeatureDetector_ctx *tmp = src; src = dst; dst = tmp;}
    
    // homography
    bool            is_trained;
    vector<DMatch>  matches;
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

    // train.. match .. detect .. track ..
    bool prepare   ( FeatureDetector_ctx *ctx, bool force = false );
    bool invalidate( FeatureDetector_ctx *ctx );

    bool matcher_train( Mat desc, bool clear_old = true );
    bool match();
    bool knn_match();
    bool detect();
    bool track();
    bool find_homography();

    bool matched_keypoints();
    
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
