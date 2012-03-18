//
//  grid.hpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/18/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import <stdlib.h>
#import <assert.h>
#import <string.h>

template<typename T> class Grid  {
public:
    inline Grid(unsigned long width, unsigned long height) {
        _width = width;
        _height = height;
        _buckets = (typeof(_buckets))calloc(sizeof(*_buckets), _width * _height);
    }
    
    Grid(const Grid *original) {
        _width = original->_width;
        _height = original->_height;
        _buckets = (typeof(_buckets))calloc(sizeof(*_buckets), _width * _height);
        memcpy(_buckets, original->_buckets, sizeof(*_buckets) * _width * _height);
    }
    
    inline ~Grid() {
        if (_buckets)
            free(_buckets);
    }
    
    inline unsigned long width(void) const {
        return _width;
    }
    inline unsigned long height(void) const {
        return _height;
    }
    
    inline const T* get_at(unsigned long x, unsigned long y) const {
        assert(x < _width);
        assert(y < _height);
        return &_buckets[y*_width + x];
    }
    
    inline T* set_at(unsigned long x, unsigned long y) {
        assert(x < _width);
        assert(y < _height);
        return &_buckets[y*_width + x];
    }
    
private:
    unsigned long _width, _height;
    T *_buckets;
    
    Grid(const Grid &); // No implicit copies
};
