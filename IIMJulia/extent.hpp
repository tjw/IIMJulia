//
//  range.h
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

class Extent {
public:
    typedef double Component;
    
    inline Extent(Component start, Component length) {
        _start = start;
        _length = length;
    }
    
private:
    Component _start;
    Component _length;
};
