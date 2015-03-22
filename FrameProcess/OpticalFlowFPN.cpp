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

#define OPTICALFLOW_NAME "OpticalFlowFPNode"
#define OPTICALFLOW_DESC "Identify optical flow between frames"
#define OPTICALFLOW_DETECTOR    "surf"

// =========================================================== OpticalFlowFPNode

// ................................................................. constructor

OpticalFlowFPNode::OpticalFlowFPNode()
                 : FeatureDetectorFPNode( (char *)OPTICALFLOW_DETECTOR )
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

bool OpticalFlowFPNode::detect_optical_flow()
{
    // save last frame from `dst` into `src`, making src ready..
    // unless its the first frame!
    swap_src_dst();
    
    // overwrite dst with new frame
    gray( *in, dst->mat );
    dst->ready = true;
    
    // look for keypoints from src, the last frame..
    // if first frame src is not ready!
    
    bool    ok = src->ready;
    int     max_keypoints       = 500 - (int)src->good_kpts.size();
    int     enough_keypoints    = 120;
    int     min_keypoints       = 12;
    double  quality_level       = 0.01;
    double  min_dist            = 10.;

    // do we have enough keypoints to track?
    bool need_points =  ( src->good_kpts.size() < enough_keypoints ) &&
                        ( max_keypoints > 0 );
    
    if ( ok && need_points ){
        vector<Point2f> pnts_2f;
        
        goodFeaturesToTrack( src->mat       ,
                             pnts_2f        ,
                             max_keypoints  ,
                             quality_level  ,
                             min_dist       );
        
        src->good_kpts.insert(src->good_kpts.end(), pnts_2f.begin(), pnts_2f.end());
        ok =  src->good_kpts.size() > min_keypoints;
    
        LOG( LEVEL_INFO ) << src->good_kpts.size() << " features..";
    }
    
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

void OpticalFlowFPNode::prune_keypoints()
{
    vector<size_t> keep_ix;
    
    for( size_t ix = 0 ; ix < v_status.size(); ix++ ){
        
        if ( v_status[ ix ] && ( v_error[ ix ] < 12.) ){
            keep_ix.push_back( ix );
        }
    }
    
    // prune..
    prune_vector(src->good_kpts , keep_ix );
    prune_vector(dst->good_kpts , keep_ix );
    prune_vector(v_error        , keep_ix );
    prune_vector(v_status       , keep_ix );
    
#if 0
    int rows = (int)dst->good_kpts.size();
    Mat dst_points = Mat( dst->good_kpts ).reshape( 1, rows );
    vector< vector<DMatch> > neighbors;
    
    matcher.radiusMatch( dst_points, dst->good_kpts, neighbors, 2.);
    
    for( int ix = 0 ; ix < neighbors.size() ; ix++ ){

        LOG( LEVEL_INFO ) << "Neighbors " << ix;
        
        double ratio = 0.;
        
        // any neighbors?
        if ( neighbors[ ix ].size() == 0 ) continue;
        
        if ( neighbors[ ix ].size() > 1  ){
            
            ratio = neighbors[ ix ][ 0 ].distance /
                    neighbors[ ix ][ 1 ].distance ;
            
            // too close?
            if ( ratio > 0.7 ) continue;
        }
        
        // closest
        DMatch m = neighbors[ ix ][ 0 ];

        LOG( LEVEL_INFO ) << "(" << neighbors[ ix ].size() << ")" 
                            << ratio      << ","
                            << m.trainIdx << ","
                            << m.queryIdx ;
        
    }
#endif

    LOG( LEVEL_INFO ) << src->good_kpts.size() << " good features..";
}

// ........................................................................ calc
bool OpticalFlowFPNode::calc()
{
    bool ok = true;
    
    Mat F = findFundamentalMat(src->good_kpts, dst->good_kpts, FM_RANSAC, 0.1, 0.99, v_status);
    
    // Sanity check for F
    
    Mat E = K.t() * F * K; //according to HZ (9.12)
    // LOG( LEVEL_INFO ) << F;
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



