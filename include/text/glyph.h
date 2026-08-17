// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
#pragma once

#include <glad/gl.h>

// Per-glyph cache entry stored in the font atlas.
// All fields are in pixel units relative to the baseline.
struct Glyph {
    // UV coordinates in the atlas texture [0, 1]
    float u0, v0;  // top-left
    float u1, v1;  // bottom-right

    // Dimensions in pixels
    int width;
    int height;

    // Offset from baseline to top-left of the bitmap
    int bearing_x;
    int bearing_y;

    // Horizontal advance to the next glyph origin (in pixels, already /64)
    int advance;
};
