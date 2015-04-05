cmake_minimum_required(VERSION 2.8.4)
project(ocv)

link_directories( /usr/local/lib)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++11 -I${CMAKE_SOURCE_DIR} \
    -I${CMAKE_SOURCE_DIR}/FrameProcess \
    -I${CMAKE_SOURCE_DIR}/slam/lsdslam \
    -I${CMAKE_SOURCE_DIR}/slam/slamptam/ptam/include \
    -I${CMAKE_SOURCE_DIR}/slam/slamptam/ptam/thirdparty/libcvd/installfiles \
    -I/usr/local/include/ \
    -I/usr/local/Cellar/eigen/3.2.2/include/eigen3 \
    -L/usr/local/lib/ \
    -L/usr/local/Cellar/boost/1.57.0/lib \
    -lopencv_core.3.0.0 -lopencv_highgui.3.0.0 \
    -lopencv_video.3.0.0 -lopencv_videoio.3.0.0 \
    -lopencv_videostab.3.0.0 -lopencv_imgcodecs.3.0.0 \
    -lopencv_imgproc.3.0.0 -lopencv_features2d.3.0.0 \
    -lopencv_xfeatures2d.3.0.0 -lopencv_flann.3.0.0 \
    -lopencv_calib3d.3.0.0 -lopencv_xfeatures2d.3.0.0 \
    -lopencv_ccalib.3.0.0 -lopencv_objdetect.3.0.0 \
    -lopencv_optflow.3.0.0 -lopencv_tracking.3.0.0 \
    -lopencv_ml.3.0.0 -lopencv_xobjdetect.3.0.0 \
    -lg2o_core -lg2o_csparse_extension -lg2o_solver_csparse -lg2o_stuff \
    -lg2o_types_sba \
    -lboost_thread-mt -lboost_system-mt \
    ")

