// =============================================================================
//
//  FrameProcessNode.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_FrameProcessNode_hpp
#define ocv_FrameProcessNode_hpp

// ==================================================================== includes
#include <iostream>
#include <opencv/cv.hpp>

using namespace cv;
using namespace std;

// ====================================================== class FrameProcessNode
//
// These graph nodes are connected in a processing chain, each node working on
// results from the previous one..
//
class FrameProcessNode{

private:

    // ..................................................... node identification
    const char *cname;
    const char *desc;

protected:
    // ................................................................ node i/o
    Mat *base;
    Mat *in;
    Mat  out;
    const char *window;
    bool dbg;
    
    // FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
    //
    // TODO: define a better connector for frame processing nodes
    // TODO: add argv, argc
    // TODO: add multi in and out Mats
    // TODO: add geometry as input and output
    // TODO: combine all inputs: argv, argc, geometry and Mats into
    //       context or input class that could be either types..
    //
    // FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME

public:

    // ................................................................. methods
     FrameProcessNode();
    ~FrameProcessNode();
 
    virtual bool process_one_frame();
    void print_desc( ostream &out_stream );
    bool setup( void *args );
    
    // ............................................................... get / set
    Mat *get_in()        { return in;  }
    void set_in(Mat *in) { this->in = in; }
    
    Mat *get_base()          { return base;  }
    void set_base(Mat *base) { this->base = base; }
    
    Mat *get_out(){ return &out;}
    
    void        set_name(      const char *cname );
    const char* get_name(){   return this->cname; }
    
    void        set_desc(      const char *desc  );
    const char* get_desc(){   return this->desc; }

    void        set_window(    const char *window );
    const char* get_window(){ return this->window; }
};

#endif /* defined(ocv_FrameProcessNode_hpp) */
