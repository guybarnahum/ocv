cmake_minimum_required(VERSION 2.8.4)
project(ocv)

link_directories( /usr/local/lib)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++11 -I${CMAKE_SOURCE_DIR} \
    -I${CMAKE_SOURCE_DIR}/FrameProcess \
    -I/usr/local/include/ \
    -L/usr/local/lib/ \
    -lopencv_core.3.0.0 -lopencv_highgui.3.0.0 \
    -lopencv_video.3.0.0 -lopencv_videoio.3.0.0 \
    -lopencv_videostab.3.0.0 -lopencv_imgcodecs.3.0.0 \
    -lopencv_imgproc.3.0.0 -lopencv_features2d.3.0.0 \
    -lopencv_xfeatures2d.3.0.0 -lopencv_flann.3.0.0 \
    -lopencv_calib3d.3.0.0 -lopencv_xfeatures2d.3.0.0 \
    -lopencv_ccalib.3.0.0 -lopencv_objdetect.3.0.0 \
    -lopencv_optflow.3.0.0 -lopencv_tracking.3.0.0 \
    -lopencv_ml.3.0.0 -lopencv_xobjdetect.3.0.0")

set(SOURCE_FILES
    cli.cpp
    cli.hpp
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
    main.cpp
    utils.cpp
    VideoProcess.cpp
    VideoProcess.hpp)

add_executable(ocv ${SOURCE_FILES})

