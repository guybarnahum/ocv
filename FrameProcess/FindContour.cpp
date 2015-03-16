// =============================================================================
//
//  FindContour.cpp
//  ocv
//
//  Created by Neta Krakover on 3/15/15.
//
// =============================================================================

// .................................................................... includes
#include "FindContour.hpp"

#define FIND_CONTOUR_NAME "ContourDetector"
#define FIND_CONTOUR_DESC "Contour detector based tracker"

// ................................................................. constructor

FindContour::FindContour():FrameProcessNode()
{
    set_name( FIND_CONTOUR_NAME );
    set_desc( FIND_CONTOUR_DESC );
}


// ........................................................... process_one_frame

bool FindContour::process_one_frame()
{
//    int thresh = 100;
//    int max_thresh = 255;

    /// Convert image to gray and blur it
//    cvtColor( *in, gray_mat, CV_BGR2GRAY );
//    blur( gray_mat, gray_mat, Size(9,9) );

//    Canny( gray_mat, canny_output, thresh, thresh*2, 3 );

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours( *in, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    if ( window ) {

        base->copyTo(out);

        /// Draw contours
        for (int i = 0; i < contours.size(); i++) {
            drawContours(out, contours, i, Scalar(0,255,0), 2, 8, hierarchy, 0, Point());
        }

        /// Show in a window
        window_show(window, out);
    }
    return true;
}
