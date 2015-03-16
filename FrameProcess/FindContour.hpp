// =============================================================================
//
//  FindContour.hpp
//  ocv
//
//  Created by Neta Krakover on 3/15/15.
//
// =============================================================================

#ifndef ocv_FindContour_hpp
#define ocv_FindContour_hpp

#include "ocvstd.hpp"
#include "FrameProcessNode.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// ================================================== class  FindContour

class FindContour : public FrameProcessNode {

private:

    Mat gray_mat;
    Mat canny_output;

public:

    FindContour();
    ~FindContour(){}

    // ....................................................... overriden methods
    bool process_one_frame();



};

#endif /* defined(__ocv__FindContour__) */
