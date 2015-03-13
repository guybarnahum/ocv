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
    const double SCALE_FACTOR         = 1.0;
    
    // ................................................................. members
    
    Ptr<FeatureDetector>     detector;
    Ptr<DescriptorExtractor> extractor;
    Ptr<DescriptorMatcher>   matcher;
    Ptr<Tracker>             tracker;
    
    // settings
    int              min_inliers; // when to reject match results
    double           scale;       // scale down video frame
    bool             do_track;    // TODO: Remove once tracking is working
    
    // object
    string           obj_path;
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
    // detector result
    vector<Point2f> scn_poly;

    // tracking result
    typedef enum{
            NONE,
            DETECTED,
            TRACKING,
    }state_e;
    
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
    
    state_e last_state;
    state_e state;
    
    void set_state( state_e st ){ last_state = state; state = st; }
    bool state_changed(){ return last_state != state; }
    
    Rect2d  scn_rect;
    
    // .................................................................. method
 //   bool init( const char *name){ return init( name, nullptr, nullptr ); }
    
    bool init( const char *dtct_name  = nullptr ,
               const char *xtrct_name = nullptr ,
               const char *match_name = nullptr ,
               const char *trckr_name = nullptr );
    
    bool init_detector ( const char *name = nullptr );
    bool init_extractor( const char *name = nullptr );
    bool init_matcher  ( const char *name = nullptr );
    bool init_tracker  ( const char *name = nullptr );
    
    bool is_valid_rect( vector<Point2f> &poly, double min_area = 0 );
    bool find_homography();

    // match .. detect .. track ..
    bool match();
    bool detect();
    bool track();
    
public:
    
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
