// =============================================================================
//
//  ObjectDetectorFPN.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

#ifndef ocv_ObjectDetectorFPN_hpp
#define ocv_ObjectDetectorFPN_hpp

// ==================================================================== includes

#include "ocvstd.hpp"
#include "FeatureDetectorFPN.hpp"

// ================================================== class ObjectDetectorFPNode

class ObjectDetectorFPNode : public FeatureDetectorFPNode {
    
private:
    
    typedef enum{
        NONE,
        DETECTED,
        TRACKING,
    }state_e;

    // obj values are calculated once
    string           obj_path;
    Size             obj_size;
    vector<Point2f>  obj_src;
    
    // output : location object in scene
    vector<Point>    obj_dst;

    bool setup( string path );
    bool matcher_train();

    state_e state;
    
    void set_state( state_e st ){ state = st; }
    const char * to_string( state_e st)
    {
        const char *str;
        switch( st ){
            default         : str = "UNKNOWN" ; break;
            case NONE       : str = "NONE"    ; break;
            case DETECTED   : str = "DETECTED"; break;
            case TRACKING   : str = "TRACKING"; break;
        }
        
        return str;
    }
    
public:
    
     ObjectDetectorFPNode();
    ~ObjectDetectorFPNode(){};
    
    bool obj_transform();
    bool track();
    bool is_valid_rect( vector<Point> &poly, double min_area = 0 );

    // ....................................................... overriden methods

    bool setup( argv_t *argv );
    bool process_one_frame();
};

#endif /* defined(ocv_ObjectDetectorFPN_hpp) */
