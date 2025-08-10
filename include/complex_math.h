#ifndef COMPLEX_MATH_H
#define COMPLEX_MATH_H

/**
 * Complex number representation and arithmetic operations
 * Using UK spelling conventions
 */

typedef struct {
    double real;
    double imag;
} Complex;

/* Complex number operations */
Complex complex_create(double real, double imag);
Complex complex_add(Complex a, Complex b);
Complex complex_subtract(Complex a, Complex b);
Complex complex_multiply(Complex a, Complex b);
Complex complex_divide(Complex a, Complex b);
Complex complex_conjugate(Complex a);
double complex_magnitude(Complex a);
double complex_magnitude_squared(Complex a);
Complex complex_from_polar(double magnitude, double phase);

/* Utility functions */
void complex_print(Complex c);
int complex_equals(Complex a, Complex b, double tolerance);

#endif