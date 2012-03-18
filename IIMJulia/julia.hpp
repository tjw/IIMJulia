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

typedef void (^iim_julia_histogram_result)(const Histogram *histogram); // block must not hold onto the histogram

extern void iim_julia_histogram(Complex c, Extent xExtent, Extent yExtent, unsigned long width, unsigned long height, iim_julia_histogram_result result);