set(SOURCE_FILES
    cli.cpp
    cli.hpp
    slam/lsdslam/DataStructures/Frame.h
    slam/lsdslam/DataStructures/Frame.cpp
    slam/lsdslam/DataStructures/FrameMemory.h
    slam/lsdslam/DataStructures/FrameMemory.cpp
    slam/lsdslam/DataStructures/FramePoseStruct.h
    slam/lsdslam/DataStructures/FramePoseStruct.cpp
    slam/lsdslam/DepthEstimation/DepthMap.h
    slam/lsdslam/DepthEstimation/DepthMap.cpp
    slam/lsdslam/DepthEstimation/DepthMapPixelHypothesis.h
    slam/lsdslam/DepthEstimation/DepthMapPixelHypothesis.cpp
    slam/lsdslam/GlobalMapping/FabMap.h
    slam/lsdslam/GlobalMapping/FabMap.cpp
    slam/lsdslam/GlobalMapping/g2oTypeSim3Sophus.h
    slam/lsdslam/GlobalMapping/g2oTypeSim3Sophus.cpp
    slam/lsdslam/GlobalMapping/KeyFrameGraph.h
    slam/lsdslam/GlobalMapping/KeyFrameGraph.cpp
    slam/lsdslam/GlobalMapping/TrackableKeyFrameSearch.h
    slam/lsdslam/GlobalMapping/TrackableKeyFrameSearch.cpp
    slam/lsdslam/IOWrapper/OpenCV/ImageDisplay_OpenCV.cpp
    slam/lsdslam/IOWrapper/ImageDisplay.h
    slam/lsdslam/IOWrapper/InputImageStream.h
    slam/lsdslam/IOWrapper/NotifyBuffer.h
    slam/lsdslam/IOWrapper/Output3DWrapper.h
    slam/lsdslam/IOWrapper/Timestamp.h
    slam/lsdslam/IOWrapper/Timestamp.cpp
    slam/lsdslam/IOWrapper/TimestampedObject.h
    slam/lsdslam/sophus/rxso3.hpp
    slam/lsdslam/sophus/se2.hpp
    slam/lsdslam/sophus/se3.hpp
    slam/lsdslam/sophus/sim3.hpp
    slam/lsdslam/sophus/so2.hpp
    slam/lsdslam/sophus/so3.hpp
    slam/lsdslam/sophus/sophus.hpp
    slam/lsdslam/sophus/tests.hpp
    slam/lsdslam/Tracking/LGSX.h
    slam/lsdslam/Tracking/Relocalizer.h
    slam/lsdslam/Tracking/Relocalizer.cpp
    slam/lsdslam/Tracking/SE3Tracker.h
    slam/lsdslam/Tracking/SE3Tracker.cpp
    slam/lsdslam/Tracking/Sim3Tracker.h
    slam/lsdslam/Tracking/Sim3Tracker.cpp
    slam/lsdslam/Tracking/TrackingReference.cpp
    slam/lsdslam/Tracking/TrackingReference.h
    slam/lsdslam/util/EigenCoreInclude.h
    slam/lsdslam/util/globalFuncs.h
    slam/lsdslam/util/globalFuncs.cpp
    slam/lsdslam/util/IndexThreadReduce.h
    slam/lsdslam/util/settings.h
    slam/lsdslam/util/settings.cpp
    slam/lsdslam/util/SophusUtil.h
    slam/lsdslam/util/SophusUtil.cpp
    slam/lsdslam/util/Undistorter.h
    slam/lsdslam/util/Undistorter.cpp
    slam/lsdslam/SlamSystem.h
    slam/lsdslam/SlamSystem.cpp

#    slam/slamptam/ptam/src/GLWindow2.cc
#    slam/slamptam/ptam/src/main.cc
#    slam/slamptam/ptam/src/MapViewer.cc
#    slam/slamptam/ptam/src/ATANCamera.cc
#    slam/slamptam/ptam/src/GLWindowMenu.cc
#    slam/slamptam/ptam/src/Map.cc
#    slam/slamptam/ptam/src/MiniPatch.cc
#    slam/slamptam/ptam/src/ShiTomasi.cc
#    slam/slamptam/ptam/src/Tracker.cc
#    slam/slamptam/ptam/src/Bundle.cc
#    slam/slamptam/ptam/src/HomographyInit.cc
#    slam/slamptam/ptam/src/MapMaker.cc
#    slam/slamptam/ptam/src/PatchFinder.cc
#    slam/slamptam/ptam/src/SmallBlurryImage.cc
#    slam/slamptam/ptam/src/KeyFrame.cc
#    slam/slamptam/ptam/src/MapPoint.cc
#    slam/slamptam/ptam/src/Relocaliser.cc
#    slam/slamptam/ptam/src/System.cc
#    slam/slamptam/ptam/src/Params.cpp
    FrameProcess/CannyFPNode.cpp
    FrameProcess/CannyFPNode.hpp
    FrameProcess/FeatureDetectorFPN.cpp
    FrameProcess/FeatureDetectorFPN.hpp
    FrameProcess/FeatureFactory.cpp
    FrameProcess/FeatureFactory.hpp
    FrameProcess/FrameProcessNode.cpp
    FrameProcess/FrameProcessNode.hpp
    FrameProcess/FrameProcessNodeFactory.cpp
    FrameProcess/FrameProcessNodeFactory.hpp
    FrameProcess/HoughFPNode.cpp
    FrameProcess/HoughFPNode.hpp
    FrameProcess/FindContour.hpp
    FrameProcess/FindContour.cpp
    FrameProcess/ObjectDetectorFPN.cpp
    FrameProcess/ObjectDetectorFPN.hpp
    FrameProcess/CcalibFPN.cpp
    FrameProcess/CcalibFPN.hpp
    FrameProcess/CascadeDetectorFPN.cpp
    FrameProcess/CascadeDetectorFPN.hpp
    FrameProcess/OpticalFlowFPN.cpp
    FrameProcess/OpticalFlowFPN.hpp
    FrameProcess/HoughCirclesFPN.cpp
    FrameProcess/HoughCirclesFPN.hpp
    FrameProcess/HoughCirclesFPN.cpp
    FrameProcess/LsdSlamFPN.hpp
    FrameProcess/LsdSlamFPN.cpp


    ocvstd.hpp
    Calibrate.hpp
    Calibrate.cpp
    camera.hpp
    camera.cpp
    main.cpp
    utils.cpp
    VideoProcess.cpp
    VideoProcess.hpp)

add_executable(ocv ${SOURCE_FILES})

