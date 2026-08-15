// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

// =============================================================================
// Math Functions & Utilities for GLSL Shader Visualizations
// =============================================================================

#ifndef MATH_FUNCTIONS_GLSL
#define MATH_FUNCTIONS_GLSL

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------
const float PI           = 3.14159265358979323846;
const float TWO_PI       = 6.28318530717958647692;
const float HALF_PI      = 1.57079632679489661923;
const float INV_PI       = 0.31830988618379067154;
const float E            = 2.71828182845904523536;
const float SQRT2        = 1.41421356237309504880;
const float LN2          = 0.69314718055994530942;
const float LN10         = 2.30258509299404568402;
const float GOLDEN_RATIO = 1.61803398874989484820;

// -----------------------------------------------------------------------------
// Elementary & Special Real Functions
// -----------------------------------------------------------------------------

// Normalized / unnormalized sinc: sin(x) / x
float sinc(float x) {
    return (abs(x) < 1e-6) ? 1.0 : sin(x) / x;
}

// Derivative of sinc(x)
float dsinc(float x) {
    return (abs(x) < 1e-4) ? 0.0 : (x * cos(x) - sin(x)) / (x * x);
}

// Hyperbolic secant sech(x) = 1 / cosh(x)
float sech(float x) {
    return 1.0 / cosh(clamp(x, -20.0, 20.0));
}

// Hyperbolic cosecant csch(x) = 1 / sinh(x)
float csch(float x) {
    float s = sinh(clamp(x, -20.0, 20.0));
    return (abs(s) < 1e-6) ? 1e6 : 1.0 / s;
}

// Hyperbolic cotangent coth(x) = cosh(x) / sinh(x)
float coth(float x) {
    float s = sinh(clamp(x, -20.0, 20.0));
    return (abs(s) < 1e-6) ? 1e6 : cosh(x) / s;
}

// Gaussian Normal Distribution
float gaussian(float x, float mu, float sigma) {
    float s = max(sigma, 1e-6);
    float z = (x - mu) / s;
    return (1.0 / (s * sqrt(TWO_PI))) * exp(-0.5 * z * z);
}

// Derivative of Gaussian Normal Distribution
float dgaussian(float x, float mu, float sigma) {
    float s = max(sigma, 1e-6);
    return -((x - mu) / (s * s)) * gaussian(x, mu, s);
}

// Cauchy / Lorentzian distribution: 1 / (pi * gamma * (1 + ((x-x0)/gamma)^2))
float lorentzian(float x, float x0, float gamma) {
    float g = max(gamma, 1e-6);
    float z = (x - x0) / g;
    return 1.0 / (PI * g * (1.0 + z * z));
}

// Logistic sigmoid function: 1 / (1 + exp(-k * (x - x0)))
float sigmoid(float x, float k, float x0) {
    return 1.0 / (1.0 + exp(-clamp(k * (x - x0), -30.0, 30.0)));
}

// Softplus: ln(1 + exp(x))
float softplus(float x) {
    return (x > 20.0) ? x : log(1.0 + exp(x));
}

// Smooth step pulse between [left, right]
float smooth_pulse(float x, float left, float right, float edge) {
    return smoothstep(left - edge, left + edge, x) * (1.0 - smoothstep(right - edge, right + edge, x));
}

// -----------------------------------------------------------------------------
// Periodic Waveforms & Fourier Syntheses
// -----------------------------------------------------------------------------

// Square wave with period 2*PI
float square_wave(float x) {
    return (sin(x) >= 0.0) ? 1.0 : -1.0;
}

// Sawtooth wave with period 2*PI in range [-1, 1]
float sawtooth_wave(float x) {
    return (fract(x / TWO_PI + 0.5) - 0.5) * 2.0;
}

// Triangle wave with period 2*PI in range [-1, 1]
float triangle_wave(float x) {
    return 1.0 - 2.0 * abs(fract(x / TWO_PI + 0.5) - 0.5) * 2.0;
}

// Fourier series synthesis of square wave: sum_{k=1..N} (4/pi) * sin((2k-1)x) / (2k-1)
float fourier_square_wave(float x, uint terms, out float dfdx) {
    float val = 0.0;
    dfdx = 0.0;
    uint n_terms = min(terms, 64u);
    for (uint k = 1u; k <= n_terms; ++k) {
        float n_k = float(2u * k - 1u);
        float coeff = (4.0 * INV_PI) / n_k;
        val += coeff * sin(n_k * x);
        dfdx += coeff * n_k * cos(n_k * x);
    }
    return val;
}

// Fourier series synthesis of sawtooth wave: sum_{k=1..N} -(2/pi) * (-1)^k * sin(k*x) / k
float fourier_sawtooth_wave(float x, uint terms, out float dfdx) {
    float val = 0.0;
    dfdx = 0.0;
    uint n_terms = min(terms, 64u);
    for (uint k = 1u; k <= n_terms; ++k) {
        float k_f = float(k);
        float sign_val = ((k & 1u) == 1u) ? 1.0 : -1.0;
        float coeff = (2.0 * INV_PI * sign_val) / k_f;
        val += coeff * sin(k_f * x);
        dfdx += coeff * k_f * cos(k_f * x);
    }
    return val;
}

