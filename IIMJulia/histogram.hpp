//
//  histogram.hpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import <stdlib.h>
#import <assert.h>
#import <string.h>

class Histogram {
public:
    inline Histogram(unsigned long width, unsigned long height) {
        _width = width;
        _height = height;
        _buckets = (typeof(_buckets))calloc(sizeof(*_buckets), _width * _height);
    }
    
    Histogram(const Histogram *original) {
        _width = original->_width;
        _height = original->_height;
        _buckets = (typeof(_buckets))calloc(sizeof(*_buckets), _width * _height);
        memcpy(_buckets, original->_buckets, sizeof(*_buckets) * _width * _height);
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
    
    inline unsigned increment(unsigned x, unsigned y) {
        assert(x < _width);
        assert(y < _height);
        return _buckets[y*_width + x]++;
    }
    
    inline unsigned long count_at(unsigned x, unsigned y) const {
        assert(x < _width);
        assert(y < _height);
        return _buckets[y*_width + x];
    }
    
private:
    unsigned long _width, _height;
    unsigned long *_buckets;
    
    Histogram(const Histogram &); // No implicit copies
};

