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

bool FeatureDetectorFPNode::init_detector( const char *requested_name )
{
    char *name = (char *)requested_name;
    detector = FeatureFactory::makeDetector ( name );
    
    bool ok = !detector.empty();
    
    if ( ok ){
        // .............................. set name / desc
        string str;
        str  = "FeatureDetectorFPNode(";
        str += (name? name : "");
        str += ")";
        set_name( str.c_str() );
        
        str  = "`";
        str += (name? name : "");
        str += "` algorithm feature detector";
        set_desc( str.c_str() );
    }
    else{
        err  = "Invalid Feature Detector (";
        err += (name? name : "?");;
        err += ")";
    }
    
    return ok;
}

bool FeatureDetectorFPNode::init_extractor( const char *requested_name )
{
    char *name = (char *)requested_name;
    extractor = FeatureFactory::makeExtractor( name );
    bool ok   = !extractor.empty();
    
    if ( ok ){
        string desc( get_desc() );
        
        desc += " extractor `";
        desc += name? name : "default";
        desc += "`";
        
        set_desc( desc.c_str() );
    }
    else{
        err  = "Invalid Feature Extractor (";
        err += name;
        err += ")";
    }

    return ok;
}

bool FeatureDetectorFPNode::init_matcher  ( const char *requested_name  )
{
    char *name = (char *)requested_name;
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
    
    //  args may override these default settings
    draw_features       = false;
    min_inliers         = 32;
    
    return ok;
}

// ....................................................................... setup
bool FeatureDetectorFPNode::setup( argv_t *argv )
{
    bool ok = (argv != nullptr );
    if (!ok){
        DBG_ASSERT( false, "no setup argv provided for " << get_name() );
        return false;
    }
    
    // call the parent setup for base class setup options
    // do this first so we have `dbg` option set, etc
    ok = FrameProcessNode::setup( argv );
    DBG_ASSERT( ok, "invalid argv provided for " << get_name() );
    if (!ok) return false;
    
    // ------------------------------- Required arguments
    const char *val;
    
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
    val = get_val( argv, "obj_path" );
    
    ok = ( val != nullptr );
    obj_path = ok? val : "";
    
    if (ok){
        ok = file_to_path( obj_path );
    }
    
    if (ok){
        obj_mat = imread( obj_path, IMREAD_GRAYSCALE );
        ok = !obj_mat.empty();
    }
    
    if (ok){
        detector->detect  ( obj_mat, obj_keypoints );
        extractor->compute( obj_mat, obj_keypoints , obj_descriptors);
    }
    
    if ( ok && ENABLE_DBG_CODE && dbg ){
        
        drawKeypoints( obj_mat, obj_keypoints, obj_mat      ,
                       DrawMatchesFlags::DRAW_OVER_OUTIMG    |
                       DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

        imshow( "object", obj_mat);
    }

    // we better have a valid tgt object
    if (!ok){
        
        if (val != nullptr){
            err = "Invalid `obj_path` option (" ;
            err += val;
            err += ")";
        }
        else{
            err = "Missing required option `obj_path`";
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

    return ok;
}

// =============================================================================
// PHILOSOPHICAL NOTE:
// The matcher is THE key step in object detection..
// The match process creates pairs of keypoints that then are used to create
// the homography that if found 'locates' the object in the frame
// 
// The complexity of this step is key to performance, me thinks.
//
// =============================================================================
/* START DISABLE_CODE */
#if 0

// ....................................................................... match

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

// FLANN - Nearest neighbor matching using FLANN library (included in OpenCV)
bool FeatureDetectorFPNode::match()
{
    bool ok = !descriptors.empty();
    if (!ok){
        return false;
    }
    
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
    
    ok = ( fix != nullptr );
    
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
            
                pt_ix = results.at<int>(ix,0);
                
                if ( pt_ix < keypoints.size() ){
                    pt    = keypoints.at( pt_ix ).pt;
                }
                else{
                    DBG_ASSERT( !dbg, "invalid pt_ix "
                                    << pt_ix
                                    << " (max:"
                                    << keypoints.size() << ")" );
                    continue;
                }
                    
                mpts_2.push_back   ( pt    );
                indexes_2.push_back( pt_ix );
                    
                pt = tgt_keypoints.at(ix).pt;
                    
                mpts_1.push_back   ( pt );
                indexes_1.push_back( ix );
            }
        }
    }
    // not ok
    else{
        err = "Could not make flann::Index";
    }

    return ok;
}

