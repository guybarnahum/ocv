// =============================================================================
//
//  FeatureFactory.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/26/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes
#include "ocvstd.hpp"
#include "FeatureFactory.hpp"

#include <opencv2/xfeatures2d.hpp>
using namespace xfeatures2d;

const char *FeatureFactory::DETECTOR_DEFAULT  = "surf";
const char *FeatureFactory::EXTRACTOR_DEFAULT = "surf";
const char *FeatureFactory::MACTHER_DEFAULT   = "FlannBased";
const char *FeatureFactory::TRACKER_DEFAULT   = "MIL";

// ======================================================== class FeatureFactory

// ................................................................ makeDetector
Ptr<FeatureDetector> FeatureFactory::makeDetector( char * &name)
{
    Ptr<FeatureDetector> fd;
    
    // ................................. default
    if ((        name == nullptr )||
        STR_EQ(  name, "default" ) )
                 name = (char *)FeatureFactory::DETECTOR_DEFAULT;
    
    // ................................ make it!
         if STR_EQ( name, "fast" ) fd = FastFeatureDetector::create();
    else if STR_EQ( name, "gftt" ) fd = GFTTDetector::create();
    else if STR_EQ( name, "mser" ) fd = MSER::create();
    else if STR_EQ( name, "orb"  ) fd = ORB::create();
    else if STR_EQ( name, "kaze" ) fd = KAZE::create();
    else if STR_EQ( name, "akaze") fd = AKAZE::create();
    else if STR_EQ( name, "brisk") fd = BRISK::create();
    
    // contrib - from xfeatures2d/nonfree.hpp
    else if STR_EQ( name, "sift" ) fd = SIFT::create();
    else if STR_EQ( name, "surf" ) fd = SURF::create(400);
    
    // contrib - from xfeatures2d.hpp
//    else if STR_EQ( name, "star" ) fd = StarDetector()::create();
//    else if STR_EQ( name, "freak") fd = FREAK()::create();

    return  fd;
}

// ............................................................... makeExtractor
Ptr<DescriptorExtractor> FeatureFactory::makeExtractor( char * &name)
{
    Ptr<DescriptorExtractor> de;
    
    // ................................. default
    if ((        name == nullptr )||
        STR_EQ(  name, "default" ) )
        name = (char *)FeatureFactory::EXTRACTOR_DEFAULT;

    // ................................. make it!
    
    if STR_EQ( name, "brief") de = BriefDescriptorExtractor::create();
    else de = (Ptr<DescriptorExtractor>) makeDetector( name );
    return  de;
}

// ................................................................. makeMatcher
Ptr<DescriptorMatcher> FeatureFactory::makeMatecher( char * &name)
{
    Ptr<DescriptorMatcher> dm;
    
    // ................................. default
    if ((        name == nullptr )||
        STR_EQ(  name, "default" ) )
                 name = (char *)FeatureFactory::MACTHER_DEFAULT;
     
    // ................................. make it!

    dm = DescriptorMatcher::create( name );
    return  dm;
}

// ................................................................. makeTracker
Ptr<Tracker> FeatureFactory::makeTracker( char *&name)
{
    Ptr<Tracker> tr = Tracker::create( name );
    
    // ................................. default
    if ((        name == nullptr )||
        STR_EQ(  name, "default" ) )
        name = (char *)FeatureFactory::TRACKER_DEFAULT;
    
    // ................................. make it!
    tr = Tracker::create( name );
    
    return tr;
}
