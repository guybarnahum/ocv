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

#include "ocvstd.hpp"

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
    // ................................................................. results
    
    vector<Rect> found_objects;
    Rect         focus;
    
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

    const char *get_val( argv_t *argv, const char *key );
    bool get_val_bool  ( argv_t *argv, const char *key, bool &var);
    bool get_val_int   ( argv_t *argv, const char *key, int  &var);

public:

    // ................................................................. methods
     FrameProcessNode();
    ~FrameProcessNode();
 
    // print desc prints if no out_str is provided. Otherwise it replaces "\n"
    // with provided delimiter into the out_str.
    void print_desc( string *out_str = nullptr, string del = " ");
    
    // ocv wrappers
    void window_show(const char *win, Mat mat);
    
    // ............................................................... get / set
    Mat *get_in()        { return in;  }
    void set_in(Mat *in) { this->in = in; }
    
    Mat *get_base()          { return base;  }
    void set_base(Mat *base) { this->base = base; }
    
    Mat *get_out(){ return &out;}
    
    void        set_name( string str ){ set_name( str.c_str() ) ; }
    void        set_name(      const char *cname );
    const char* get_name(){   return this->cname; }

    void        set_desc( string str ){ set_desc( str.c_str() ); }
    void        set_desc(      const char *desc  );
    const char* get_desc(){   return this->desc; }

    void        set_window(    const char *window );
    const char* get_window(){ return this->window; }
    
    // .................................................. image processing utils
    
    bool gray( const Mat &mat, Mat &gray )
    {
        bool ok = true;
        switch( mat.channels() ){
            case 3 : cvtColor( mat, gray, CV_BGR2GRAY ); break;
            case 4 : cvtColor( mat, gray, CV_BGRA2GRAY); break;
            case 1 : gray = mat; break;
            default: ok = false; break;
        }
        return ok;
    }
    // ......................................................... virtual methods
    virtual bool select_focus( const vector<Rect> &rects, Rect &focus);
            bool select_focus(){ return select_focus( found_objects, focus ); }
    
    virtual bool process_one_frame();
    virtual bool setup( argv_t *argv );
};

#endif /* defined(ocv_FrameProcessNode_hpp) */
