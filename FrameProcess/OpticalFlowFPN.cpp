// =============================================================================
//
//  OpticalFlowFPN.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/15/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#include "OpticalFlowFPN.hpp"
#include <set>

#define OPTICALFLOW_NAME "OpticalFlowFPNode"
#define OPTICALFLOW_DESC "Identify optical flow between frames"
#define OPTICALFLOW_DETECTOR    "surf"

// =========================================================== OpticalFlowFPNode

// ................................................................. constructor

OpticalFlowFPNode::OpticalFlowFPNode() : FeatureDetectorFPNode()
{
    // setup name / desc
    string str  = OPTICALFLOW_NAME;
    str += ":";
    str += FeatureDetectorFPNode::get_name();
    
    set_name( str );
    
    // desc
    str  = OPTICALFLOW_DESC;
    str += "\n";
    str += FeatureDetectorFPNode::get_desc();
    set_desc( str );

    method = METHOD_DEFAULT;
    K = Mat::eye(3, 3, CV_32F);
}

OpticalFlowFPNode::~OpticalFlowFPNode(){ }

// ................................................................. from_string

OpticalFlowFPNode::method_e OpticalFlowFPNode::from_string( const char *val )
{
    if ( val == nullptr ) return METHOD_NONE;
    string str( val );
    
    if ( str == "rich-features" ) return METHOD_RICH_FEATURES;
    if ( str == "optical-flow"  ) return METHOD_OPTICAL_FLOW;
    
    LOG( LEVEL_ERROR ) << "invalid method name (" << val << ")";
    
    return METHOD_NONE;
}

const char *OpticalFlowFPNode::to_string( OpticalFlowFPNode::method_e m )
{
    switch( m ){
        case METHOD_NONE            : return "none";
        case METHOD_OPTICAL_FLOW    : return "optical-flow";
        case METHOD_RICH_FEATURES   : return "rich-features";
    }
    
    return "unknown";
}

// ....................................................................... setup

bool OpticalFlowFPNode::setup( argv_t *argv )
{
    // call the parent setup for base class setup options
    // do this first so we have `dbg` option set, etc
    bool ok = FeatureDetectorFPNode::setup( argv );
    if (!ok) return false;
    
    // NOTICE : Optional settings ahead - reset ok state
    //          and start preserving errors with && ok
    
    // do_rich_features option
    const char *val = get_val( argv, "method" );
    method = from_string( val );
    
    LOG( LEVEL_INFO ) << "Using " << to_string( method ) ;
    
    val = get_val( argv, "camera" );
    if ( val ){
        camera_xml = val;
        ok = import_camera_mat( camera_xml, K );
    }
    
    LOG( LEVEL_INFO ) << "Camera matrix: " << K;
    
    return ok;
}

// ........................................................ detect_rich_features

bool OpticalFlowFPNode::detect_rich_features()
{
    // save last frame from dst into src, making src ready..
    swap_src_dst();
    
    // overwrite dst with new frame
    gray( *in, dst->mat );
    invalidate(dst);

    // we have both src and dst!
    // detect matches last frame in `dst` and the new frame in src..
    // NOTICE: dst should always be ready unless its the first frame..
    bool ok = detect();
    if ( ok ) ok = matched_keypoints();

    return ok;
}

// ......................................................... detect_optical_flow
bool OpticalFlowFPNode::find_features_to_track( FeatureDetector_ctx *ctx )
{
    // look for keypoints from src, the last frame..
    // if first frame src is not ready!
    
    bool    ok = ctx->ready;
    int     max_keypoints       = 500 - (int)ctx->good_kpts.size();
    int     enough_keypoints    = 300;
    int     min_keypoints       = 40;
    double  quality_level       = 0.01;
    double  min_dist            = 10.;
    
    // do we have enough keypoints to track?
    bool need_points =  ( ctx->good_kpts.size() < enough_keypoints ) &&
                        ( max_keypoints > 0 );
    
    if ( ok && need_points ){
        vector<Point2f> pnts_2f;
        
        goodFeaturesToTrack( ctx->mat, pnts_2f,
                             max_keypoints, quality_level, min_dist );
        
        ctx->good_kpts.insert(ctx->good_kpts.end(), pnts_2f.begin(), pnts_2f.end());
        ok =  ctx->good_kpts.size() > min_keypoints;
        
        LOG( LEVEL_INFO ) << ctx->good_kpts.size() << " features..";
    }
    
    return ok;
}

