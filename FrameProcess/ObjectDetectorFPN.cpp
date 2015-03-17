// =============================================================================
//
//  ObjectDetectorFPN.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/15/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#include "ObjectDetectorFPN.hpp"

#define OBJECT_DETECTOR_NAME "ObjectDetectorFPNode"
#define OBJECT_DETECTOR_DESC "Detect a given object in a frame"

// ======================================================== ObjectDetectorFPNode

// ................................................................. constructor

ObjectDetectorFPNode::ObjectDetectorFPNode():FeatureDetectorFPNode()
{
    // prepare object location
    obj_dst.push_back( Point( 0, 0 ));
    obj_dst.push_back( Point( 0, 0 ));
    obj_dst.push_back( Point( 0, 0 ));
    obj_dst.push_back( Point( 0, 0 ));
    
    // state of obj_dst -- none, detect, track, repeat as needed
    state = NONE ;
}

// ....................................................................... setup

// .............................................................. setup obj_path

bool ObjectDetectorFPNode::setup( string path )
{
    bool ok = file_to_path( path );
    
    if (ok){
        try{
            src_mat = imread( obj_path, IMREAD_GRAYSCALE );
            ok = !src_mat.empty();
        }
        catch( Exception e ){
            set_err( INVALID_ARGS, e.what() );
            ok  = false;
        }
    }
    
    if (ok){
        
        obj_src.push_back( Point2f( 0           , 0            ));
        obj_src.push_back( Point2f( src_mat.cols, 0            ));
        obj_src.push_back( Point2f( src_mat.cols, src_mat.rows ));
        obj_src.push_back( Point2f( 0           , src_mat.rows ));
        
        src_keypoints.clear();
        
        detector->detect  ( src_mat, src_keypoints );
        extractor->compute( src_mat, src_keypoints , src_descriptors);
        matcher_train();
        is_trained = true;
        
        LOG( LEVEL_INFO ) << "trained matcher with " <<
                            src_keypoints.size() << " keypoints";
    }
    
    if ( ok && dbg ){
        
        drawKeypoints( src_mat, src_keypoints, src_mat       ,
                      DrawMatchesFlags::DRAW_OVER_OUTIMG    |
                      DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
        
        window_show( obj_path.c_str(), src_mat);
    }
    
    return ok;
}

// ............................................................... matcher_train
// pre-cache the obj_descriptors for fast compare with scenes
// notice that it is possible to train for multiple objects (unimplemented)
//

bool ObjectDetectorFPNode::matcher_train()
{
    // API of cv::DescriptorMatcher is somewhat tricky
    // First we clear old train data:
    matcher->clear();
    
    // Then we add vector of descriptors (each descriptors matrix describe
    // one object). This allows us to perform search across multiple objects:
    std::vector<cv::Mat> obj_dsc(1);
    obj_dsc[0] = src_descriptors.clone();
    matcher->add( obj_dsc );
    
    // We have train data, now train
    matcher->train();
    
    // TODO: No way to fail? This is crazy!
    return true;
}

// ....................................................................... setup

bool ObjectDetectorFPNode::setup( argv_t *argv )
{
    // call the parent setup for base class setup options
    bool ok = FeatureDetectorFPNode::setup( argv );
    if (!ok) return false;
    
    // obj_path (Required)
    
    obj_path = get_val( argv, "obj_path" );
    ok = setup( obj_path );
    
    // we better have a valid tgt object
    if (!ok){
        string msg;
        if ( obj_path != nullptr){
            msg  = "<invalid `obj_path`:'" ; msg += obj_path; msg += "'>";
        }
        else
            msg = "<missing required option `obj_path`>";
        
        set_err(INVALID_ARGS, msg );
        return false;
    }

    // setup name / desc
    string str  = OBJECT_DETECTOR_NAME;
    str += ":";
    str += FeatureDetectorFPNode::get_name();
    
    set_name( str );
    
    // desc
    str  = OBJECT_DETECTOR_DESC;
    str += "\n";
    str += FeatureDetectorFPNode::get_desc();
    set_desc( str );

    return ok;
}

// ............................................................... obj_transform
// With homography calculate object location in scene

bool ObjectDetectorFPNode::obj_transform()
{
    bool ok = !H.empty();
    
    vector<Point2f> pts_2f( 4 );
    try{
        perspectiveTransform( obj_src, pts_2f, H );
    }
    catch( Exception e ){
        ok = false;
    }
    
    if ( ok ){
        obj_dst[0] = pts_2f[0];
        obj_dst[1] = pts_2f[1];
        obj_dst[2] = pts_2f[2];
        obj_dst[3] = pts_2f[3];
    
        double min_area = (double)( dst_mat.cols * dst_mat.rows)/ 100.0;
        ok = is_valid_rect ( obj_dst, min_area );
    }
    
    return ok;
}

// ............................................................... is_valid_rect

bool ObjectDetectorFPNode::is_valid_rect( vector<Point> &poly,
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

// ....................................................................... track

bool ObjectDetectorFPNode::track()
{
    // do we have a rect?
    bool   ok  = false;
    Rect2d focus_d= focus;
    
    if ( enable_tracking ){
        try{
            switch ( state ){
                case DETECTED   : ok = tracker->init  (*in, focus_d); break;
                case TRACKING   : ok = tracker->update(*in, focus_d); break;

                default   :
                case NONE :
                    LOG( LEVEL_WARNING ) << "Invalid state (" << state << ")";
                    ok = false;
                    break;

            }
        }
        catch(Exception e ){
            set_err( OCV_EXCEPTION , e.what());
            ok = false;
        }
    }
    
    if ( ok ) focus = focus_d;
    return ok;
}

// ........................................................... process_one_frame

bool ObjectDetectorFPNode::process_one_frame()
{
    bool ok = false;
    
    // src_mat already is prepared..
    // prepare dst_mst from current frame
    gray(*in, dst_mat);
    
    switch( state ){
            
        case DETECTED:
        case TRACKING:  ok = track();
                        if ( ok ) break;
                         // else try to detect..
        default      :
        case NONE    :  set_state( NONE );
                        ok = detect();
                        if ( ok )
                             ok = obj_transform();
            
                        if ( ok ){
                            focus = boundingRect( obj_dst );
                            set_state( DETECTED );
                        }
                        break;
    }
    
    if ( window ){
        
        base->copyTo( out );
        
        if ( ok && obj_dst.size()){
            polylines( out, obj_dst, true, OCV_GREEN, 1 );
            rectangle( out, focus  ,       OCV_WHITE, 1 );
        }
        
        window_show( window, out );
    }
    
    // once we do all the detection and tracking, process base class..
    return  FeatureDetectorFPNode::process_one_frame();
}
