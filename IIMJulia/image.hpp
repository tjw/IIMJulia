//
//  image.hpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import "histogram.hpp"

#import <ApplicationServices/ApplicationServices.h>

extern CGImageRef create_bw_image(const Histogram *H) CF_RETURNS_RETAINED;
