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
    
    err   = "";
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

// ..................................................................... get_val
const char *
FrameProcessNode::get_val( argv_t *argv, const char *key )
{
    if ( argv == nullptr ) return nullptr;
    
    auto it = argv->find( key );
    const char  *val = ( it != argv->end() )? it->second : nullptr;
    return val;
}

// ....................................................................... setup
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

bool
FrameProcessNode::setup( argv_t *args )
{
    if ( args == nullptr ) return false;
    
    // child classes may call the parent setup for base class setup options
    const char *val = nullptr;
    
    // window
    val = get_val( args, "window" );
    if ( val != nullptr ) set_window( val );
    
    // dbg
    val = get_val( args, "dbg" );
    if ( val != nullptr ){
        // TODO: add support for various dbg levels with `val`
        dbg = true;
    }
    
    return true;
}

// ..................................................................... setters
void
FrameProcessNode::set_window( const char *window)
{
    delete this->window;
    this->window = nullptr;
    
    if ( window ){
        this->window = strdup(window);
        namedWindow(this->window);
    }
}

void
FrameProcessNode::set_name( const char *cname )
{
    delete this->cname;
    this->cname = cname? strdup(cname) : nullptr;
}


void
FrameProcessNode::set_desc(    const char *desc  )
{
    delete this->desc;
    this->desc  = desc? strdup(desc) : nullptr;
}

// ........................................................... process_one_frame
bool
FrameProcessNode::process_one_frame()
{
    if ( window ){
        imshow( window, out );
    }
    
    return true;
}

// .................................................................. print_desc
void
FrameProcessNode::print_desc( ostream &out_stream )
{
    string desc;

    desc += ">> ";
    desc += get_name() ;
    desc += " class: " ;
    desc += get_desc();
    desc += "\n";

    out_stream << desc;
}
