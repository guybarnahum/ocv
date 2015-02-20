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

        if ( dbg ) cout << lines.size() << " found" << endl;
        
        for( size_t ix = 0; ix < lines.size(); ix++ ){
            
            Vec4i l = lines[ ix ];
            
            line( out,   Point( l[0], l[1] ), Point( l[2], l[3] ),
                                     Scalar(0,0,255), 3, LINE_AA);
            
            if ( dbg ){
                cout << "("   << l[0] << "," << l[1] <<
                        ")-(" << l[3] << "," << l[4] << ")" << endl;
            }
        }
        
        imshow( window, out );
    }

    return true;
}
