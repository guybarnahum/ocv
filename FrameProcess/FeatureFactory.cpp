// =============================================================================
//
//  FeatureFactory.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/26/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================
#include "ocv.hpp"
#include "FeatureFactory.hpp"

#include <opencv2/xfeatures2d.hpp>
using namespace xfeatures2d;

const char *FeatureFactory::DETECTOR_DEFAULT  = "sift";
const char *FeatureFactory::EXTRACTOR_DEFAULT = "brief";
const char *FeatureFactory::MACTHER_DEFAULT   = "FlannBased";

// ======================================================== class FeatureFactory

// ................................................................ makeDetector
Ptr<FeatureDetector> FeatureFactory::makeDetector( const char * name)
{
    Ptr<FeatureDetector> fd;
    
    // ................................. default
    if ((        name == nullptr )||
        STR_EQ(  name, "default" ) ) name = FeatureFactory::DETECTOR_DEFAULT;
    
    // ................................
         if STR_EQ( name, "fast" ) fd = FastFeatureDetector::create();
    else if STR_EQ( name, "gftt" ) fd = GFTTDetector::create();
    else if STR_EQ( name, "mser" ) fd = MSER::create();
    else if STR_EQ( name, "orb"  ) fd = ORB::create();
    else if STR_EQ( name, "kaze" ) fd = KAZE::create();
    else if STR_EQ( name, "akaze") fd = AKAZE::create();
    else if STR_EQ( name, "brisk") fd = BRISK::create();
    
    // contrib - from xfeatures2d/nonfree.hpp
    else if STR_EQ( name, "sift" ) fd = SIFT::create();
    else if STR_EQ( name, "surf" ) fd = SURF::create(600.0);
    
    // contrib - from xfeatures2d.hpp
//    else if STR_EQ( name, "star" ) fd = StarDetector()::create();
//    else if STR_EQ( name, "freak") fd = FREAK()::create();

    return  fd;
}

// ............................................................... makeExtractor
Ptr<DescriptorExtractor> FeatureFactory::makeExtractor( const char * name)
{
    Ptr<DescriptorExtractor> de;
    
    // ................................. default
    if ((        name == nullptr )||
        STR_EQ(  name, "default" ) ) name = FeatureFactory::EXTRACTOR_DEFAULT;
    
    // ................................
    if STR_EQ( name, "brief") de = BriefDescriptorExtractor::create();

    return  de;
}

// ................................................................. makeMatcher
Ptr<DescriptorMatcher> FeatureFactory::makeMatecher( const char * name)
{
    Ptr<DescriptorMatcher> dm;
    
    // ................................. default
    if ((        name == nullptr )||
        STR_EQ(  name, "default" ) ) name = FeatureFactory::MACTHER_DEFAULT;
 
    string type( name );
    
    dm = DescriptorMatcher::create( type );
    return  dm;
}
