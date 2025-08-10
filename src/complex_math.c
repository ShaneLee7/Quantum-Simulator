#include "complex_math.h"
#include <math.h>
#include <stdio.h>

Complex complex_create(double real, double imag) {
    Complex c = {real, imag};
    return c;
}

Complex complex_add(Complex a, Complex b) {
    return complex_create(a.real + b.real, a.imag + b.imag);
}

Complex complex_subtract(Complex a, Complex b) {
    return complex_create(a.real - b.real, a.imag - b.imag);
}

Complex complex_multiply(Complex a, Complex b) {
    double real = a.real * b.real - a.imag * b.imag;
    double imag = a.real * b.imag + a.imag * b.real;
    return complex_create(real, imag);
}

Complex complex_divide(Complex a, Complex b) {
    double denominator = b.real * b.real + b.imag * b.imag;
    double real = (a.real * b.real + a.imag * b.imag) / denominator;
    double imag = (a.imag * b.real - a.real * b.imag) / denominator;
    return complex_create(real, imag);
}

Complex complex_conjugate(Complex a) {
    return complex_create(a.real, -a.imag);
}

double complex_magnitude(Complex a) {
    return sqrt(a.real * a.real + a.imag * a.imag);
}

double complex_magnitude_squared(Complex a) {
    return a.real * a.real + a.imag * a.imag;
}

Complex complex_from_polar(double magnitude, double phase) {
    return complex_create(magnitude * cos(phase), magnitude * sin(phase));
}

void complex_print(Complex c) {
    if (c.imag >= 0) {
        printf("%.6f + %.6fi", c.real, c.imag);
    } else {
        printf("%.6f - %.6fi", c.real, -c.imag);
    }
}

int complex_equals(Complex a, Complex b, double tolerance) {
    return (fabs(a.real - b.real) < tolerance) && (fabs(a.imag - b.imag) < tolerance);
}