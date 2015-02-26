// =============================================================================
//
//  FeatureDetectorFPN.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/21/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#include <sstream>
#include "FeatureDetectorFPN.hpp"
#include "FeatureFactory.hpp"

// ........................................................................ init

bool FeatureDetectorFPNode::init_detector( const char *name )
{
    detector  = FeatureFactory::makeDetector ( name );
    
    bool ok = !detector.empty();
    
    if ( ok ){
        // .............................. set name / desc
        string str;
        str  = "FeatureDetectorFPNode(";
        str += name;
        str += ")";
        set_name( str.c_str() );
        
        str  = "`";
        str += name;
        str += "` algorithm feature detector";
        set_desc( str.c_str() );
    }
    else{
        err  = "Invalid Feature Detector (";
        err += name;
        err += ")";
    }
    
    return ok;
}

bool FeatureDetectorFPNode::init_extractor( const char *name )
{
    extractor = detector;
    return !extractor.empty();
}

bool FeatureDetectorFPNode::init_matcher  ( const char *name  )
{
    matcher = FeatureFactory::makeMatecher( name );
    bool ok = !matcher.empty();
    
    if ( ok ){
        string desc( get_desc() );
        
        desc += " matcher type `";
        desc += name? name : "default";
        desc += "`";
        
        set_desc( desc.c_str() );
    }
    
    return ok;
}


bool FeatureDetectorFPNode::init( const char *dtct_name  ,
                                  const char *xtrct_name ,
                                  const char *match_name )
{
    bool      ok = init_detector ( dtct_name  );
    if ( ok ) ok = init_extractor( xtrct_name );
    if ( ok ) ok = init_matcher  ( match_name );
    
    // ................. args may override these defaults
    tgt                 = nullptr;
    draw_features       = false;
    minInliers          = 8;
    
    return ok;
}

// ....................................................................... setup
bool FeatureDetectorFPNode::setup( argv_t *argv )
{
    const char *val;
    bool ok = (argv != nullptr );
    if (!ok){
        cout << "no setup argv provided for " << get_name() << endl;
        return false;
    }
    
    // ------------------------------- Required arguments
    
    // ...................................... algo option
    // algo may have a valid default so its really
    // semi-required if there is such a thing..
    
    val = get_val( argv, "algo" );
    ok = ( val != nullptr );
    if ( ok ){
         ok = init_detector( val );
    }
    
    // we better have a valid algo feature detector
    // regardless of how we got it from the option or default..
    ok = !detector.empty();
    
    if (!ok){
        
        if (val != nullptr){
            err = "Invalid `algo` option (" ;
            err += val;
            err += ")";
        }
        else{
            err = "Missing required option `algo`";
        }
        
        return false;
    }
    
    // ....................................... tgt option
    val = get_val( argv, "tgt" );
    
    ok = ( val != nullptr );
    tgt = ok? strdup( val ) : nullptr;
    
    if (ok){
        tgt_mat = imread( tgt, IMREAD_GRAYSCALE );
        ok = !tgt_mat.empty();
    }
    
    if (ok){
        detector->detect( tgt_mat, tgt_keypoints );
        detector->compute(tgt_mat, tgt_keypoints , tgt_descriptors);
    }
    
    // we better have a valid tgt object
    if (!ok){
        
        if (val != nullptr){
            err = "Invalid `tgt` option (" ;
            err += val;
            err += ")";
        }
        else{
            err = "Missing required option `tgt`";
        }
        
        return false;
    }
    // ------------------------------ Optional arguments
    
    // ..................................... match option
    val = get_val( argv, "matcher" );
    if (val != nullptr){
         ok = init_matcher( val );
    }
    if (!ok){
        err  = "invalid matcher option (";
        err += val;
        err += ")";
        return false;
    }
    
    // ............................ draw_features option
    val = get_val( argv, "draw_features" );
    if (val != nullptr){
        draw_features = !STR_EQ(val, "false");
    }

    // call the parent setup for base class setup options
    return FrameProcessNode::setup( argv );
}

bool FeatureDetectorFPNode::match()
{
    bool ok = !matcher.empty();
    
    if (!ok){
        err = "empty macher!";
        return false;
    }
    
    int k = 2; // find the 2 nearest neighbors
    vector<vector<DMatch> > matches;
    
    matcher->knnMatch( tgt_descriptors, descriptors, matches, k);
        
    // process nearest neighbors results
    float nndr_ratio = 0.6;
    int     pt_ix ;
    Point2f pt    ;

    for(int ix=0; ix< matches.size(); ix++){
            
        // in case of binary, just take the nearest
        if( tgt_bin_descriptors                          ||
            ( matches.at(ix).at(0).distance            <=
              matches.at(ix).at(1).distance * nndr_ratio ) )
        {
            pt_ix = matches.at( ix ).at( 0 ).queryIdx;
            pt    = keypoints.at( pt_ix ).pt;
                
            mpts_1.push_back   ( pt    );
            indexes_1.push_back( pt_ix );
                
            pt_ix = matches.at( ix ).at( 0 ).trainIdx;
            pt    = tgt_keypoints.at( pt_ix ).pt;
                
            mpts_2.push_back   ( pt    );
            indexes_2.push_back( pt_ix );
        }
    }
    

    return ok;
}

