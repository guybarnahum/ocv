// =============================================================================
//
//  FrameProcessNode.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#include "FrameProcessNode.hpp"

// ========================================================== class FrameProcess

FrameProcessNode::FrameProcessNode()
{
    // base class does not call set_name and set_desc 
    cname = strdup( "FrameProcess" );
    desc  = strdup( "frame process base class - copies in to out" );
    window= nullptr;
    
    // nothing to do in base class - input === output
    in    = &out;
    
    dbg = false;
}

FrameProcessNode::~FrameProcessNode()
{
    delete cname;
    delete desc;
    delete window;
}

// ................................................................ ocv wrappers

void FrameProcessNode::window_show(const char *win, Mat mat)
{
    if ( win && ( mat.cols > 0 ) && ( mat.rows > 0 )){
     
        try{
            imshow( win, mat );
        }
        catch( Exception e ){
            LOG( LEVEL_ERROR ) << e.what();
        }
    }
}

// ..................................................................... get_val

// BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG
//
// For some reason, don't ask me why, the map::find routine does not find keys
// ( std::map<const char*, const char *> or argv_t ).
//
// auto it = argv->find( key );
// if ( it != argv->end() ) return it->second;
//
// argv->find( key ) returns an iterator pointing to argv->end()! for keys
// that exists in the map!! WTF?! Please teach me c++ and let me know what
// is going in here..
//
// Right now a simple loop over the keys finds values for keys.. as a workaround
//
// BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG

const char *FrameProcessNode::get_val( argv_t *argv, const char *key )
{
    if ( argv ){
        for( auto it = argv->begin(); it != argv->end(); it++ ){
            if ( 0 == strcmp( key, it->first ) ){
                return it->second;
            }
        }
    }
    
#if 0
    if ( argv ){
        auto it = argv->find( key );
        if ( it != argv->end() ) return it->second;
    }
#endif
    
    return nullptr;
}

// ................................................................ get_val_bool

bool FrameProcessNode::get_val_bool( argv_t *argv, const char *key, bool &var)
{
    bool ok = true;
    const char *val = get_val( argv, key );

    // key not found? we are ok!
    if ( val == nullptr ) return ok;
    
    // key found? examine it:
    // values are only true or false or empty, anything else is invalid!
    bool b = true;
    
         if STR_EQ( val, "false" ) b = false;
    else if STR_EQ( val, ""      ) b = true;
    else if STR_EQ( val, "true"  ) b = true;
    else ok = false;
    
    if (ok) var = b;
    return ok;
}

// ................................................................. get_val_int

bool FrameProcessNode::get_val_int( argv_t *argv, const char *key, int  &var)
{
    bool ok = true;
    const char *val = get_val( argv, key );
    
    // key not found? we are ok!
    if ( val == nullptr ) return ok;
    
    // key found? examine it: only numbers are accepted!
    int i;
    string str( val );
    
    try{
        i = stoi(str);
    }
    catch( exception const & e ){
        ok = false;
    }

    if ( ok ) var = i;
    
    return ok;
}

// ....................................................................... setup
//
// Consider to:
// TODO: define a better connector for frame processing nodes
// TODO: add argv, argc
// TODO: add multi in and out Mats
// TODO: add geometry as input and output
// TODO: combine all inputs: argv, argc, geometry and Mats into
//       context or input class that could be either types..
//
// .............................................................................

bool FrameProcessNode::setup( argv_t *args )
{
    if ( args == nullptr ) return false;
    
    // child classes may call the parent setup for base class setup options
    const char *val = nullptr;
    
    // window
    val = get_val( args, "window" );
    if ( val != nullptr ) set_window( val );
    
    // dbg
    bool  ok = get_val_bool( args, "dbg", dbg );
    if ( !ok ){
        set_err( INVALID_ARGS, "dbg option should be boolean");
    }
    
    return ok;
}

// ..................................................................... setters

void FrameProcessNode::set_window( const char *window)
{
    delete this->window;
    this->window = nullptr;
    
    if ( window ){
        this->window = strdup(window);
        namedWindow(this->window);
    }
}

void FrameProcessNode::set_name( const char *cname )
{
    delete this->cname;
    this->cname = cname? strdup(cname) : nullptr;
}

void FrameProcessNode::set_desc(    const char *desc  )
{
    delete this->desc;
    this->desc  = desc? strdup(desc) : nullptr;
}

// ................................................................ select_focus

bool FrameProcessNode::select_focus( const vector<Rect> &rects, Rect &focus)
{
    bool ok = rects.size() > 0;
    
    if ( ok ){
        Rect max = rects[ 0 ];
        
        for( size_t ix = 1; ix < rects.size(); ix++ ){
            if ((         max.width *         max.height )<
                ( rects[ ix ].width * rects[ ix ].height ) ){
                max = rects[ ix ];
            }
        }
        
        focus = max;
    }
    
    return ok;
}

// ........................................................... process_one_frame
bool FrameProcessNode::process_one_frame()
{
    window_show( window, out );
    return true;
}

// ................................................................. process_key
bool FrameProcessNode::process_key( int key )
{
    UNUSED( key );
    return true;
}

// .................................................................. print_desc
// print desc prints if no out_str is provided. Otherwise it replaces "\n"
// with provided delimiter into the out_str.
void FrameProcessNode::print_desc( string *out_str, string del )
{
    string str = get_name();
    
    if ( out_str != nullptr ){
        *out_str += str;
        *out_str += del;
    }
    else{
        LOG( LEVEL_INFO ) << str;
    }
    
    str = get_desc();
    std::stringstream ss( str );
    
    char line[ 256 ];
    
    while ( ss.getline( line, 256 ) ) {
        if ( out_str != nullptr ){
            *out_str += line; *out_str += del;
        }
        else{
            LOG( LEVEL_INFO ) << line;
        }
    }
}
