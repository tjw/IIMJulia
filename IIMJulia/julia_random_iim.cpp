//
//  julia.cpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import "julia.hpp"

#import <stdio.h>
#import <dispatch/dispatch.h>
#import <CoreFoundation/CFDate.h>
#import "image.hpp"

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

void julia_random_iim(Complex c, Extent xExtent, Extent::Component yCenter, unsigned long width, unsigned long height, julia_result result)
{
    assert(width > 0);
    assert(height > 0);
    assert(result);
    
    dispatch_queue_t resultQueue = dispatch_get_current_queue();
    dispatch_retain(resultQueue);
    
    result = [result copy];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        // Make the y-axis scale to the x-axis
        Extent::Component yLength = (xExtent.length() / width) * height;
        Extent yExtent(yCenter - yLength/2, yCenter + yLength/2);
        
        CFAbsoluteTime lastNotifyTime = CFAbsoluteTimeGetCurrent();
        OFRandomState *state = OFRandomStateCreate();
        Histogram *histogram = new Histogram(width, height);
        
        double xStep = width / xExtent.length();
        double yStep = height / yExtent.length();
        
        while (YES) {
            unsigned tries = 100;
            while (tries--) {
                Complex u = Complex(4 * OFRandomNextDouble() - 2, 4 * OFRandomNextDouble() - 2);
                //fprintf(stderr, "Try %f, %f\n", u.r, u.i);
                
                // Skip the first few preimages while it settles down
                unsigned skip = 20;
                while (skip--) {
                    u = _iim_julia_preimage(state, u, c);
                    //fprintf(stderr, "  preimage %f, %f\n", u.r, u.i);
                }
                
                // Record some preimages into the histogram
                unsigned int captures = 1000000;
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
                    if (histogram->increment(column, row) > 255)
                        break; // This bucket is plenty full. Try a new random seed
                }
            }
            
            CFAbsoluteTime currentTime = CFAbsoluteTimeGetCurrent();
            if (currentTime - lastNotifyTime > 1) {
                lastNotifyTime = currentTime;
                
                CGImageRef imageRef = create_bw_image(histogram);
                dispatch_async(resultQueue, ^{
                    result(imageRef);
                    CFRelease(imageRef);
                });
            }
        }
        
        OFRandomStateDestroy(state);
    });
}
