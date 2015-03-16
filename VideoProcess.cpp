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

// ...................................................................... errors

// ========================================================== class VideoProcess


void
VideoProcess::init()
{
    // make errs into OcvError
    ErrorCaptureFailure         = make_err( "Video Process: capture failure" );
    ErrorSetupFrameProcessNode  = make_err( "Video Process: setup Frame-Process-Node failure" );

    capture_width  = get( CAP_PROP_FRAME_WIDTH  );
    capture_height = get( CAP_PROP_FRAME_HEIGHT );

    LOG( LEVEL_INFO ) << "Video process " << capture_width << "x"
                                          << capture_height;
    set_abort_key( KEY_ESCAPE );
    set_abort_key( 'q' );
    set_abort_key( 'Q' );
    
    ready = isOpened();
    
    // are we good to go?
    if ( !ready ) set_err( ErrorCaptureFailure );
}

// .................................................................. print_desc
void
VideoProcess::print_desc()
{
    string desc;
    desc += "Video Process contains the following steps (";
    desc += to_string( processors.size() );
    desc += "):";
    
    LOG( LEVEL_INFO ) << desc;
    
    for ( size_t ix = 0 ; ix < processors.size(); ix++ ){
        
        FrameProcessNode *fp = processors[ ix ];
        
        if ( fp ){
            desc = "";
            fp->print_desc( &desc );
            
        }
        else{
            desc = " invalid fp! ";
        }
        
        LOG( LEVEL_INFO ) << "(" << ix << ") " << desc;
    }
}

// ....................................................................... setup

bool VideoProcess::setup( vector<argv_t> *v_argv )
{
    bool ok = is_ready();
    
    if ( ok ){
        for( int ix = 0; ix < v_argv->size(); ix++ ){
            argv_t *args  = &(*v_argv)[ ix ];
            const char *fpn = nullptr;
            
            // look for fpn field..
            for( auto it  = args->begin(); it != args->end()  ;it++ ){
                if ( 0 == strcmp( it->first, "fpn" ) ){
                    fpn = it->second;
                    break;
                }
            }
            
            // setup args or break on failure
            if ( !(ok = setup( fpn, args )) ){
                // errors are set in the setup method..
                break;
            }
        }
    }
    
    // sanity check : do we have processors?!
    if (ok){
        ok    = ( processors_num() > 0 );
        ready = ok;
        if (!ok) set_err( ErrorSetupFrameProcessNode   ,
                          "No processors in pipeline!" );
    }
    
    return ok;
}

// TODO: break even further to increase reuse..
bool
VideoProcess::setup( const char *key, const char *val )
{
    bool ok = false;
    
    if ( STR_EQ( key, "width" ) ){
        int    tmp   = capture_width;
        string sval = val;
        try{
            ok = true;
            capture_width = stoi(val);
        }
        catch( exception const & e ){
            capture_width = tmp;
            ok = false;
        }
        if (ok){
            set( CAP_PROP_FRAME_WIDTH, capture_width );
            LOG( LEVEL_INFO ) << "Video Process set Width : "
                              << capture_width;
        }
    }
    else if ( STR_EQ( key, "height" ) ){
        int    tmp  = capture_height;
        string sval = val;
        try{
            ok = true;
            capture_height = stoi(val);
        }
        catch( exception const & e ){
            capture_height = tmp;
            ok = false;
        }
        if (ok){
            set( CAP_PROP_FRAME_HEIGHT, capture_height );
            LOG( LEVEL_INFO ) << "Video Process set Height : "
                              << capture_height;
        }
    }
    
    if (!ok){
        string msg = "("; msg += key; msg += ","; msg += val; msg += ")";
        set_err( INVALID_ARGS, msg.c_str() );
    }
    
    return ok;
}

// ....................................................................... setup

bool
VideoProcess::setup( argv_t *args )
{
    // TODO: add video process args setup here..
    bool ok = (args != nullptr);
    
    for( auto it = args->begin(); it != args->end() ; it++ ){
        ok = setup( it->first, it->second );
        if (!ok){
            LOG( LEVEL_WARNING ) << "unknown arg <"
                                 << it->first << "," << it->second;
        }
    }
    
    // emit errors
    if ( !ok ){
        string err_msg = "Error setting args for VideoProcess class";
        set_err( INVALID_ARGS, err_msg  );
        ready = false;
    }

    return ok;
}

bool
VideoProcess::setup( const char* name, argv_t *args )
{
    FrameProcessNode *fpn = FrameProcessNodeFactory::make( name );
    bool   ok = fpn? setup( fpn, args ) : setup( args );
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
        
        set_err( ErrorSetupFrameProcessNode, err_msg );
        ready = false;
    }
    
    return ok;
}

// ===================================================================== process

// ..................................................................... process
bool
VideoProcess::process()
{
    bool not_done = true;
    bool ok       = true;
    
    while ( not_done ) {
        
        *this >> in;
        
        // invoke all frame processor nodes in chain
        
        for( auto it = processors.begin(); it != processors.end(); it++ ){
            
            FrameProcessNode *fp = *it;
            
            // process pressed keys
            fp->process_key( get_key() );
            
            // process frame
            if ( !( ok = fp->process_one_frame() ) ) break;
        }
        
        not_done = ok && process_key();
    }
    
    return ok;
}

// ................................................................. process_key
bool
VideoProcess::process_key( int key )
{
    // To simulate a key press, just pass a valid key value.
    // otherwise the default val is set -1 that is a flag for sampling one
    // from the user.. waitKey returns -1 is no key was pressed
    // 30 ms is a magic number recommended to give highgui time to
    // process window events like imshow, etc.
    
    if ( key < 0 ){
         key = pressed_key = waitKey( 30 );
    }
    
    if ( key > 0 ){
        char ch = (char)( key & 0xFF);
        string s_key; to_key( key, s_key );

        LOG( LEVEL_INFO ) <<  "key " << s_key << " pressed..";
        
        if( abort_keys.find( ch ) != string::npos ) return false;  // abort!
    }
    
    // keep running - don't abort!
    return true;
}
