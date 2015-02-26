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
    
    bool             brute_force_matcher;
    bool             draw_features;
    int              minInliers;

    Mat              gray;
    vector<KeyPoint> keypoints;
    Mat              descriptors;
    
    // tgt object
    const char      *tgt;
    Mat              tgt_mat;
    vector<KeyPoint> tgt_keypoints;
    Mat              tgt_descriptors;
    bool             tgt_bin_descriptors;
    
    // Homography
    vector<Point2f> mpts_1, mpts_2;
    vector<int>     indexes_1, indexes_2;
    
    // location of tgt object in frame
    vector<unsigned char> outlier_mask;
    vector<Point2f> rect;
    
    // .................................................................. method
 //   bool init( const char *name){ return init( name, nullptr, nullptr ); }
    
    bool init( const char *dtct_name  = nullptr ,
               const char *xtrct_name = nullptr ,
               const char *match_name = nullptr );
    
    bool init_detector ( const char *name = nullptr );
    bool init_extractor( const char *name = nullptr );
    bool init_matcher  ( const char *name = nullptr );
    
    bool match();
    bool find_homography();
    
public:
    
    FeatureDetectorFPNode( const char *name ):FrameProcessNode()
    { init( name, nullptr, nullptr ); }
    
    FeatureDetectorFPNode():FrameProcessNode()
    { init( nullptr, nullptr, nullptr ); }
    
    ~FeatureDetectorFPNode()
    {
        // algo are static pointers? should not be deleted?!
        // delete algo;
        
        delete tgt;
    }
    
    // ......................................................... virtual methods
    virtual bool process_one_frame();
    virtual bool setup( argv_t *argv );
};


#endif /* defined(ocv__FeatureDetectorFPN_hpp) */
