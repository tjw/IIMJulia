//
//  image.cpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import "image.hpp"

#import <stdlib.h>

static void _freeDataProviderReleaseDataCallback(void *info, const void *data, size_t size)
{
    free((void *)data);
}

CGImageRef create_bw_image(const Histogram *H)
{
    const unsigned long width = H->width();
    const unsigned long height = H->height();
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericGray);
    CGBitmapInfo bitmapInfo = kCGImageAlphaNone;

    uint8_t *buffer = (uint8_t *)malloc(width * height);
    for (unsigned long row = 0; row < height; row++) {
        for (unsigned long column = 0; column < width; column++) {
            unsigned long count = H->count_at(column, row);
            
            uint8_t *pixel = &buffer[row*width + column];
            if (count > 0)
                *pixel = 255;
            else
                *pixel = 0;
        }        
    }
    
    CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL/*info*/, buffer/*data*/, width * height/*size*/, _freeDataProviderReleaseDataCallback);

    CGImageRef image = CGImageCreate(H->width(), H->height(),
                                     8/*bitsPerComponent*/, 8/*bitsPerPixel*/, H->width()/*bytesPerRow*/,
                                     colorSpace, bitmapInfo, dataProvider,
                                     NULL/*decode*/, false/*shouldInterpolate*/,
                                     kCGRenderingIntentDefault);
    CFRelease(dataProvider);
    
    return image;
}

