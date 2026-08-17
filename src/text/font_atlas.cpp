// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp

#include "text/font_atlas.h"

#include <stdexcept>
#include <format>

FontAtlas::FontAtlas(const std::filesystem::path& font_path, unsigned int pixel_height)
{
    // --- FreeType initialisation ---
    if (FT_Init_FreeType(&m_ft_lib))
        throw std::runtime_error{"FontAtlas: failed to initialise FreeType"};

    if (FT_New_Face(m_ft_lib, font_path.c_str(), 0, &m_face))
        throw std::runtime_error{
            std::format("FontAtlas: failed to load font '{}'", font_path.string())};

    // Set pixel size (height); width=0 lets FreeType choose automatically.
    FT_Set_Pixel_Sizes(m_face, 0, pixel_height);

    // --- HarfBuzz: wrap the FreeType face ---
    // hb_ft_font_create_referenced() creates the HarfBuzz font object.
    m_hb_font = hb_ft_font_create_referenced(m_face);
    if (!m_hb_font)
        throw std::runtime_error{"FontAtlas: failed to create HarfBuzz font"};

    // CRITICAL: activate FreeType font funcs so HarfBuzz reads the font's
    // OpenType GSUB / GPOS tables through FreeType's own backend.
    // Without this call HarfBuzz falls back to a dumb cmap-only lookup
    // that produces no conjuncts, no matra reordering, no kerning.
    hb_ft_font_set_funcs(m_hb_font);

    // --- Create the GL texture atlas (single channel, 8-bit) ---
    // Disable default 4-byte row alignment so 1-byte-per-pixel bitmaps upload correctly.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Allocate the full atlas up front, zero-initialised.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                 ATLAS_WIDTH, ATLAS_HEIGHT,
                 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // restore default
}

FontAtlas::~FontAtlas()
{
    if (m_texture)  glDeleteTextures(1, &m_texture);
    if (m_hb_font)  hb_font_destroy(m_hb_font);
    if (m_face)     FT_Done_Face(m_face);
    if (m_ft_lib)   FT_Done_FreeType(m_ft_lib);
}

const Glyph& FontAtlas::get_or_add(FT_UInt glyph_id)
{
    // Return from cache if already rasterized.
    auto it = m_cache.find(glyph_id);
    if (it != m_cache.end())
        return it->second;

    // Rasterize the glyph into m_face->glyph->bitmap.
    if (FT_Load_Glyph(m_face, glyph_id, FT_LOAD_RENDER))
    {
        // Insert a zeroed dummy so we don't keep trying.
        return m_cache.emplace(glyph_id, Glyph{}).first->second;
    }

    FT_GlyphSlot slot = m_face->glyph;
    const int bw = static_cast<int>(slot->bitmap.width);
    const int bh = static_cast<int>(slot->bitmap.rows);

    // --- Shelf packer ---
    // If this glyph doesn't fit in the current shelf, start a new one.
    if (m_pen_x + bw + 1 > ATLAS_WIDTH)
    {
        m_pen_x   = 1;
        m_pen_y  += m_row_height + 1;
        m_row_height = 0;
    }

    if (m_pen_y + bh + 1 > ATLAS_HEIGHT)
    {
        // Atlas is full — return empty dummy. In practice 1024×1024 @ 28px
        // holds thousands of glyphs.
        return m_cache.emplace(glyph_id, Glyph{}).first->second;
    }

    // Upload the bitmap sub-region into the atlas texture.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    m_pen_x, m_pen_y,
                    bw, bh,
                    GL_RED, GL_UNSIGNED_BYTE,
                    slot->bitmap.buffer);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Build the Glyph record.
    Glyph g;
    g.u0 = static_cast<float>(m_pen_x)      / ATLAS_WIDTH;
    g.v0 = static_cast<float>(m_pen_y)      / ATLAS_HEIGHT;
    g.u1 = static_cast<float>(m_pen_x + bw) / ATLAS_WIDTH;
    g.v1 = static_cast<float>(m_pen_y + bh) / ATLAS_HEIGHT;
    g.width     = bw;
    g.height    = bh;
    g.bearing_x = slot->bitmap_left;
    g.bearing_y = slot->bitmap_top;
    g.advance   = static_cast<int>(slot->advance.x >> 6); // 26.6 fixed → pixels

    // Advance shelf cursor.
    m_pen_x     += bw + 1;
    m_row_height = std::max(m_row_height, bh);

    return m_cache.emplace(glyph_id, g).first->second;
}