bool FeatureDetectorFPNode::match()
{
    bool ok = true;
    
    vector<DMatch> matches;
    
    matcher->match( tgt_descriptors, descriptors, matches );
    
    vector<int> train_ix( matches.size() );
    
    for( int ix = 0; ix < matches.size(); ix++ ){
        train_ix[ix] = matches[ix].trainIdx;
    }
    
    vector<Point2f> points1; KeyPoint::convert(tgt_keypoints, points1);
    vector<Point2f> points2; KeyPoint::convert(    keypoints, points2, train_ix);
    
    Mat H = findHomography( points1, points2, RANSAC, 1.0 );
    Mat drawImg;

    if( !H.empty() ) // filter outliers
    {
        vector<char> matches_mask( matches.size(), 0 );
        vector<Point2f> points1; KeyPoint::convert( tgt_keypoints, points1);
        vector<Point2f> points2; KeyPoint::convert(     keypoints, points2, train_ix);
        
        Mat points1t; perspectiveTransform( Mat(points1), points1t, H );
        
        for( int ix = 0; ix < points1.size(); ix++ ){
            if( norm(points2[ix] - points1t.at<Point2f>(ix,0)) < 4 ) // inlier
                matches_mask[ix] = 1;
        }
        
        base->copyTo( out );
        // draw inliers
        drawMatches( tgt_mat, tgt_keypoints, out, keypoints, matches, drawImg,
                    CV_RGB(0, 255, 0),
                    CV_RGB(0, 0, 255),
                    matches_mask     , DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
#if 0
        // draw outliers
        for( size_t ix = 0; ix < matches_mask.size(); ix++ ){
            matches_mask[ ix ] = !matches_mask[ ix ];
        }
        
        drawMatches( tgt_mat, tgt_keypoints, out, keypoints, matches, drawImg,
                    CV_RGB(0, 0, 255),
                    CV_RGB(255, 0, 0),
                    matches_mask     , DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
#endif
        imshow( "matched", drawImg );
    }
    // else
    //    drawMatches( tgt_mat, tgt_keypoints, out, keypoints, matches, drawImg );

    return ok;
}

#endif /* DISABLE_CODE */

// ....................................................................... match

bool FeatureDetectorFPNode::match()
{
    FlannBasedMatcher     matcher;
    std::vector< DMatch > matches;
    try{
        matcher.match( obj_descriptors, scn_descriptors, matches );
    }
    catch(cv::Exception e){
        DBG_ASSERT(false, e.what() );
        return false;
    }
    
    double max_dist = 0; double min_dist = 100;
    
    //-- Quick calculation of max and min distances between keypoints
    for( int ix = 0; ix < obj_descriptors.rows; ix++ ){
        
        double dist = matches[ix].distance;
        
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }
        
    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
    std::vector< DMatch > good_matches;
    
    for( int ix = 0; ix < obj_descriptors.rows; ix++ )
        if( matches[ix].distance < 3 * min_dist )
            good_matches.push_back( matches[ix]);

    if ( ENABLE_DBG_CODE && dbg ){
        
        // draw good matches
        Mat draw_mat;
        base->copyTo( out );
        
        drawMatches( obj_mat, obj_keypoints, out, scn_keypoints, good_matches,
                     draw_mat,
                     Scalar::all(-1), Scalar::all(-1),
                     vector<char>() ,
                     DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS |
                     DrawMatchesFlags::DRAW_RICH_KEYPOINTS    );

        imshow( "matched", draw_mat );
    }
    
    // filter "good" matches back to keypoints
    size_t obj_kpts_num = obj_keypoints.size();
    size_t scn_kpts_num = scn_keypoints.size();
    
    obj_good_kpts.clear();
    scn_good_kpts.clear();
    
    for( int ix = 0 ; ix < good_matches.size(); ix++ ){
        // get keypoints from the good matches
        size_t qix = good_matches[ ix ].queryIdx;
        size_t tix = good_matches[ ix ].trainIdx;
        
        // be carefull with indexs
        DBG_ASSERT( qix < obj_kpts_num,
                   "invalid qix(" << qix << ") max:" << obj_kpts_num );
        
        DBG_ASSERT( tix < scn_kpts_num,
                   "invalid qix(" << tix << ") max:" << scn_kpts_num );
 
        bool valid_pt = (qix < obj_kpts_num) && ( tix < scn_kpts_num );

        if (valid_pt){
            // indexes are valid..
            obj_good_kpts.push_back( obj_keypoints[ qix ].pt );
            scn_good_kpts.push_back( scn_keypoints[ tix ].pt );
        }
    }
    
    // do we have enough good matches?
    bool   ok = ( scn_good_kpts.size() > min_inliers );
    return ok;
}


bool FeatureDetectorFPNode::is_valid_rect( vector<Point2f> &poly, double min_area  )
{
    // should be a rectangle
    bool ok = ( poly.size() == 4 );
    
    // huristic 1: area should be at least 1% of the scene
    if ( ok && (min_area != 0) ){
        
        double area = 0;
        for (size_t ix = 0; ix < poly.size(); ix++){
        
            size_t next_ix =  (ix+1)%poly.size();
                              
            double dX = poly[ next_ix ].x - poly[ ix ].x;
            double dY = poly[ next_ix ].y + poly[ ix ].y;
        
            area += dX * dY;  // This is the integration step.
        }
    
        area = abs( area / 2 );
        ok   = area > min_area;
    }
    
    // huristic 2: angles should be not too small or too large
    if ( ok ){
        // test angle 0-1-2
        // test angle 1-2-3
        // test angle 2-3-0
        // test angle 3-0-1
    }
    
    return ok;
}

bool FeatureDetectorFPNode::find_homography()
{
    bool ok = true;
    
    vector<char> mask(0);
    Mat H = findHomography( obj_good_kpts, scn_good_kpts, FM_RANSAC, 3.0, mask );

    //  evaluate homography
    
    ok = !H.empty();
    
    if ( ok ){
        size_t inliers  = 0;
        size_t outliers = 0;
        
        for( size_t ix = 0; ix < mask.size(); ix++ ){
            if ( mask[ ix ] == 0 ) outliers++;
            else                   inliers++;
        }
        
        ok = inliers > min_inliers ;
    }
    
    //  evaluate output rect
    vector<Point2f> scn_corners(4);
    
    if ( ok ){
        
        // map object into scene with homography
        vector<Point2f> obj_corners(4);
    
        obj_corners[0] = Point2f( 0           , 0            );
        obj_corners[1] = Point2f( obj_mat.cols, 0            );
        obj_corners[2] = Point2f( obj_mat.cols, obj_mat.rows );
        obj_corners[3] = Point2f( 0           , obj_mat.rows );
        
        perspectiveTransform( obj_corners, scn_corners, H);

        double min_area = (double)(scn_mat.cols * scn_mat.rows)/ 100.0;
        ok = is_valid_rect( scn_corners, min_area );
    }
    
    // do we have a new valid found rect?
    if ( ok ){
        scn_rect = scn_corners;
    }
    
    return ok;
}

bool FeatureDetectorFPNode::process_one_frame()
{
    bool ok = !detector.empty() && !obj_mat.empty() ;
    bool found = false;
    
    // new scene - reset old scn context
    scn_keypoints.clear();
    scn_rect.clear();

    if ( ok ){
        cvtColor(*in, scn_mat, COLOR_BGR2GRAY);
        detector->detect  ( scn_mat, scn_keypoints );
        extractor->compute( scn_mat, scn_keypoints , scn_descriptors );
        found = match();
    }
    
    if ( found ){
         found = find_homography();
    }
    
    if ( window ){
        
        base->copyTo( out );
        
        if ( found ){
            
            DBG_ASSERT( scn_rect.size() == 4, "found but rect is invalid!" );
            
            //-- Draw lines between the corners (the mapped object in the scene )
            line( out, scn_rect[0] , scn_rect[1] , Scalar( 0, 255, 0), 4 );
            line( out, scn_rect[1] , scn_rect[2] , Scalar( 0, 255, 0), 4 );
            line( out, scn_rect[2] , scn_rect[3] , Scalar( 0, 255, 0), 4 );
            line( out, scn_rect[3] , scn_rect[0] , Scalar( 0, 255, 0), 4 );
        }
        
        imshow( window, out );
    }
    
    DBG_ASSERT( ok, err );
    
    return ok;
}