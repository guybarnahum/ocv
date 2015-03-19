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
    obj_dst.push_back  ( Point2f( 0, 0 ) );
    obj_dst.push_back  ( Point2f( 0, 0 ) );
    obj_dst.push_back  ( Point2f( 0, 0 ) );
    obj_dst.push_back  ( Point2f( 0, 0 ) );
    
    obj_dst2i.push_back( Point2i( 0, 0 ) );
    obj_dst2i.push_back( Point2i( 0, 0 ) );
    obj_dst2i.push_back( Point2i( 0, 0 ) );
    obj_dst2i.push_back( Point2i( 0, 0 ) );
    
    // state of obj_dst -- none, detect, track, repeat as needed
    state = NONE ;
}

// ....................................................................... setup

bool ObjectDetectorFPNode::setup( string path )
{
    bool ok = file_to_path( path );
    
    if (ok){
        try{
            src->mat = imread( obj_path, IMREAD_GRAYSCALE );
            ok = !src->mat.empty();
        }
        catch( Exception e ){
            set_err( INVALID_ARGS, e.what() );
            ok  = false;
        }
    }
    
    if (ok){
        
        obj_src.push_back( Point2f( 0            , 0            ));
        obj_src.push_back( Point2f( src->mat.cols, 0            ));
        obj_src.push_back( Point2f( src->mat.cols, src->mat.rows ));
        obj_src.push_back( Point2f( 0            , src->mat.rows ));
        
        invalidate( src );
        prepare( src );
        matcher_train( src->descriptors );
    }
    
    if ( ok && dbg ){
        
        drawKeypoints( src->mat, src->keypoints, src->mat      ,
                      DrawMatchesFlags::DRAW_OVER_OUTIMG    |
                      DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
        
        window_show( window, src->mat);
    }
    
    return ok;
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

// .................................................................. detect_obj
bool ObjectDetectorFPNode::detect_obj()
{
    bool ok = detect();
    if ( ok ) ok = homography();
    if ( ok ){
        try{
            perspectiveTransform( obj_src, obj_dst, H );
        }
        catch( Exception e ){
            ok = false;
        }
    }
    
    if ( ok ){
        double min_area = mat_area ( dst->mat ) / 100.0;
        double area     = poly_area( obj_dst  );
        ok = area > min_area;
    }
    
    if ( ok ){
        convert_round_points_2f( obj_dst );
        focus = boundingRect( obj_dst );
    }
    
    return ok;
}

// ................................................................... track_obj

bool ObjectDetectorFPNode::track_obj()
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
    
    // src->mat already is prepared..
    // prepare dst.mst from current frame

    gray(*in, dst->mat);
    invalidate( dst );
    
    switch( state ){
            
        case DETECTED:
        case TRACKING:  if ( (ok = track_obj()) == true ) break;
                         // else try to detect..
        default      :
        case NONE    :  if ( (ok = detect_obj()) == true ) set_state( DETECTED );
                        else                               set_state( NONE );
                        break;
    }
    
    if ( window ){
        
        base->copyTo( out );
        
        if ( ok && obj_dst.size()){
            
            obj_dst2i[ 0 ].x = obj_dst[ 0 ].x;
            obj_dst2i[ 0 ].y = obj_dst[ 0 ].y;
 
            obj_dst2i[ 1 ].x = obj_dst[ 1 ].x;
            obj_dst2i[ 1 ].y = obj_dst[ 1 ].y;

            obj_dst2i[ 2 ].x = obj_dst[ 2 ].x;
            obj_dst2i[ 2 ].y = obj_dst[ 2 ].y;

            obj_dst2i[ 3 ].x = obj_dst[ 3 ].x;
            obj_dst2i[ 3 ].y = obj_dst[ 3 ].y;

            polylines( out, obj_dst2i, true, OCV_GREEN, 1 );
            rectangle( out, focus    ,       OCV_WHITE, 1 );
        }
        
        window_show( window, out );
    }
    
    // once we do all the detection and tracking, process base class..
    // right now it sets dbg information
    return  FeatureDetectorFPNode::process_one_frame();
}