// Fourier series synthesis of triangle wave: sum_{k=1..N} (8/pi^2) * (-1)^(k-1) * sin((2k-1)x) / (2k-1)^2
float fourier_triangle_wave(float x, uint terms, out float dfdx) {
    float val = 0.0;
    dfdx = 0.0;
    uint n_terms = min(terms, 64u);
    float factor = 8.0 / (PI * PI);
    for (uint k = 1u; k <= n_terms; ++k) {
        float n_k = float(2u * k - 1u);
        float sign_val = ((k & 1u) == 1u) ? 1.0 : -1.0;
        float coeff = factor * sign_val / (n_k * n_k);
        val += coeff * sin(n_k * x);
        dfdx += coeff * n_k * cos(n_k * x);
    }
    return val;
}

// -----------------------------------------------------------------------------
// Complex Number Operations
// -----------------------------------------------------------------------------

vec2 c_add(vec2 a, vec2 b) { return a + b; }
vec2 c_sub(vec2 a, vec2 b) { return a - b; }

vec2 c_mul(vec2 a, vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

vec2 c_sq(vec2 z) {
    return vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y);
}

vec2 c_div(vec2 a, vec2 b) {
    float d = dot(b, b);
    return vec2(dot(a, b), a.y * b.x - a.x * b.y) / max(d, 1e-12);
}

vec2 c_inv(vec2 z) {
    float d = dot(z, z);
    return vec2(z.x, -z.y) / max(d, 1e-12);
}

vec2 c_conj(vec2 z) {
    return vec2(z.x, -z.y);
}

float c_abs(vec2 z) {
    return length(z);
}

float c_arg(vec2 z) {
    return atan(z.y, z.x);
}

vec2 c_exp(vec2 z) {
    return exp(z.x) * vec2(cos(z.y), sin(z.y));
}

vec2 c_log(vec2 z) {
    return vec2(log(max(length(z), 1e-12)), atan(z.y, z.x));
}

vec2 c_pow(vec2 z, float p) {
    float r = length(z);
    if (r < 1e-12) return vec2(0.0);
    float theta = atan(z.y, z.x);
    return pow(r, p) * vec2(cos(p * theta), sin(p * theta));
}

vec2 c_sin(vec2 z) {
    return vec2(sin(z.x) * cosh(z.y), cos(z.x) * sinh(z.y));
}

vec2 c_cos(vec2 z) {
    return vec2(cos(z.x) * cosh(z.y), -sin(z.x) * sinh(z.y));
}

// -----------------------------------------------------------------------------
// Coordinate Transformations
// -----------------------------------------------------------------------------

vec2 cartesian_to_polar(vec2 pos) {
    return vec2(length(pos), atan(pos.y, pos.x)); // vec2(r, theta)
}

vec2 polar_to_cartesian(float r, float theta) {
    return vec2(r * cos(theta), r * sin(theta));
}

// -----------------------------------------------------------------------------
// Curve & Anti-Aliasing Drawing Helpers with Optional Neon Glow
// -----------------------------------------------------------------------------

// Anti-aliased line drawing with optional neon glow for explicit curves y = f(x)
vec4 draw_explicit_curve(float y_screen, float y_func, float dfdx, vec3 color, float width_px, float px_size, float glow_intensity) {
    float d = abs(y_screen - y_func) / sqrt(1.0 + dfdx * dfdx);
    float half_w = width_px * px_size * 0.5;
    float aa_w = px_size * 1.25;
    float core_alpha = 1.0 - smoothstep(half_w - aa_w * 0.5, half_w + aa_w * 0.5, d);
    core_alpha = clamp(core_alpha, 0.0, 1.0);

    if (glow_intensity > 0.0) {
        // Multi-tier exponential neon glow bloom
        float inner_glow = exp(-d / (px_size * 4.5)) * 0.75;
        float outer_glow = exp(-d / (px_size * 16.0)) * 0.35;
        float glow_alpha = (inner_glow + outer_glow) * glow_intensity;

        // Brighten center line with white hotspot core
        vec3 lit_color = mix(color, vec3(1.0), core_alpha * 0.5);
        float total_alpha = clamp(core_alpha + glow_alpha * (1.0 - core_alpha), 0.0, 1.0);
        return vec4(lit_color, total_alpha);
    }

    return vec4(color, core_alpha);
}

vec4 draw_explicit_curve(float y_screen, float y_func, float dfdx, vec3 color, float width_px, float px_size) {
    return draw_explicit_curve(y_screen, y_func, dfdx, color, width_px, px_size, 0.0);
}

// Anti-aliased line drawing with optional neon glow for implicit curves F(x, y) = 0
vec4 draw_implicit_curve(float val, vec2 grad, vec3 color, float width_px, float px_size, float glow_intensity) {
    float grad_len = max(length(grad), 1e-6);
    float d = abs(val) / grad_len;
    float half_w = width_px * px_size * 0.5;
    float aa_w = px_size * 1.25;
    float core_alpha = 1.0 - smoothstep(half_w - aa_w * 0.5, half_w + aa_w * 0.5, d);
    core_alpha = clamp(core_alpha, 0.0, 1.0);

    if (glow_intensity > 0.0) {
        float inner_glow = exp(-d / (px_size * 4.5)) * 0.75;
        float outer_glow = exp(-d / (px_size * 16.0)) * 0.35;
        float glow_alpha = (inner_glow + outer_glow) * glow_intensity;

        vec3 lit_color = mix(color, vec3(1.0), core_alpha * 0.5);
        float total_alpha = clamp(core_alpha + glow_alpha * (1.0 - core_alpha), 0.0, 1.0);
        return vec4(lit_color, total_alpha);
    }

    return vec4(color, core_alpha);
}

vec4 draw_implicit_curve(float val, vec2 grad, vec3 color, float width_px, float px_size) {
    return draw_implicit_curve(val, grad, color, width_px, px_size, 0.0);
}

#endif // MATH_FUNCTIONS_GLSL
