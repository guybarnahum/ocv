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

#include "ocvstd.hpp"
#include "utils.hpp"

#include "FrameProcessNode.hpp"
#include "FrameProcessNodeFactory.hpp"

// =========================================== class VideoProcess : VideoCapture

class VideoProcess : public VideoCapture{
  
public:
    // ................................................................. members
    argv_t  args;
    bool    ready;
    int     abort_key;
    
    Mat     in;
    vector<Vec4i> lines;
    Rect          focus;
    
    vector<FrameProcessNode *> processors;
    
    // NOTE: some drivers do not support setting up the
    // video capture resolution
    
    int     capture_width;
    int     capture_height;
    
    // .................................................................... errs
    err_t ErrorSetupFrameProcessNode;
    err_t ErrorCaptureFailure;
    
public:
    
    // ............................................................ constructors
    
     VideoProcess( const String& filename ):VideoCapture( filename ){ init();}
     VideoProcess( int           device   ):VideoCapture( device   ){ init();}
    ~VideoProcess(){};
    
    void init();
    
    // ................................................................. methods
    bool is_ready(){ return ready; }
    size_t processors_num(){ return processors.size(); }
    
    bool setup( vector<argv_t> *v_argv ); // setup everything
    bool setup( argv_t *args = nullptr ); // setup self only
    bool setup( const char *key, const char *val ); 

    // setup a frame process node into pipeline
    bool setup( const char*       name, argv_t *args = nullptr ); // fpn by name
    bool setup( FrameProcessNode *fpn , argv_t *args = nullptr ); // fpn
    
    void print_desc();
    bool process();
    virtual bool process_key( int key = -1 );

    // ....................................................... getters / setters
    
    int  set_abort_key( int key ){ int k=abort_key; abort_key=key; return k; }
    int  get_abort_key(){ return  abort_key; }
};

#endif /* defined(ocv_VideoProcess_hpp) */
