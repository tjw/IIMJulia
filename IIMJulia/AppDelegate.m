//
//  AppDelegate.m
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import "AppDelegate.h"

#import "julia.hpp"
#import "image.hpp"

@interface AppDelegate ()
@property(nonatomic,strong) IBOutlet NSImageView *imageView;
@end

@implementation AppDelegate

@synthesize window = _window;
@synthesize imageView = _imageView;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    iim_julia_histogram(Complex(-0.74543, 0.11301), Extent(-1.55, 1.55), 0.0/*yCenter*/, 4*2048, 4*1536, ^(const Histogram *H){
        CGImageRef imageRef = create_bw_image(H);
        NSImage *image = [[NSImage alloc] initWithCGImage:imageRef size:CGSizeMake(H->width(), H->height())];
        _imageView.image = image;
        
        CFRelease(imageRef);
    });
}

@end
