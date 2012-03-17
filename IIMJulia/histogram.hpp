//
//  histogram.hpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import <stdlib.h>

class Histogram {
public:
    inline Histogram(unsigned long width, unsigned long height) {
        _width = width;
        _height = _height;
        _buckets = (typeof(_buckets))calloc(sizeof(*_buckets), _width * _height);
    }
    
    inline ~Histogram() {
        if (_buckets)
            free(_buckets);
    }
    
    inline unsigned long width(void) const {
        return _width;
    }
    inline unsigned long height(void) const {
        return _height;
    }
    
private:
    unsigned long _width, _height;
    unsigned long *_buckets;
};

