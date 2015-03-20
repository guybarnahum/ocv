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

// BruteForce (it uses L2 )
// BruteForce-L1
// BruteForce-Hamming
// BruteForce-Hamming(2)
// FlannBased

const char *FeatureFactory::MACTHER_DEFAULT   = "FlannBased";
const char *FeatureFactory::TRACKER_DEFAULT   = "MIL";

// ======================================================== class FeatureFactory

FeatureFactory::algo_e FeatureFactory::to_algo_e( string name )
{
    // to lower
    for( size_t ix = 0; ix < name.length(); ix++ ){
        name[ ix ] = tolower( name[ ix ] );
    }
    
    // common
    if ( name == "sift"  ) return SIFT_DETECT ;
    if ( name == "orb"   ) return ORB_DETECT  ;

    // rarely if ever used..
    if ( name == "surf"  ) return SURF_DETECT ;
    if ( name == "fast"  ) return FAST_DETECT ;
    if ( name == "mser"  ) return MSER_DETECT ;
    if ( name == "kaze"  ) return KAZE_DETECT ;
    if ( name == "akaze" ) return AKAZE_DETECT;
    if ( name == "brisk" ) return BRISK_DETECT;
    if ( name == "gftt"  ) return GFTT_DETECT ;
    if ( name == "star"  ) return STAR_DETECT ;
    if ( name == "freak" ) return FREAK_DETECT;

    return NONE;
}

// ................................................................ makeDetector
Ptr<FeatureDetector> FeatureFactory::makeDetector( char * &name )
{
    Ptr<FeatureDetector> fd;
    
    // ................................. default
    if ((        name == nullptr )||
        STR_EQ(  name, "default" ) )
                 name = (char *)FeatureFactory::DETECTOR_DEFAULT;
    
    FeatureFactory::algo_e algo = to_algo_e( string(name) );
    
    switch( algo ){
        case FAST_DETECT  : fd = FastFeatureDetector::create(); break;
        case GFTT_DETECT  : fd = GFTTDetector::create(); break;
        case MSER_DETECT  : fd = MSER::create(); break;
        case ORB_DETECT   : fd = ORB::create(); break;
        case KAZE_DETECT  : fd = KAZE::create(); break;
        case AKAZE_DETECT : fd = AKAZE::create(); break;
        case BRISK_DETECT : fd = BRISK::create(); break;
            
        // contrib - from xfeatures2d/nonfree.hpp
        // these are not free for commercial use! Use ORB (FAST modified)
        // for an open source alternative

        case SIFT_DETECT  : fd = SIFT::create(); break;
        case SURF_DETECT  : fd = SURF::create(400); break;
        
        default:    LOG( LEVEL_WARNING ) << "Unknown algo_e(" << name << ":"
                                                              << algo << ")";
                    // fall through!
        // contrib - from xfeatures2d.hpp
        case STAR_DETECT  : // fd = StarDetector()::create();
        case FREAK_DETECT : // fd = FREAK()::create();
        case NONE         :
                    LOG( LEVEL_ERROR ) << "Unsupported algo (" << name << ")";
                    break;
    }

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
Ptr<DescriptorMatcher> FeatureFactory::makeMatcher( char * &name,
                                                    const char * detct )
{
    UNUSED( detct );
    Ptr<DescriptorMatcher> dm;
    
    // ................................. default
    if ((        name == nullptr )||
        STR_EQ(  name, "default" ) )
                 name = (char *)FeatureFactory::MACTHER_DEFAULT;
    
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
