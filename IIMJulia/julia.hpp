//
//  julia.hpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import "complex.hpp"
#import "extent.hpp"
#import "histogram.hpp"

typedef void (^julia_result)(const Histogram *histogram); // block must not hold onto the histogram

extern void julia_random_iim(Complex c, Extent xExtent, Extent::Component yCenter, unsigned long width, unsigned long height, julia_result result);
extern void julia_miim(Complex c, Extent xExtent, Extent::Component yCenter, unsigned long width, unsigned long height, julia_result result);
