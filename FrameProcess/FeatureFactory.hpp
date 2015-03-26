// =============================================================================
//
//  FeatureFactory.h
//  ocv
//
//  Created by Guy Bar-Nahum on 2/26/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_FeatureFactory_hpp
#define ocv_FeatureFactory_hpp

// ==================================================================== includes
#include "ocvstd.hpp"

#include <opencv/cv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/tracking.hpp>

// ============================================= class FeatureDetectorFPNFactory
// The detector can be any of (see OpenCV features2d.hpp):

class FeatureFactory {

private:
    typedef enum{
        
        DEFAULT_DETECT  = -1,
        NONE            =  0,
        FAST_DETECT ,
        GFTT_DETECT ,
        MSER_DETECT ,
        ORB_DETECT  ,
        KAZE_DETECT ,
        AKAZE_DETECT,
        BRISK_DETECT,
        SIFT_DETECT ,
        SURF_DETECT ,
        STAR_DETECT ,
        FREAK_DETECT,
        
    }algo_e;
    
    static algo_e to_algo_e( string name );
    
    static const char *DETECTOR_DEFAULT  ;
    static const char *EXTRACTOR_DEFAULT ;
    static const char *MACTHER_DEFAULT   ;
    static const char *TRACKER_DEFAULT   ;
    
public:

    static Ptr<FeatureDetector>     makeDetector ( char * &name );
    static Ptr<DescriptorExtractor> makeExtractor( char * &name );
    static Ptr<DescriptorMatcher>   makeMatcher  ( char * &name );
    static Ptr<Tracker>             makeTracker  ( char * &name );
};

#endif /* defined(__ocv__FeatureFactory__) */
