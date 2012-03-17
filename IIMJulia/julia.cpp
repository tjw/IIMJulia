//
//  julia.cpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import "julia.hpp"

#import <stdio.h>

extern "C" {
#import <OmniFoundation/OFRandom.h>
}

static Complex _iim_julia_preimage(OFRandomState *state, Complex u, Complex c)
{
    Complex w = (u - c).sqrt();
    
    // TODO: Could grab 64 bits and use them up one by one.
    if (OFRandomNextState64(state) & 0x1)
        w = -w;
    
    return w;
}

void iim_julia_histogram(Complex c, Extent xExtent, Extent yExtent, Histogram *histogram)
{
    OFRandomState *state = OFRandomStateCreate();
    
    double xStep = histogram->width() / xExtent.length();
    double yStep = histogram->height() / yExtent.length();
    
    unsigned tries = 1000;
    while (tries--) {
        Complex u = Complex(OFRandomNextDouble(), OFRandomNextDouble());
        //fprintf(stderr, "Try %f, %f\n", u.r, u.i);
        
        // Skip the first few preimages while it settles down
        unsigned skip = 20;
        while (skip--) {
            u = _iim_julia_preimage(state, u, c);
            //fprintf(stderr, "  preimage %f, %f\n", u.r, u.i);
        }
        
        // Record some preimages into the histogram
        unsigned int captures = 1000;
        while (captures--) {
            u = _iim_julia_preimage(state, u, c);
            //fprintf(stderr, "  value %f, %f\n", u.r, u.i);
            
            if (!xExtent.contains(u.r) || !yExtent.contains(u.i)) {
                //fprintf(stderr, "    outside\n");
                continue;
            }
            
            unsigned column = floor(xStep * (u.r - xExtent.min()));
            unsigned row = floor(yStep * (u.i - yExtent.min()));
            
            //fprintf(stderr, "    incr %u, %u\n", column, row);
            histogram->increment(column, row);
        }
    }
    
    OFRandomStateDestroy(state);
}
