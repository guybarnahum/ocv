// =============================================================================
//
//  ObjectDetectorFPN.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/12/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_ObjectDetectorFPN_hpp
#define ocv_ObjectDetectorFPN_hpp

#include "ocvstd.hpp"
#include "FrameProcessNode.hpp"
#include <opencv2/objdetect.hpp>
#include <opencv2/xobjdetect.hpp>

// ================================================ class CascadeDetectorAdapter

class CascadeDetectorAdapter: public DetectionBasedTracker::IDetector
{
public:
    CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector):
    IDetector(),
    Detector(detector)
    {
        CV_Assert(detector);
    }
    
    void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects)
    {
        Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize, maxObjSize);
    }
    
    virtual ~CascadeDetectorAdapter(){}
    
private:
    CascadeDetectorAdapter();
    cv::Ptr<cv::CascadeClassifier> Detector;
};


// ================================================== class ObjectDetectorFPNode

#define OBJECT_DETECTOR_NAME "ObjectDetector"
#define OBJECT_DETECTOR_DESC "Cascade classifier detector based tracker"

class ObjectDetectorFPNode : public FrameProcessNode {
 
private:
    
    // cascade format xml file
    string obj_path;
    
    // detector
    Ptr<cv::CascadeClassifier> cascade;
    Ptr<DetectionBasedTracker::IDetector> i_detector;
    Ptr<DetectionBasedTracker::IDetector> i_tracker ;
    DetectionBasedTracker::Parameters     params;
    DetectionBasedTracker                *detector;
    
    // output : location of object in scene
    // detector result
    Mat gray;
    vector<Rect> FoundObjects;
    
public:
    
    ObjectDetectorFPNode( char *name ):FrameProcessNode()
    { init(); }
    
    ObjectDetectorFPNode():FrameProcessNode()
    { init(); }
    
    bool init()
    {
        set_name( OBJECT_DETECTOR_NAME );
        set_desc( OBJECT_DETECTOR_DESC );
        err = "";
        
        return true;
    }
    
    ~ObjectDetectorFPNode(){
        delete detector;
    }
    
    // ......................................................... virtual methods
    virtual bool process_one_frame();
    virtual bool setup( argv_t *argv );

};

#endif /* defined(__ocv__ObjectDetectorFPN__) */
