// =============================================================================
//
//  HoughFPNode.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes

#include "HoughFPNode.hpp"

#define HOUGH_NAME "HoughFPNode"
#define HOUGH_DESC "Identify lines and circles with Hough algorithm"

// ================================================================= HoughFPNode

// ................................................................. constructor

HoughFPNode::HoughFPNode():FrameProcessNode()
{
    set_name( HOUGH_NAME );
    set_desc( HOUGH_DESC );
    
    do_lines = true;
}

// ................................................................. process_key

bool
HoughFPNode::process_key(int key )
{
    switch( key ){
        // toggle between lines and circles
        case ' ' : do_lines = !do_lines; break;
    }
    
    return true;
}

// ........................................................... process_one_frame
bool
HoughFPNode::process_one_frame()
{
    if ( do_lines ){
        HoughLinesP( *in, lines, 1, CV_PI/180, 50, 50, 10 );
    }
    else{
        gray( *in, gray_mat );
        // smooth it, otherwise a lot of false circles may be detected
        GaussianBlur( gray_mat, gray_mat, Size(9, 9), 2, 2 );
        HoughCircles( gray_mat, circles, CV_HOUGH_GRADIENT,
                     2, gray_mat.rows/4, 200, 100 );
    }
    if ( window ){
        
        base->copyTo( out );

        if ( do_lines ){
            
            LOG( LEVEL_DEV ) << lines.size() << " lines found" ;
        
            for( size_t ix = 0; ix < lines.size(); ix++ ){
            
                Vec4i l = lines[ ix ];
            
                line( out,   Point( l[0], l[1] ), Point( l[2], l[3] ),
                                         OCV_RED, 3, LINE_AA);
            
                LOG( LEVEL_DEV ) << "("   << l[0] << "," << l[1] <<
                                    ")-(" << l[3] << "," << l[4] << ")" ;
            }
        }
        else{
            
            for( size_t ix = 0; ix < circles.size(); ix++ ){
                
                Point center(cvRound(circles[ix][0]), cvRound(circles[ix][1]));
                int   radius = cvRound(circles[ix][2]);
                
                // draw the circle outline
                circle( out, center, radius, OCV_GREEN, 3, 8, 0 );
            }
        }
        
        window_show( window, out );
    }

    return true;
}