#if 0
// FLANN - Nearest neighbor matching using FLANN library (included in OpenCV)
bool FeatureDetectorFPNode::match_flann()
{
    cv::Mat results;
    cv::Mat dists;
    
    int k=2; // find the 2 nearest neighbors
    flann::Index *fix = nullptr;
    
    switch( tgt_descriptors.type() ){
            
        case CV_8U: // Create Flann LSH index
            // Binary descriptors detected (from ORB, Brief, BRISK, FREAK)
            tgt_bin_descriptors = true;
        
            fix = new flann::Index( descriptors,
                                    flann::LshIndexParams(12, 20, 2),
                                    cvflann::FLANN_DIST_HAMMING     );
            break;
            
        case CV_32F: // Create Flann KDTree index
            tgt_bin_descriptors = false;
            fix = new flann::Index( descriptors,
                                    flann::KDTreeIndexParams()  ,
                                    cvflann::FLANN_DIST_EUCLIDEAN );
            break;
    }
    
    bool ok = ( fix != nullptr );
    
    if ( ok ){
        // Distance results are stored in CV_32FC1 ?!?!?
        // NOTE OpenCV doc is not clear about that...
        results = Mat( descriptors.rows, k, CV_32SC1 ); // Results index
        dists   = Mat( descriptors.rows, k, CV_32FC1 );
            
        // ...................................... search
        fix->knnSearch( descriptors, results, dists, k, flann::SearchParams() );
    
        // ............ process nearest neighbors results
        float nndr_ratio = 0.6;
        Point2f pt    ;
        int     pt_ix ;
        
        for(int ix=0; ix< tgt_descriptors.rows; ix++){

            // in case of binary, just take the nearest
            if( tgt_bin_descriptors                   ||
                ( dists.at<float>(ix,0)            <=
                  dists.at<float>(ix,1) * nndr_ratio ) ){
            
                mpts_1.push_back   ( tgt_keypoints.at(ix).pt );
                indexes_1.push_back( ix );
                
                pt_ix = results.at<int>(ix,0);
                pt    = keypoints.at( pt_ix ).pt;
                        
                mpts_2.push_back   ( pt    );
                indexes_2.push_back( pt_ix );
            }
        }
    }
    // not ok
    else{
        err = "Could not make flann::Index";
    }

    return ok;
}
#endif

bool FeatureDetectorFPNode::find_homography()
{
    unsigned int minInliers = 8;
    bool ok = ( mpts_1.size() >= minInliers );
    
    if (!ok){
        // frame rejected, but keep looking at other frames..
        cout << "Only " << mpts_1.size() << " keypoints found.. skiping" << endl;
        return false;
    }
    
    Mat H = findHomography( mpts_1, mpts_2, RANSAC, 1.0, outlier_mask );

    int inliers=0, outliers=0;
    
    for(unsigned int ix=0; ix<mpts_1.size(); ix++ ){
        
        if( outlier_mask.at( ix )) inliers++;
        else                        outliers++;
    }
    
    cout << "found " << inliers << " and " << outliers << " total " << mpts_1.size() << endl;
    
    //-- Get the corners from the image_1 ( the object to be "detected" )
    vector<Point2f> tgt_corners(4);
    
    tgt_corners[0] = Point2f( 0           , 0            );
    tgt_corners[1] = Point2f( tgt_mat.cols, 0            );
    tgt_corners[2] = Point2f( tgt_mat.cols, tgt_mat.rows );
    tgt_corners[3] = Point2f( 0           , tgt_mat.rows );
    
    perspectiveTransform( tgt_corners, rect, H);
    
    return ok;
}

bool FeatureDetectorFPNode::process_one_frame()
{
    bool ok = !detector.empty()  && !tgt_mat.empty() ;
    bool found = false;
    
    if ( ok ){
        cvtColor(*in, gray, COLOR_BGR2GRAY);
        extractor->compute( gray, keypoints, descriptors );
    }
    
    if ( ok ){
         found = find_homography();
    }
    
    if ( window ){
        
        base->copyTo( out );
        
        if ( found ){
            //-- Draw lines between the corners (the mapped object in the scene - image_2 )
            line( out, rect[0] + Point2f( tgt_mat.cols, 0),
                       rect[1] + Point2f( tgt_mat.cols, 0), Scalar( 0, 255, 0), 4 );
            
            line( out, rect[1] + Point2f( tgt_mat.cols, 0),
                       rect[2] + Point2f( tgt_mat.cols, 0), Scalar( 0, 255, 0), 4 );
            
            line( out, rect[2] + Point2f( tgt_mat.cols, 0),
                       rect[3] + Point2f( tgt_mat.cols, 0), Scalar( 0, 255, 0), 4 );
            
            line( out, rect[3] + Point2f( tgt_mat.cols, 0),
                       rect[0] + Point2f( tgt_mat.cols, 0), Scalar( 0, 255, 0), 4 );
        
            // cleanup found rect
            rect.clear();
        }
        
        imshow( window, out );
    }
    
    if ( !ok ){
        cout << err << endl;
    }
    
    return ok;
}