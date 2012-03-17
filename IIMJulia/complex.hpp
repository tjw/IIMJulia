//
//  complex.cpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import <math.h>

class Complex {
public:
    typedef double Component;
    Component r, i;
    
    inline Complex(double inR, double inI) {
        r = inR;
        i = inI;
    }
    
    inline Complex operator+(const Complex &other) const {
        return Complex(r + other.r, i + other.i);
    }
    
    inline Complex operator-(void) const {
        return Complex(-r, -i);
    }
    
    inline Complex operator-(const Complex &other) const {
        return Complex(r - other.r, i - other.i);
    }
    
    inline Complex operator*(const Complex &other) const {
        Component R = r*other.r - i*other.i;
        Component I = r*other.i + i*other.r;
        
        return Complex(R, I);
    }
    
    inline Component length(void) const {
        return ::sqrt(r*r + i*i);
    }
    
    // Principle square root
    inline Complex sqrt(void) const {
        Component L = this->length();
        
        Component R = ::sqrt((L + r) / 2.0);
        Component I = ::sqrt((L - r) / 2.0);
        
        // Sign of the imaginary part of the principle square root is the same as the sign of the original.
        // TODO: FP function that returns -1 for < 0 input, 1 otherwise? signbit() return -1 : 1 as int.
        if (i < 0)
            I = -I;
        
        return Complex(R, I);
    }
};
