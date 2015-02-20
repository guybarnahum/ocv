// =============================================================================
//
//  VideoProcess.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_VideoProcess_hpp
#define ocv_VideoProcess_hpp

// ==================================================================== includes
#include <iostream>
#include <opencv/cv.hpp>

#include "FrameProcessNodeFactory.hpp"

// ======================================================================== enum

enum{
    KEY_ESCAPE = 27,
    KEY_SPACE  = 32,
};

enum{
    ERR_OK              =  0,
    ERR_CAPTURE_FAILURE = -1,
};

// =========================================== class VideoProcess : VideoCapture

class VideoProcess : public VideoCapture{
  
public:
    // ................................................................. members
    bool ready;
    
    int abort_key;
    
    Mat  in;
    
    vector<Vec4i> lines;
    vector<FrameProcessNode *> processors;
    
public:
    
    // ............................................................ constructors
    
     VideoProcess( const String& filename ):VideoCapture( filename ){ init();}
     VideoProcess( int           device   ):VideoCapture( device   ){ init();}
    ~VideoProcess();
    
    void init();
    
    // ................................................................. methods
    bool is_ready(){ return ready; }

    void setup( const char*     cname, void *args = nullptr );
    void setup( FrameProcessNode *fpn, void *args = nullptr );

    void print_desc( ostream &out_stream );

    bool process();
    virtual bool process_key( int key = -1 );

    int  set_abort_key( int key ){ int k=abort_key; abort_key=key; return k; }
    int  get_abort_key(){ return  abort_key; }

};

#endif /* defined(ocv_VideoProcess_hpp) */
