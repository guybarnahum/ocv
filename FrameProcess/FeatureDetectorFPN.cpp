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
#include "FeatureFactory.hpp"
#include "FeatureDetectorFPN.hpp"

// ........................................................................ init

bool FeatureDetectorFPNode::init_detector( const char *requested_name )
{
    char *name = (char *)requested_name;
    detector = FeatureFactory::makeDetector ( name );
    
    bool ok = !detector.empty();
    
    if ( ok ){
        // .............................. set name / desc
        string str;
        str  = "<FPN:FeatureDetector>";
        set_name( str.c_str() );
        
        str  = "<detector:'"; str += (name? name : ""); str += "'>\n";
        set_desc( str.c_str() );
    }
    else{
        string msg = "<invalid detector:'"; msg+=(name? name:"?"); msg += "'>";
        set_err( INVALID_ARGS, msg.c_str() );
    }
    
    return ok;
}

bool FeatureDetectorFPNode::init_extractor( const char *requested_name )
{
    char *name = (char *)requested_name;
    extractor  = FeatureFactory::makeExtractor( name );
    bool ok    = !extractor.empty();
    
    if ( ok ){
        string desc( get_desc() );
        desc += "<extractor:'"; desc += name? name : "default"; desc += "'>\n";
        set_desc( desc.c_str() );
    }
    else{
        string msg = "<invalid extractor:'"; msg += name; msg += "'>";
        set_err( INVALID_ARGS, msg.c_str() );
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
        desc += "<matcher:'"; desc += name? name : "default"; desc += "'>\n";
        set_desc( desc.c_str() );
    }
    else{
        string msg = "<invalid matcher:'"; msg += name; msg += "'>";
        set_err(INVALID_ARGS, msg.c_str());
    }
    
    return ok;
}

bool FeatureDetectorFPNode::init_tracker( const char *requested_name  )
{
    char *name = (char *)requested_name;
    tracker = FeatureFactory::makeTracker( name );
    bool ok = !tracker.empty();
    
    if ( ok ){
        string desc( get_desc() );
        desc += "<tracker:'"; desc += name? name : "default"; desc += "'>\n";
        set_desc( desc.c_str() );
    }
    else{
        string msg = "<invalid tracker:'"; msg += name; msg += "'>";
        set_err(INVALID_ARGS, msg.c_str());
    }
    
    return ok;
}


