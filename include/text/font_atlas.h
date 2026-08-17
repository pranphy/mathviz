// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
#pragma once

#include <filesystem>
#include <unordered_map>

#include <glad/gl.h>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// HarfBuzz
#include <hb.h>
#include <hb-ft.h>

#include "text/glyph.h"

// FontAtlas manages:
//   - A FreeType face for rasterizing glyph bitmaps
//   - A HarfBuzz font wrapping the same face for text shaping
//   - A single GL_R8 OpenGL texture that glyphs are packed into lazily
//   - A cache mapping (FT_UInt glyph_id) -> Glyph metrics & UV coords
//
// This is an internal implementation detail of TextRenderer.
// Scenes never interact with FontAtlas directly.
class FontAtlas {
public:
    // Atlas texture dimensions (pixels).
    static constexpr int ATLAS_WIDTH  = 1024;
    static constexpr int ATLAS_HEIGHT = 1024;

    // Construct atlas from a TTF/OTF file at pixel_height pt.
    // Throws std::runtime_error on any failure.
    FontAtlas(const std::filesystem::path& font_path, unsigned int pixel_height);
    ~FontAtlas();

    // Non-copyable
    FontAtlas(const FontAtlas&) = delete;
    FontAtlas& operator=(const FontAtlas&) = delete;

    // Ensure glyph_id is rasterized and packed into the atlas.
    // Returns a reference to the cached Glyph (valid for the lifetime of FontAtlas).
    const Glyph& get_or_add(FT_UInt glyph_id);

    // The HarfBuzz font for shaping — borrowed, do not destroy.
    hb_font_t* hb_font() const { return m_hb_font; }

    // The FreeType face — needed for FT_Load_Glyph.
    FT_Face ft_face() const { return m_face; }

    // GL texture id of the atlas (GL_R8).
    GLuint texture_id() const { return m_texture; }

    int atlas_width()  const { return ATLAS_WIDTH; }
    int atlas_height() const { return ATLAS_HEIGHT; }

private:
    FT_Library m_ft_lib = nullptr;
    FT_Face    m_face   = nullptr;
    hb_font_t* m_hb_font = nullptr;

    GLuint m_texture = 0;

    std::unordered_map<FT_UInt, Glyph> m_cache;

    // Shelf packer state: current row baseline Y and cursor X
    int m_pen_x = 1;
    int m_pen_y = 1;
    int m_row_height = 0; // tallest glyph in the current shelf
};