bool OpticalFlowFPNode::detect_optical_flow()
{
    // save last frame from `dst` into `src`, making src ready..
    // unless its the first frame!
    swap_src_dst();
    
    // overwrite dst with new frame
    gray( *in, dst->mat );
    dst->ready = true;
    
    bool ok = find_features_to_track( src );
    
    // calc flow from last frame in `src` and the new frame in `dst`..
    
    if ( ok ){
        dst->good_kpts.clear();
        v_status.clear();
        v_error.clear();
        
        switch( method ){
                
            case METHOD_OPTICAL_FLOW_PYR_LK :
                    calcOpticalFlowPyrLK( src->mat      , dst->mat      ,
                                          src->good_kpts, dst->good_kpts,
                                          v_status      , v_error       );
                    break;
                
            default: ok = false;
                LOG( LEVEL_ERROR )  << "Internal Error : method ("
                                    << to_string( method ) << ")";
                break;
        }
    }

    OCV_ASSERT( v_status.size() == v_error.size() );
    
    if ( ok ) prune_keypoints();

    return ok;
}

// ............................................................. prune_keypoints

#if 0

void OpticalFlowFPNode::prune_keypoints()
{
    vector<Point2f> to_find;
    vector<int> to_find_back_idx;
    
    for (unsigned int i=0; i<v_status.size(); i++) {
        if (v_status[i] && v_error[i] < 12.0) {
            to_find_back_idx.push_back(i);
            to_find.push_back(dst->good_kpts[i]);
//          to_find.push_back(j_pts[i]);
        } else {
            v_status[i] = 0;
        }
    }
    
    std::set<int> found_in_imgpts_j;
    Mat to_find_flat = Mat(to_find).reshape(1,(uint)to_find.size());
    
//    vector<Point2f> j_pts_to_find;
//    KeyPointsToPoints(imgpts[idx_j],j_pts_to_find);
//    Mat j_pts_flat = Mat(j_pts_to_find).reshape(1,j_pts_to_find.size());
    Mat j_pts_flat = Mat(dst->good_kpts).reshape(1,(uint)dst->good_kpts.size());
    
    vector<vector<DMatch> > knn_matches;
    //FlannBasedMatcher matcher;
    BFMatcher matcher(CV_L2);
    matcher.radiusMatch(to_find_flat,j_pts_flat,knn_matches,2.0f);
    vector<DMatch> matches;
    
    for(int i=0;i<knn_matches.size();i++) {
       DMatch _m;
       if(knn_matches[i].size()==1) {
           _m = knn_matches[i][0];
       } else if(knn_matches[i].size()>1) {
           if(knn_matches[i][0].distance / knn_matches[i][1].distance < 0.7) {
               _m = knn_matches[i][0];
           } else {
               continue; // did not pass ratio test
           }
       } else {
           continue; // no match
       }
        
       if (found_in_imgpts_j.find(_m.trainIdx) == found_in_imgpts_j.end()) { // prevent duplicates
           _m.queryIdx = to_find_back_idx[_m.queryIdx]; //back to original indexing of points for <i_idx>
           matches.push_back(_m);
           found_in_imgpts_j.insert(_m.trainIdx);
       }
    }

    cout << "pruned " << matches.size() << " / " << knn_matches.size() << " matches" << endl;
}
#else