bool FeatureDetectorFPNode::init( const char *dtct_name  ,
                                  const char *xtrct_name ,
                                  const char *match_name ,
                                  const char *trckr_name )
{
    bool      ok = init_detector ( dtct_name  );
    if ( ok ) ok = init_extractor( xtrct_name );
    if ( ok ) ok = init_matcher  ( match_name );
    
    // TODO: tracking is not working yet, set to true for debugging
         enable_tracking = false;
    if ( enable_tracking && ok )
              ok = init_tracker  ( trckr_name );
    
    //  args may override these default settings
    min_inliers = FeatureDetectorFPNode::MIN_INLINERS_DEFAULT;
    do_knn_match        = false;
    do_refine_homography= false;
    
    // state of rect -- none, detect, then track, repeat as needed
    state               = NONE ;
    
    // prepare object location
    pts.push_back( Point( 0, 0 ));
    pts.push_back( Point( 0, 0 ));
    pts.push_back( Point( 0, 0 ));
    pts.push_back( Point( 0, 0 ));
    
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
    ok = ( nullptr != ( val = get_val( argv, "algo" ) ) );
    if ( ok ) ok = init_detector( val );
    
    // we better have a valid algo feature detector
    // regardless of how we got it from the option or default..
    ok = !detector.empty();
    
    if (!ok){
        string msg;
        if ( val != nullptr ){
            msg = "<invalid `algo`:'" ; msg += val; msg += "'>";
        }
        else{
            msg = "<missing required option `algo`>";
        }
        
        set_err( INVALID_ARGS, msg );
        return false;
    }
    
    // ......................................... obj_path
    
    ok = ( nullptr != ( val = get_val( argv, "obj_path" ) ) );
    obj_path = ok? val : "";
    
    if (ok) ok = file_to_path( obj_path );
    
    if (ok){
        try{
            obj_mat = imread( obj_path, IMREAD_GRAYSCALE );
            ok = !obj_mat.empty();
        }
        catch( Exception e ){
            set_err( INVALID_ARGS, e.what() );
            ok  = false;
        }
    }
    
    if (ok){

        obj_corners.push_back( Point2f( 0           , 0            ));
        obj_corners.push_back( Point2f( obj_mat.cols, 0            ));
        obj_corners.push_back( Point2f( obj_mat.cols, obj_mat.rows ));
        obj_corners.push_back( Point2f( 0           , obj_mat.rows ));
        
        obj_keypoints.clear();
        
        detector->detect  ( obj_mat, obj_keypoints );
        extractor->compute( obj_mat, obj_keypoints , obj_descriptors);
        matcher_train();
        
        LOG( LEVEL_INFO ) << "trained matcher with " <<
                                obj_keypoints.size() << " keypoints";
    }
    
    if ( ok && dbg ){
        
        drawKeypoints( obj_mat, obj_keypoints, obj_mat       ,
                       DrawMatchesFlags::DRAW_OVER_OUTIMG    |
                       DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

        window_show( obj_path.c_str(), obj_mat);
    }

    // we better have a valid tgt object
    if (!ok){
        string msg;
        if (val != nullptr){
            msg  = "<invalid `obj_path`:'" ; msg += val; msg += "'>";
        }
        else{
            msg = "<missing required option `obj_path`>";
        }
        
        set_err(INVALID_ARGS, msg );
        return false;
    }
    
    // ------------------------------ Optional arguments
    
    // .................................... match option
    
    val = get_val( argv, "matcher" );
    if (val != nullptr){
        ok = init_matcher( val );
    }
    
    if (!ok){
        string msg = "<invalid matcher option:'"; msg += val; msg += "'>";
        set_err(INVALID_ARGS, msg );
        return false;
    }
    
    // .................................. do knn matcher?
    
    if ( ok ) ok = get_val_bool( argv, "knn_match", do_knn_match );
    if (!ok){
        string msg = "<'knn_match' expects boolean value>";
        set_err(INVALID_ARGS, msg );
        return false;
    }
    else{
        LOG( LEVEL_INFO ) << "Using "
                          << ( do_knn_match? "knn_match" : "normal match");
    }
    
    // ............................... refine homography?
    
    if ( ok ) ok = get_val_bool( argv, "refine", do_refine_homography );
    if (!ok){
        string msg = "<'refine' expects boolean value>";
        set_err(INVALID_ARGS, msg );
        return false;
    }
    else{
        LOG( LEVEL_INFO ) << "Using "
        << ( do_refine_homography? "refined" : "rough")
        << " homography";
    }
    
    // ...................................... min_inliers
    
    if ( ok ) ok = get_val_int( argv, "inliers"  , min_inliers  );
    if (!ok){
        string msg = "<inliers expects int value>";
        set_err(INVALID_ARGS, msg );
        return false;
    }
    else{
        LOG( LEVEL_INFO ) << "Inliers:" << min_inliers;
    }
    
    return ok;
}

// =============================================================================
// PHILOSOPHICAL NOTE:
//
// The matcher is THE key step in object detection..
// The match process creates pairs of keypoints that then are used to create
// the homography that if found 'locates' the object in the frame
// 
// The complexity of this step is key to performance, me thinks.
//
// =============================================================================

// ............................................................... matcher_train
// pre-cache the obj_descriptors for fast compare with scenes
// notice that it is possible to train for multiple objects (unimplemented)
//

bool FeatureDetectorFPNode::matcher_train()
{
    // API of cv::DescriptorMatcher is somewhat tricky
    // First we clear old train data:
    matcher->clear();
    
    // Then we add vector of descriptors (each descriptors matrix describe
    // one object). This allows us to perform search across multiple objects:
    std::vector<cv::Mat> obj_dsc(1);
    obj_dsc[0] = obj_descriptors.clone();
    matcher->add( obj_dsc );
    
    // We have train data, now train
    matcher->train();
    
    // TODO: No way to fail?
    return true;
}

// ................................................................... knn_match
//
// knn match strategy
//
// For matching, we use the k nearest neighbour search.
// A kk search basically computes the 'distance' between a query descriptor
// and all of the training descriptors, and returns the k pairs with lowest
// distance.
//
// We use k=2, and get 2 pairs of matches for each query descriptor.
//
// What is important is how we decide which of all these matches are
// 'good matches' after all. One strategy is to trust only matches where if
// the distance(match1,query) < 0.8 * distance(match2,query), then match1 is
// a good match otherwise discard both match1 and match2 as false matches.
//
// .............................................................................

bool FeatureDetectorFPNode::knn_match()
{
    vector<vector<DMatch>> knn_matches;
    
    // To avoid NaN's when best match has 0 distance we will use inversed ratio
    // we assume that while best match can be zero, while better match is
    // allways non zero!
    const float min_ratio = 1.f / 1.5f;
        
    // KNN match will return 2 nearest matches for each query descriptor
    matcher->knnMatch( scn_descriptors, knn_matches, 2);
 
    // pick the best matches from knn_matches
    matches.clear();

    for (size_t ix=0; ix< knn_matches.size(); ix++){
        
        const cv::DMatch& best_match   = knn_matches[ix][0];
        const cv::DMatch& better_match = knn_matches[ix][1];
            
        float ratio = best_match.distance / better_match.distance;
            
        // Pass only matches where distance ratio between
        // nearest matches is greater than 1.5 (distinct criteria)
        if ( ratio < min_ratio){
            matches.push_back( best_match);
        }
    }

    // draw matches?
    if ( dbg ){
        
        Mat draw_mat;
        base->copyTo( out );
        
        try{
            drawMatches( out, scn_keypoints, obj_mat, obj_keypoints,
                        matches, draw_mat,
                        Scalar::all(-1), Scalar::all(-1),
                        vector<char>() ,
                        DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS |
                        DrawMatchesFlags::DRAW_RICH_KEYPOINTS    );
            
            window_show( "matched", draw_mat );
        }
        catch( Exception e ){
            LOG( LEVEL_WARNING ) << e.what();
        }
    }

    bool   ok = matches.size() > min_inliers;
    return ok;
}
// ....................................................................... match

bool FeatureDetectorFPNode::match()
{
    matches.clear();
    
    try{
        matcher->match( scn_descriptors, matches );
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
    
    // keep only "good" matches (i.e. whose distance is less than 3 * min_dist )
    std::vector< DMatch > good_matches;
    
    for( int ix = 0; ix < obj_descriptors.rows; ix++ ){
        if( matches[ ix ].distance < 3 * min_dist ){
            good_matches.push_back( matches[ix]);
        }
    }
    
    matches.swap(good_matches);
    
    // draw matches?
    if ( dbg ){
        
        Mat draw_mat;
        base->copyTo( out );
        try{
            
            drawMatches( out, scn_keypoints, obj_mat, obj_keypoints,
                         matches, draw_mat,
                         Scalar::all(-1), Scalar::all(-1),
                         vector<char>() ,
                         DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS |
                         DrawMatchesFlags::DRAW_RICH_KEYPOINTS    );
            
            window_show( "matched", draw_mat );
        }
        catch( Exception e ){
            LOG( LEVEL_WARNING ) << e.what();
        }
    }

    bool   ok = matches.size() > min_inliers;
    return ok;
}

// ............................................................... is_valid_rect

bool FeatureDetectorFPNode::is_valid_rect( vector<Point> &poly,
                                           double min_area    )
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

// ........................................................... matched_keypoints
//
// Translate matches into source and target point vectors, we shall attempt
// to find a homography that maps the source into the target points..
//
// .............................................................................

bool FeatureDetectorFPNode::matched_keypoints()
{
    // use matches to define homography keypoints
    
    size_t obj_kpts_num = obj_keypoints.size();
    size_t scn_kpts_num = scn_keypoints.size();

    obj_good_kpts.clear();
    scn_good_kpts.clear();

    for( int ix = 0 ; ix < matches.size(); ix++ ){
    
        // get keypoints from the good matches
    
        size_t obj_ix = matches[ ix ].trainIdx;
        size_t scn_ix = matches[ ix ].queryIdx;
    
        // be carefull with indexs
        DBG_ASSERT( scn_ix < scn_kpts_num,
                   "invalid scn_ix(" << scn_ix << ") max:" << scn_kpts_num );
        
        DBG_ASSERT( obj_ix < obj_kpts_num,
                   "invalid obj_ix(" << obj_ix << ") max:" << obj_kpts_num );
        
        bool valid_pt = (obj_ix < obj_kpts_num) && ( scn_ix < scn_kpts_num );
    
        if (valid_pt){
            // indexes are valid..
            obj_good_kpts.push_back( obj_keypoints[ obj_ix ].pt );
            scn_good_kpts.push_back( scn_keypoints[ scn_ix ].pt );
        }
    }
    
    bool ok = scn_good_kpts.size() > min_inliers;
    return ok;
}

// ............................................................. find_homography

bool FeatureDetectorFPNode::find_homography()
{
    // transcode matches into keypoints for homography
    bool ok =  matched_keypoints();
    
    if ( ok ){

        // Find homography matrix and get inliers mask
        vector<char> inliers_mask( obj_good_kpts.size() );
        H_rough = findHomography ( obj_good_kpts, scn_good_kpts,
                                   FM_RANSAC, 3.0, inliers_mask );

        ok = !H_rough.empty();
    
        if (ok){
            int inliers = 0;
            for (size_t ix=0; ix < inliers_mask.size(); ix++){
                if (inliers_mask[ix]){
                // if we found enough - we are good..
                    ok = ++inliers > min_inliers ;
                    if ( ok ) break;
                }
            }
        }
    }
    
    if ( ok ){
        vector<Point2f> pts_2f(4);
        perspectiveTransform( obj_corners, pts_2f, H_rough);
        
        pts[0] = pts_2f[0]; pts[1] = pts_2f[1];
        pts[2] = pts_2f[2]; pts[3] = pts_2f[3];
        
        double min_area = (double)(scn_mat.cols * scn_mat.rows)/ 100.0;
        ok = is_valid_rect( pts, min_area );
    }
    
    return ok;
}

// ...................................................................... detect
bool FeatureDetectorFPNode::detect()
{
    gray(*in, scn_mat);
    
    scn_rect.height = in->cols;
    scn_rect.width  = in->rows;
    scn_rect.x      = 0;
    scn_rect.y      = 0;
    
    scn_keypoints.clear();
    detector->detect  ( scn_mat, scn_keypoints );
    extractor->compute( scn_mat, scn_keypoints , scn_descriptors );
    
    bool found = ( do_knn_match? knn_match() : match() ) && find_homography();
    
    if ( found ){
         focus = boundingRect(pts);
         set_state( DETECTED );
    }
    else{
         set_state( NONE );
    }
    
    return found;
}

// ....................................................................... track

bool FeatureDetectorFPNode::track()
{
    // do we have a rect?
    bool   found  = false;
    Rect2d focus_d= focus;
    
    if ( enable_tracking ){
        try{
            switch ( state ){
                default         :
                case NONE       :
                    LOG( LEVEL_WARNING ) << "Invalid state (" << state << ")";
                    found = false; break;
                case DETECTED   : found = tracker->init  (*in, focus_d); break;
                case TRACKING   : found = tracker->update(*in, focus_d); break;
            }
        }
        catch(Exception e ){
            set_err( OCV_EXCEPTION , e.what());
            found = false;
        }
    }
    
    if ( found ){
         focus = focus_d;
         set_state( TRACKING );
    }
    else{
         set_state( NONE );
    }
    
    return  found;
}

// ........................................................... process_one_frame

bool FeatureDetectorFPNode::process_one_frame()
{
    // Sanity checks
    if ( detector.empty ()||
         extractor.empty()||
         matcher.empty  ()||
         obj_mat.empty  () ){
        
        LOG( LEVEL_ERROR ) << "internal error, setup failed!";
        // don't call again!
        return false;
    }
    
    bool found = false;
    
    switch( state ){
        case DETECTED   :
        case TRACKING   : if ( true == (found = track()) ) break;
                          // if not found try to detect..
        default         :
        case NONE       : found = detect(); break;
    }
    
    if ( window ){
        
        base->copyTo( out );
        
        if ( found ){
            
            if (pts.size())
                
                polylines( out, pts, true, Scalar(   0, 255,   0), 1 );
            rectangle( out, focus ,       Scalar( 255, 255, 255), 1 );
        }
        
        window_show( window, out );
    }
    
    return true;
}