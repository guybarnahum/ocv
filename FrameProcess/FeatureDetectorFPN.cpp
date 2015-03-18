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
        string str = "FeatureDetector:"; str += name;
        set_name( str );

        str  = "<detector:'"; str += (name? name : ""); str += "'>\n";
        set_desc( str );
    }
    else{
        string msg = "<invalid detector:'"; msg+=(name? name:"?"); msg += "'>";
        set_err( INVALID_ARGS, msg );
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
        desc += "<matcher:'"; desc += name? name : "default"; desc += "'>";
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
    // init context structs
    src = &ctx1;
    dst = &ctx2;
    
    invalidate( src );
    invalidate( dst );

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
    is_trained          = false;
    dbg_window          = nullptr;
    
    return ok;
}

// ....................................................................... setup

bool FeatureDetectorFPNode::setup( argv_t *argv )
{
    // call the parent setup for base class setup options
    // do this first so we have `dbg` option set, etc
    bool ok = FrameProcessNode::setup( argv );
    if (!ok) return false;
    
    // Since FeatureDetectorFPNode is not created explicitly, show its matches
    // if dbg is set on the child class
    
    if ( dbg ) dbg_window = "FeatureDetectorFPNode";
    
    // ...................................... algo option
    // if supplied it better be valid! Otherwise we are good with default
    const char *val;
    ok = ( nullptr != ( val = get_val( argv, "algo" ) ) );
    if ( ok ){
         ok = init_detector( val );
         if (!ok){
            string msg = "<invalid `algo`:'" ; msg += val; msg += "'>";
            set_err( INVALID_ARGS, msg );
            return false;
         }
    }
    
    // .................................... match option
    // if supplied it better be valid! Otherwise we are good with default
    ok = ( nullptr != (val = get_val( argv, "matcher" ) ) );
    if ( ok ){
         ok = init_matcher( val );
        if (!ok){
            string msg = "<invalid matcher option:'"; msg += val; msg += "'>";
            set_err(INVALID_ARGS, msg );
            return false;
        }
    }
    
    // NOTICE : Optional settings ahead - reset ok state
    //          and start preserving errors with && ok
    ok = true;
    
    // .................................. do knn matcher?

    ok = get_val_bool( argv, "knn_match", do_knn_match ) && ok;
    LOG( LEVEL_INFO ) << "Using "
                      << ( do_knn_match? "knn_match" : "normal match");
    
    // ............................... refine homography?
    
    ok = get_val_bool( argv, "refine", do_refine_homography ) && ok;
    LOG( LEVEL_INFO ) << "Using " << ( do_refine_homography? "refined":"rough")
                                  << " homography";
    
    // ...................................... min_inliers
    
    ok = get_val_int( argv, "inliers"   , min_inliers ) && ok;
    LOG( LEVEL_INFO ) <<    "Inliers:" << min_inliers ;
    
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

bool FeatureDetectorFPNode::matcher_train( Mat desc, bool clear_old )
{
    // API of cv::DescriptorMatcher is somewhat tricky
    //  clear old train data or retain?
    if ( clear_old ) matcher->clear();
    
    // Then we add vector of descriptors (each descriptors matrix describe
    // one object). This allows us to perform search across multiple objects:
    vector<Mat> obj_dsc(1);
    obj_dsc[0] = src->descriptors.clone();
    matcher->add( obj_dsc );
    
    // We have train data, now train
    matcher->train();
    is_trained = true;
    
    LOG( LEVEL_INFO ) << "trained matcher with " << src->keypoints.size()
                                                 << " keypoints";
    
    // TODO: No way to fail? This is crazy!
    return is_trained;
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
    
    // mark src and dst order in the match, this is needed for operations
    // that preserve the match order when using matches
    
    // KNN match will return 2 nearest matches for each query descriptor
    try{
        if ( is_trained ){ // matcher is already primed with src_descriptors
            src->query = true;
            dst->query = false;
            
            matcher->knnMatch( dst->descriptors, knn_matches, 2);
        }
        else{
            src->query = false;
            dst->query = true;
            
            matcher->knnMatch( src->descriptors,
                               dst->descriptors, knn_matches,2);
        }
    }
    catch( Exception e ){
        LOG( LEVEL_ERROR ) << e.what();
        return false;
    }
    
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

    bool   ok = matches.size() > min_inliers;
    return ok;
}
// ....................................................................... match

bool FeatureDetectorFPNode::match()
{
    matches.clear();
    
    // mark src and dst order in the match, this is needed for operations
    // that preserve the match order when using matches
    src->query = false;
    dst->query = true;

    try{
        if ( is_trained ){ // matcher is already primed with src_descriptors
            src->query = true;
            dst->query = false;

            matcher->match( dst->descriptors, matches );
        }
        else{
            src->query = false;
            dst->query = true;
            
            matcher->match( src->descriptors, dst->descriptors, matches );
        }
    }
    catch( Exception e ){
        LOG( LEVEL_ERROR ) << e.what();
        return false;
    }
    
    double max_dist = 0; double min_dist = 100;
    
    //-- Quick calculation of max and min distances between keypoints
    for( int ix = 0; ix < src->descriptors.rows; ix++ ){
        
        double dist = matches[ix].distance;
        
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }
    
    // keep only "good" matches (i.e. whose distance is less than 3 * min_dist )
    std::vector< DMatch > good_matches;
    
    for( int ix = 0; ix < src->descriptors.rows; ix++ ){
        if( matches[ ix ].distance < 3 * min_dist ){
            good_matches.push_back( matches[ix]);
        }
    }
    
    matches.swap(good_matches);
    
    bool   ok = matches.size() > min_inliers;
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
    
    size_t src_kpts_num = src->keypoints.size();
    size_t dst_kpts_num = dst->keypoints.size();

    src->good_kpts.clear();
    dst->good_kpts.clear();

    size_t src_ix = 0;
    size_t dst_ix = 0;

    // good_keypoints from the matches
    
    for( int ix = 0 ; ix < matches.size(); ix++ ){
    
        // respect the query flag that was set during match - not the names..
        
        // NOTICE: due to OCV bug, the order of the Idx swapped!
        // It works this way, but not the right way!
        
        if ( src->query ){
            src_ix = matches[ ix ].trainIdx;
            dst_ix = matches[ ix ].queryIdx;
        }
        else
        if ( dst->query ){
            src_ix = matches[ ix ].queryIdx;
            dst_ix = matches[ ix ].trainIdx;
        }
        
        OCV_ASSERT( (dst->query && !src->query)||(!dst->query && src->query) );
        
        // be carefull with indexs
        OCV_ASSERT( src_ix < src_kpts_num );
        OCV_ASSERT( dst_ix < dst_kpts_num );
        
        bool valid_pt = (src_ix < src_kpts_num) && ( dst_ix < dst_kpts_num );
    
        if (valid_pt){
            // indexes are valid..
            src->good_kpts.push_back( src->keypoints[ src_ix ].pt );
            dst->good_kpts.push_back( dst->keypoints[ dst_ix ].pt );
        }
    }
    
    bool ok = dst->good_kpts.size() > min_inliers;
    return ok;
}

// ............................................................. find_homography

bool FeatureDetectorFPNode::find_homography()
{
    // transcode matches into keypoints for homography
    bool ok =  matched_keypoints();
    
    if ( ok ){

        // Find homography matrix and get inliers mask
        vector<char> inliers_mask( src->good_kpts.size() );
        H = H_rough = findHomography ( src->good_kpts, dst->good_kpts,
                                       FM_RANSAC, 3.0, inliers_mask );

        ok = !H.empty();
    
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
    
    return ok;
}

// .................................................................. invalidate

bool FeatureDetectorFPNode::invalidate( FeatureDetector_ctx *ctx )
{
    bool ok = ( ctx != nullptr );
    if ( ok )   ctx->ready = false;
    return ok;
}

// ..................................................................... prepare

bool FeatureDetectorFPNode::prepare( FeatureDetector_ctx *ctx, bool force )
{
    bool ok = ( ctx != nullptr );
    
    if ( ok && ( force || (!ctx->ready) ) ){
        ctx->keypoints.clear();
        detector->detect( ctx->mat, ctx->keypoints );
        extractor->compute( ctx->mat, ctx->keypoints ,ctx->descriptors );
        ctx->ready = true;
    }
    
    return ok;
}

// ...................................................................... detect

bool FeatureDetectorFPNode::detect()
{    
    prepare( src );
    prepare( dst );
    
    bool   found = do_knn_match? knn_match() : match();
    return found;
}

// ........................................................... process_one_frame

bool FeatureDetectorFPNode::process_one_frame()
{
    // Sanity checks
    bool ok = !( detector.empty()|| extractor.empty()|| matcher.empty() );
 
    if (!ok){
        LOG( LEVEL_ERROR ) << "FeatureDetectorFPNode::internal error";
        // don't call again!
        return false;
    }
    
    if ( dbg_window ){
        
        base->copyTo( out );
        
        try{
            drawMatches( out, dst->keypoints, src->mat, src->keypoints,
                        matches, matches_mat,
                        Scalar::all(-1), Scalar::all(-1),
                        vector<char>() ,
                        DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS |
                        DrawMatchesFlags::DRAW_RICH_KEYPOINTS    );
        }
        catch( Exception e ){
            LOG( LEVEL_WARNING ) << e.what();
            ok = false;
        }
        
        if ( ok ) window_show( dbg_window, matches_mat );
    }
    
    return ok;
}
