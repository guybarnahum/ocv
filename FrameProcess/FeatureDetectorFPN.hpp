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
    
private:
    // ................................................................... const
    const int    MIN_INLINERS_DEFAULT = 8;
    
    // ................................................................. members
    
    // ......................... ocv algorithms
    
    Ptr<FeatureDetector>     detector;
    Ptr<DescriptorExtractor> extractor;
    Ptr<DescriptorMatcher>   matcher;
    Ptr<Tracker>             tracker;
    
    // ............................... settings
    int              min_inliers;
    bool             enable_tracking;   // TODO: Remove once tracking is working
    bool             do_knn_match;
    
    // ................................. object
    // obj values are calculated once
    string           obj_path;
    Size             obj_size;
    vector<Point2f>  obj_corners;

    Mat              obj_mat;
    vector<KeyPoint> obj_keypoints;
    Mat              obj_descriptors;
    
    // scene values are calculated per frame
    Mat              scn_mat;
    vector<KeyPoint> scn_keypoints;
    Mat              scn_descriptors;
    
    // homography
    vector<DMatch> matches;

    vector<Point2f> obj_good_kpts;
    vector<Point2f> scn_good_kpts;
    
    Mat             H_rough;
    
    // output : location of object in scene
    vector<Point> pts;

    // .................................................................
    // tracking result
    typedef enum{
            NONE,
            DETECTED,
            TRACKING,
    }state_e;
    
    state_e state;
    Rect2d  scn_rect;
    
    // ................................................................. methods
    
    bool init( const char *dtct_name  = nullptr ,
               const char *xtrct_name = nullptr ,
               const char *match_name = nullptr ,
               const char *trckr_name = nullptr );
    
    bool init_detector ( const char *name = nullptr );
    bool init_extractor( const char *name = nullptr );
    bool init_matcher  ( const char *name = nullptr );
    bool init_tracker  ( const char *name = nullptr );

    bool matcher_train();

    // match .. detect .. track ..
    bool match();
    bool knn_match();
    bool detect();
    bool track();

    bool matched_keypoints();
    bool find_homography();
    bool is_valid_rect( vector<Point> &poly, double min_area = 0 );
    
    // ............................ state
    
    void set_state( state_e st ){ state = st; }
    const char * to_string( state_e st)
    {
        const char *str;
        switch( st ){
            default         : str = "UNKNOWN" ; break;
            case NONE       : str = "NONE"    ; break;
            case DETECTED   : str = "DETECTED"; break;
            case TRACKING   : str = "TRACKING"; break;
        }
        
        return str;
    }
    
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
