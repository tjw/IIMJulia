//
//  histogram.hpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import "grid.hpp"

class Histogram : public Grid<unsigned long> {
public:
    inline Histogram(unsigned long width, unsigned long height) : Grid<unsigned long>(width, height) {
    }
    
    inline Histogram(const Histogram *original) : Grid<unsigned long>(original) {
    }
    
    inline unsigned long increment(unsigned x, unsigned y) {
        unsigned long *bucket = set_at(x, y);
        unsigned long count = *bucket + 1;
        *bucket = count;
        return count;
    }
    
    inline unsigned long count_at(unsigned long x, unsigned long y) const {
        const unsigned long *bucket = get_at(x, y);
        return *bucket;
    }
};

