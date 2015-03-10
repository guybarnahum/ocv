// =============================================================================
//
//  HoughLineFPNode.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 2/16/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes

#include "HoughLineFPNode.hpp"

// ============================================================= HoughLineFPNode

// ........................................................... process_one_frame
bool
HoughLineFPNode::process_one_frame()
{
    HoughLinesP( *in, lines, 1, CV_PI/180, 50, 50, 10 );
    
    if ( window ){
        
        base->copyTo( out );

        LOG( LEVEL_DEV ) << lines.size() << " found" ;
        
        for( size_t ix = 0; ix < lines.size(); ix++ ){
            
            Vec4i l = lines[ ix ];
            
            line( out,   Point( l[0], l[1] ), Point( l[2], l[3] ),
                                     Scalar(0,0,255), 3, LINE_AA);
            
            LOG( LEVEL_DEV ) << "("   << l[0] << "," << l[1] <<
                                ")-(" << l[3] << "," << l[4] << ")" ;
        }
        
        window_show( window, out );
    }

    return true;
}
