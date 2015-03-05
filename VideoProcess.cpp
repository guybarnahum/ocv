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
    set_abort_key( KEY_ESCAPE );
    
    ready = isOpened();
 
    // are we good to go?
    if ( ready ) set_err( ERR_OK );
    else         set_err( ERR_CAPTURE_FAILURE, "Video Capture not ready" );
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

bool VideoProcess::setup( vector<argv_t> &v_argv )
{
    bool ok = is_ready();
    
    for( auto it = v_argv.begin(); ok && it != v_argv.end(); it++ ){
            
        // obtain Frame Process Node by name, and set it up with its args
        argv_t *args = &(*it);
        auto it_fpn = it->find( "fpn" );
        const char *fpn = ( it_fpn != it->end() )? it_fpn->second : nullptr;
        ok = setup( fpn, args );
    }
    
    return ok;
}

bool
VideoProcess::setup(argv_t *args)
{
    // TODO: add video process args setup here..
    bool ok = (args != nullptr);
    
    
    // emit errors
    
    if ( !ok ){
        string err_msg = "Error setting args for VideoProcess class";
        set_err( ERR_INVALID_ARGS, err_msg  );
        ready = false;
    }

    return ok;
}

bool
VideoProcess::setup( const char* name, argv_t *args )
{
    FrameProcessNode *fpn = name? FrameProcessNodeFactory::make(name) : nullptr;
    bool ok = fpn? setup( fpn, args ) : setup( args );
    return ok;
}

bool
VideoProcess::setup( FrameProcessNode *fpn, argv_t *args )
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
    
    bool ok = fpn->setup( args );
    
    // Connect node to existing chain..
    if (ok){
        processors.push_back( fpn );
    }
    else{
        // signal that we have a problem..
        string err_msg;
        
        err_msg  = "Error in setup of `";
        err_msg += fpn->get_name();
        err_msg += "` frame process node\n";
        err_msg += fpn->get_err();
        
        set_err( ERR_SETUP_FRAME_PROCESS_NODE, err_msg );
        
        ready = false;
    }
    
    return ok;
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
            
            ok = fp->process_one_frame();
            if (!ok){
                cout << fp->get_name() << " error: " << fp->get_err() << endl;
            }
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
