//
//  AppDelegate.m
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import "AppDelegate.h"

#import "julia.hpp"

@interface AppDelegate ()
{
    Complex _c;
    CGImageRef _imageRef;
    NSImage *_image;
}

@property(nonatomic,strong) IBOutlet NSImageView *imageView;

@property(nonatomic,strong) IBOutlet NSTextField *rTextField;
@property(nonatomic,strong) IBOutlet NSTextField *iTextField;

@property(nonatomic,assign) double rValue;
@property(nonatomic,assign) double iValue;

@end


@implementation AppDelegate

@synthesize window = _window;
@synthesize imageView = _imageView;
@synthesize rTextField = _rTextField;
@synthesize iTextField = _iTextField;

- (double)rValue;
{
    return _c.r;
}
- (void)setRValue:(double)rValue;
{
    _c.r = rValue;
}

- (double)iValue;
{
    return _c.i;
}
- (void)setIValue:(double)iValue;
{
    _c.i = iValue;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSScreen *screen = [NSScreen mainScreen];
    NSRect frame = [screen frame];
    frame = [screen convertRectToBacking:frame];
    NSLog(@"screen size %@", NSStringFromRect(frame));
    
    CGSize imageSize = frame.size;
    
    _c = Complex(-0.74543, 0.11301);
    
    julia_miim(_c, Extent(-1.55, 1.55), 0.0/*yCenter*/, 4*imageSize.width, 4*imageSize.height, ^(CGImageRef imageRef){
        CGImageRelease(_imageRef);
        _imageRef = (CGImageRef)CFRetain(imageRef);
        _image = [[NSImage alloc] initWithCGImage:_imageRef size:CGSizeMake(CGImageGetWidth(_imageRef), CGImageGetHeight(_imageRef))];
        _imageView.image = _image;
    });
}

- (IBAction)saveImage:(id)sender;
{
    NSSavePanel *savePanel = [NSSavePanel savePanel];
    [savePanel setAllowedFileTypes:[NSArray arrayWithObjects:(id)kUTTypePNG, nil]];
    [savePanel beginSheetModalForWindow:_window completionHandler:^(NSInteger result){
        if (result != NSFileHandlingPanelOKButton)
            return;
        
        CGImageDestinationRef dest = CGImageDestinationCreateWithURL((__bridge CFURLRef)[savePanel URL], kUTTypePNG, 1, NULL/*options*/);
        CGImageDestinationAddImage(dest, _imageRef, NULL/*properties*/);
        if (!CGImageDestinationFinalize(dest)) {
            NSLog(@"Unable to finalize destination.");
        } else
            CFRelease(dest);
    }];
}
    
@end
