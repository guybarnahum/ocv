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

#include "ocv.hpp"

#include <opencv/cv.hpp>
#include <opencv2/features2d.hpp>

// ============================================= class FeatureDetectorFPNFactory
// The detector can be any of (see OpenCV features2d.hpp):

class FeatureFactory {

private:
    static const char *DETECTOR_DEFAULT  ;
    static const char *EXTRACTOR_DEFAULT ;
    static const char *MACTHER_DEFAULT   ;

public:

    static Ptr<FeatureDetector>     makeDetector ( char * &name );
    static Ptr<DescriptorExtractor> makeExtractor( char * &name );
    static Ptr<DescriptorMatcher>   makeMatecher ( char * &name );
};

#endif /* defined(__ocv__FeatureFactory__) */