void OpticalFlowFPNode::prune_keypoints()
{
    vector<size_t> keep_ix;
    
    for( size_t ix = 0 ; ix < v_status.size(); ix++ ){
        
        if ( v_status[ ix ] && ( v_error[ ix ] < 12.) ){
            keep_ix.push_back( ix );
        }
        else{
            // mark as invalid
            v_status[ ix ] = 0;
        }
    }
    
    // prune..
    prune_vector(src->good_kpts , keep_ix );
    prune_vector(dst->good_kpts , keep_ix );
    prune_vector(v_error        , keep_ix );
    prune_vector(v_status       , keep_ix );
    
#if 0
    vector<vector<DMatch> > knn_matches;
    Mat src_mat = Mat(src->good_kpts).reshape(1,(uint)src->good_kpts.size());
    Mat dst_mat = Mat(dst->good_kpts).reshape(1,(uint)dst->good_kpts.size());

    matcher->radiusMatch(src_mat,dst_mat,knn_matches,2.0f);

    set<int> found_in_dst;

    for(size_t ix=0; ix < knn_matches.size(); ix++ ){
        
        DMatch _m;
        
        if( knn_matches[ix].size()==1) {
            _m = knn_matches[ix][0];
        }
        else
        if( knn_matches[ix].size() >1) {
            
            if( knn_matches[ix][0].distance / knn_matches[ix][1].distance < 0.7) {
                _m = knn_matches[ix][0];
            }
            else {
                continue; // did not pass ratio test -- too close
            }
        } else {
            continue; // no match
        }
#if 0
        // prevent duplicates
        if ( found_in_dst.find( _m.trainIdx ) == found_in_dst.end() ) {
            //fix queryIdx - back to original indexing of points for <i_idx>
            _m.queryIdx = (uint)keep_ix[ _m.queryIdx ];
            matches.push_back(_m);
            found_in_dst.insert( _m.trainIdx );
        }
#endif
        
    }
    
#endif

    LOG( LEVEL_INFO ) << src->good_kpts.size() << " good features..";
}

#endif

// ........................................................................ calc
//
// Following Hartley and Zisserman show in their book: "Multiple View Geometry"
//
// @Book{Hartley2004,
// author = "Hartley, R.~I. and Zisserman, A.",
// title = "Multiple View Geometry in Computer Vision",
// edition = "Second",
// year = "2004",
// publisher = "Cambridge University Press, ISBN: 0521540518"
// }

bool OpticalFlowFPNode::calc()
{
    bool ok = true;
    
    Mat F = findFundamentalMat(src->good_kpts, dst->good_kpts, FM_RANSAC, 0.1, 0.99, v_status);
    
    // Sanity check for F
    
    Mat E = K.t() * F * K;            // HZ (9.12)
    SVD svd(E);
    
    Matx33d W   ( 0,-1, 0,            // HZ 9.13
                  1, 0, 0,
                  0, 0, 1);
    
    Matx33d Winv( 0, 1, 0,
                 -1, 0, 0,
                  0, 0, 1);
    
    Mat R = svd.u * Mat(W) * svd.vt ; // HZ 9.19
    Mat t = svd.u.col(2)            ; // u3
    
    Matx34d P1 = Matx34d( R.at<double>(0,0), R.at<double>(0,1),	R.at<double>(0,2), t.at<double>(0) ,
                          R.at<double>(1,0), R.at<double>(1,1),	R.at<double>(1,2), t.at<double>(1) ,
                          R.at<double>(2,0), R.at<double>(2,1),	R.at<double>(2,2), t.at<double>(2) );
    
    // LOG( LEVEL_INFO ) << P1;

    return ok;
}

// ........................................................... process_one_frame

bool OpticalFlowFPNode::process_one_frame()
{
    bool ok = false;
    
    switch( method ){
        case METHOD_RICH_FEATURES: ok = detect_rich_features(); break;
        case METHOD_OPTICAL_FLOW : ok = detect_optical_flow (); break;
        
        default : ok = false;
            LOG( LEVEL_ERROR ) << "Internal Error : invalid method ("
                               << method << ")";
            break;
    }
    
    if ( ok ) calc();
    
    if ( window ){
        base->copyTo( out );
        
        if ( ok ) drawArrows( out,
                              src->good_kpts,
                              dst->good_kpts,
                              v_status,
                              v_error,
                              OCV_WHITE );
        
        window_show( window, out );
    }
    
    return FeatureDetectorFPNode::process_one_frame();
}



