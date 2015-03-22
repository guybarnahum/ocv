// =============================================================================
//
//  FindContour.cpp
//  ocv
//
//  Created by Neta Krakover on 3/15/15.
//
// =============================================================================

// .................................................................... includes
#include "HoughCirclesFPN.hpp"

#define HOUGH_CIRCLES_NAME "HoughCircles"
#define HOUGH_CIRCLES_DESC "Identify circles with Hough algorithm without canny"

// ................................................................. constructor

HoughCirclesFPN::HoughCirclesFPN():FrameProcessNode()
{
    set_name( HOUGH_CIRCLES_NAME );
    set_desc( HOUGH_CIRCLES_DESC );
}


// ........................................................... process_one_frame

bool HoughCirclesFPN::process_one_frame()
{

    gray( *in, gray_mat );
    GaussianBlur( gray_mat, gray_mat, Size(9, 9), 2, 2 );

    HoughCircles( gray_mat, circles, CV_HOUGH_GRADIENT,
            2, gray_mat.rows/4, 200, 100 );
    if ( window ){

        base->copyTo( out );


        for( size_t ix = 0; ix < circles.size(); ix++ ){

            Point center(cvRound(circles[ix][0]), cvRound(circles[ix][1]));
            int   radius = cvRound(circles[ix][2]);

            // draw the circle outline
            circle( out, center, radius, OCV_GREEN, 3, 8, 0 );
        }

        window_show( window, out );
    }

    return true;
}
