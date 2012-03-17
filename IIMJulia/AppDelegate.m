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
    // Insert code here to initialize your application
    Histogram *H = new Histogram(800, 600);
    iim_julia_histogram(Complex(-0.74543, 0.11301), Extent(-1.0, 1.0), Extent(-1.0, 1.0), H);

    CGImageRef imageRef = create_bw_image(H);
    NSImage *image = [[NSImage alloc] initWithCGImage:imageRef size:CGSizeMake(H->width(), H->height())];
    _imageView.image = image;
    
    CFRelease(imageRef);
    delete H;
}

@end
