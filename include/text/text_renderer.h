// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
#pragma once

#include <filesystem>
#include <memory>
#include <string_view>
#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <hb.h>

// Forward declare so scenes only need to include this header.
class FontAtlas;

// TextStyle — optional shaping hints for draw().
// For ASCII / Latin text leave all fields at their defaults;
// HarfBuzz will auto-detect via Unicode properties.
// For complex scripts (Devanagari, Arabic, Bengali, etc.) set
// script and direction explicitly to guarantee correct shaping.
struct TextStyle {
    hb_script_t    script    = HB_SCRIPT_INVALID;   // e.g. HB_SCRIPT_DEVANAGARI
    hb_direction_t direction = HB_DIRECTION_INVALID; // e.g. HB_DIRECTION_LTR / RTL
    // BCP-47 language tag: "en", "hi", "ne", "ar", etc.
    // Empty string = let HarfBuzz decide.
    const char*    language  = "";
    // Optional explicit OpenType feature list (e.g. kern, liga, akhn).
    // nullptr = use HarfBuzz defaults for the detected script.
    const hb_feature_t* features     = nullptr;
    unsigned int        feature_count = 0;
};

// TextRenderer
// ============
// Decoupled, self-contained ASCII / Unicode text overlay for MathViz scenes.
//
// Usage (inside any scene's post_draw() or render()):
//
//   // One-liner convenience:
//   text.draw("f(x) = sin(x)", 50.f, 30.f, width, height);
//
//   // Batched (single draw call for many strings):
//   text.begin(width, height);
//   text.draw("x", 400.f, 20.f);
//   text.draw("y", 20.f, 300.f, 20.f, {1,1,0,1});
//   text.flush();
//
// Coordinates: screen-space pixels, origin at top-left (matches GLFW).
// The renderer saves & restores all OpenGL state it modifies.
class TextRenderer {
public:
    // font_path : path to a TTF/OTF file (e.g. system CMU Serif)
    // pixel_height : glyph raster size in pixels (default 28px)
    explicit TextRenderer(const std::filesystem::path& font_path,
                          unsigned int pixel_height = 28);
    ~TextRenderer();

    // Non-copyable, movable.
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    TextRenderer(TextRenderer&&) noexcept;
    TextRenderer& operator=(TextRenderer&&) noexcept;

    // --- Batched API ---

    // Screen-space batch: pixel coordinates, origin top-left.
    // (x=0,y=0) is the top-left corner of the window.
    void begin(int viewport_width, int viewport_height);

    // World-space batch: pass the scene's own projection matrix.
    // Text drawn via draw() will follow the scene's pan/zoom.
    // Build the matrix with:
    //   glm::ortho(x_min, x_max, y_min, y_max)
    // where x_min/x_max/y_min/y_max are the scene's current world bounds.
    // Note: in world space Y is typically up, so text is also Y-up.
    void begin(glm::mat4 projection);

    // Queue a string at (x, y) pixels from top-left.
    // size   : render scale factor relative to the atlas pixel_height (1.0 = native).
    // color  : RGBA [0,1].
    // style  : optional shaping hints (script / direction / language).
    // Must be called between begin() and flush().
    void draw(std::string_view utf8_text,
              float x, float y,
              float size    = 1.0f,
              glm::vec4 color = {1.f, 1.f, 1.f, 1.f},
              TextStyle style = {});

    // Flush: upload vertex data and issue a single glDrawArrays call.
    void flush();

    // --- Immediate convenience (no begin/flush needed) ---

    // Screen-space: x,y in pixels, origin top-left.
    void draw_immediate(std::string_view utf8_text,
                        float x, float y,
                        int viewport_w, int viewport_h,
                        float size    = 1.0f,
                        glm::vec4 color = {1.f, 1.f, 1.f, 1.f},
                        TextStyle style = {});

    // World-space: x,y in the scene's own coordinate system.
    // projection = glm::ortho(x_min, x_max, y_min, y_max)
    void draw_world_immediate(std::string_view utf8_text,
                              float x, float y,
                              glm::mat4 projection,
                              float size    = 1.0f,
                              glm::vec4 color = {1.f, 1.f, 1.f, 1.f},
                              TextStyle style = {});

private:
    void init_gl();
    void compile_shaders();
    void push_quad(float x0, float y0, float x1, float y1,
                   float u0, float v0, float u1, float v1,
                   glm::vec4 color);

    std::unique_ptr<FontAtlas> m_atlas;

    // GL objects owned by this renderer
    GLuint m_program = 0;
    GLuint m_vao     = 0;
    GLuint m_vbo     = 0;

    // Per-frame vertex buffer (x, y, u, v, r, g, b, a) per vertex, 6 verts per glyph
    std::vector<float> m_verts;

    // Cached projection uniform location
    GLint m_loc_projection  = -1;
    GLint m_loc_font_atlas  = -1;

    // Set by begin() — either computed from viewport or supplied directly.
    glm::mat4 m_projection{1.0f};
    bool m_in_batch = false;
};
