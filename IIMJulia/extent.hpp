//
//  range.h
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import <assert.h>

class Extent {
public:
    typedef double Component;
    
    inline Extent(Component min, Component max) {
        assert(min < max);
        _min = min;
        _max = max;
    }
    
    // TODO: Make this inclusive, or not?
    inline bool contains(Component x) const {
        return x >= _min && x <= _max;
    }

    inline Component min(void) const {
        return _min;
    }
    
    inline Component max(void) const {
        return _max;
    }
    
    inline Component length(void) const {
        return _max - _min;
    }
    
private:
    Component _min;
    Component _max;
};
