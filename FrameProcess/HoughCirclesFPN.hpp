// =============================================================================
//
//  FindContour.hpp
//  ocv
//
//  Created by Neta Krakover on 3/15/15.
//
// =============================================================================

#ifndef ocv_HoughCirclesFPN_hpp
#define ocv_HoughCirclesFPN_hpp

#include "ocvstd.hpp"
#include "FrameProcessNode.hpp"

// ================================================== class  FindContour

class HoughCirclesFPN : public FrameProcessNode {

private:

    Mat gray_mat;

public:

    vector<Vec3f> circles;

    HoughCirclesFPN();
    ~HoughCirclesFPN(){}

    // ....................................................... overriden methods
    bool process_one_frame();

};

#endif /* defined(__ocv__HoughCirclesFPN__) */
