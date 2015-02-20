// =============================================================================
//
//  frameProcess.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================


// ==================================================================== includes

#include "VideoProcess.hpp"

// ................................................................ is_printable
bool
is_printable( int key )
{
    return ( key > 32 ) && ( key < 127 );
}

// ....................................................... pre_process_one_frame
void
print_key( int key )
{
    switch( key ){
        case KEY_ESCAPE : cout << "<escape>"; break;
        case KEY_SPACE  : cout << "<space>" ; break;
        default         :
            if ( is_printable( key ))
                cout << "'" << (char)key << "'";
            else
                cout << "<" << key << ">" ;
            break;
    }
}

// ========================================================== class VideoProcess

void
VideoProcess::init()
{
    ready = isOpened();
    set_abort_key( KEY_ESCAPE );
}

VideoProcess::~VideoProcess()
{
    
}

// .................................................................. print_desc
void
VideoProcess::print_desc( ostream &out_stream )
{
    string desc;
    desc += "Video Process contains the following steps (";
    desc += to_string( processors.size() );
    desc += " )\n";
    
    out_stream << desc;
    
    for ( auto it  = processors.begin() ;
         it != processors.end()   ; it++ ){
        
        FrameProcessNode *fp = *it;
        
        assert( fp );
        
        fp->print_desc( out_stream );
    }
    
    out_stream << endl;
}

// ....................................................................... setup
void
VideoProcess::setup( const char* cname, void *args )
{
    FrameProcessNode *fpn = FrameProcessNodeFactory::make( cname );
    setup( fpn, args );
}

void
VideoProcess::setup( FrameProcessNode *fpn, void *args )
{
    // Connect node input into last node output
    // -- unless its the first one connect it into frame stream
    if ( processors.empty() ){
        fpn->set_in( &in );
    }
    else{
        FrameProcessNode *last = processors.back();
        fpn->set_in( last->get_out() );
    }

    // setup node with arguments
    // FIXME TODO: args are a hack right now..
    fpn->set_base( &in );
    fpn->setup( args );
    
    // Connect node to existing chain..
    processors.push_back( fpn );
}

// ===================================================================== process

// ..................................................................... process
bool
VideoProcess::process()
{
    bool done = false;
    bool ok   = true;
    
    while ( !done ) {
        
        *this >> in;
        
        // invoke all frame processor nodes in chain
        auto    it  = processors.begin() ;
        while ( it != processors.end  () ){
            
            FrameProcessNode *fp = *it++;
            if (ok) ok = fp->process_one_frame();
        }
        
        done = !ok || process_key();
    }
    
    return ok;
}

// ................................................................. process_key
bool
VideoProcess::process_key( int key )
{
    if ( key < 0 ){
        key = waitKey(1);
    }
    
    if ( key > 0 ){
        
        cout <<  " key " ; print_key( key ) ; cout << " pressed.." << endl;
        if( key == abort_key ) return true;
    }
    
    return false;
}
