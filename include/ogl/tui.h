#pragma once

#include <cmath>
#include <print>
#include <vector>

#include "utils/math.h"

float ylm(unsigned l, unsigned m, float theta, float /*phi*/) {
    return std::sph_legendref(l, m, theta);
    return 0.0;
}
float facratio(unsigned a, unsigned b){
    auto diff = std::max(a,b) - std::min(a,b);
    return diff;
}

float gaussian(float mu, float sigma, float x) {
    return std::exp(-0.5 * (x - mu) * (x - mu) / (sigma * sigma));
}

void sample() {
    float mu = 1;
    float sigma = 2.0;
    int nb = 5000;
    auto samples =  sample([=](float x) { return gaussian(mu, sigma, x); }, -10, 10,nb);
    for (int i = 0; i < nb; ++i) {
        std::println("{} {}", i, samples[i]);
    }
}


float Rnla(unsigned n, unsigned l, float r, float a) {
    float rho = 2 * r / (n * a);
    float fac3 = std::exp(-rho / 2.0) * std::pow(rho, l) * std::assoc_laguerref(n - l - 1, 2 * l + 1, rho);
    float coef =  std::sqrt(fct(n-l-1)/(2.0*n*fct(n+l)));
    //float coef = std::pow(rho/r, 3.0/2.0) ; //* std::sqrt(fct(n-l-1)/(2*n*fct(n+l)));
    //std::println("The coef and fac make n={}. l={}: {:.3e} x {:.3e} = {:.3e}", n,l,coef, fac3, coef*fac3);
    return coef*fac3;
}

void print_yl(){
    //unsigned n = 3; unsigned l = 2; unsigned m = 0; //float a = 5.29e-11;
    //float a = 1.0;
    auto ts = linspace(0, std::numbers::pi,100);
    std::vector<float> ylm(ts.size());
    std::vector<float> ylm2(ts.size());
    std::vector<float> ylm3(ts.size());
    for (unsigned i = 0; i < ts.size(); ++i) {
        ylm[i] = std::sph_legendref(0, 0, ts[i]);
        ylm2[i] = std::sph_legendref(1, 0, ts[i]);
        ylm3[i] = std::sph_legendref(1, 1, ts[i]);
    }
    for (unsigned i = 0; i < ts.size(); ++i) {
        std::println("{}, {}, {}, {}", ts[i], ylm[i],ylm2[i],ylm3[i]);
    }
}


void print_rnl(){
    //unsigned n = 3; unsigned l = 2; //float a = 5.29e-11;
    float a = 1.0;
    auto rs = linspace(0, 18*a,100);
    std::vector<float> rnl(rs.size());
    std::vector<float> rnl2(rs.size());
    std::vector<float> rnl3(rs.size());
    for (unsigned i = 0; i < rs.size(); ++i) {
        rnl[i] = Rnla(3, 1, rs[i], a);
        rnl2[i] = Rnla(3, 2, rs[i], a);
        rnl3[i] = Rnla(2, 1, rs[i], a);
    }
    for (unsigned i = 0; i < rs.size(); ++i) {
        std::println("{}, {}, {}, {}", rs[i]/a, rnl[i],rnl2[i],rnl3[i]);
    }
}

void test_laguerre() {
    // std::println("I am here");
    // float x = 2*r/(n*a);
    // std::assoc_laguerref(n-l-1,2*l+1,x);
    unsigned N = 100;
    auto xs = linspace(0, 5, 10);
    std::vector<float> ys(xs.size());
    for (unsigned i = 0; i < xs.size(); ++i) {
        ys[i] = std::assoc_laguerref(2, 3, xs[i]);
    }
    for (unsigned i = 0; i < N; ++i) {
        std::println("{}, {}", xs[i], ys[i]);
    }
}

void do_things() {
    //sample();
    //test_laguerre();
    print_yl();
}
