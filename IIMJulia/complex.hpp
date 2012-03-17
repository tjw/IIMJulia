//
//  complex.cpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/17/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

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
    
    inline Complex operator-(const Complex &other) const {
        return Complex(r - other.r, i - other.i);
    }
    
    inline Complex operator*(const Complex &other) const {
        Component R = r*other.r - i*other.i;
        Component I = r*other.i + i*other.r;
        
        return Complex(R, I);
    }
};
