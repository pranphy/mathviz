// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp

#include "text/text_renderer.h"
#include "text/font_atlas.h"

#include <stdexcept>
#include <string>
#include <utility>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ---------------------------------------------------------------------------
// Shader source (compiled at runtime so this module is self-contained).
// ---------------------------------------------------------------------------
static const char* VERT_SRC = R"GLSL(
#version 460 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec4 a_color;
out vec2 v_uv;
out vec4 v_color;
uniform mat4 u_projection;
void main() {
    gl_Position = u_projection * vec4(a_pos, 0.0, 1.0);
    v_uv    = a_uv;
    v_color = a_color;
}
)GLSL";

static const char* FRAG_SRC = R"GLSL(
#version 460 core
in  vec2 v_uv;
in  vec4 v_color;
out vec4 frag_color;
uniform sampler2D u_font_atlas;
void main() {
    float alpha = texture(u_font_atlas, v_uv).r;
    frag_color  = vec4(v_color.rgb, v_color.a * alpha);
}
)GLSL";

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
namespace {

GLuint compile_shader(GLenum type, const char* src)
{
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    GLint ok = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(id, sizeof(log), nullptr, log);
        glDeleteShader(id);
        throw std::runtime_error{std::string{"TextRenderer shader compile failed:\n"} + log};
    }
    return id;
}

GLuint link_program(GLuint vert, GLuint frag)
{
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    GLint ok = GL_FALSE;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        glDeleteProgram(prog);
        throw std::runtime_error{std::string{"TextRenderer program link failed:\n"} + log};
    }
    glDetachShader(prog, vert);
    glDetachShader(prog, frag);
    return prog;
}

} // namespace

// ---------------------------------------------------------------------------
// TextRenderer
// ---------------------------------------------------------------------------
TextRenderer::TextRenderer(const std::filesystem::path& font_path,
                           unsigned int pixel_height)
    : m_atlas(std::make_unique<FontAtlas>(font_path, pixel_height))
{
    init_gl();
}

TextRenderer::~TextRenderer()
{
    if (m_vbo)     glDeleteBuffers(1, &m_vbo);
    if (m_vao)     glDeleteVertexArrays(1, &m_vao);
    if (m_program) glDeleteProgram(m_program);
}

TextRenderer::TextRenderer(TextRenderer&&) noexcept = default;
TextRenderer& TextRenderer::operator=(TextRenderer&&) noexcept = default;

void TextRenderer::init_gl()
{
    compile_shaders();

    // Dynamic VBO: (x, y, u, v, r, g, b, a) = 8 floats per vertex
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    constexpr GLsizei stride = 8 * sizeof(float);
    // a_pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(0));
    // a_uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(2 * sizeof(float)));
    // a_color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(4 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::compile_shaders()
{
    GLuint vert = compile_shader(GL_VERTEX_SHADER,   VERT_SRC);
    GLuint frag = compile_shader(GL_FRAGMENT_SHADER, FRAG_SRC);
    m_program   = link_program(vert, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    m_loc_projection = glGetUniformLocation(m_program, "u_projection");
    m_loc_font_atlas = glGetUniformLocation(m_program, "u_font_atlas");
}

// ---------------------------------------------------------------------------
// Script-run segmentation helpers
// ---------------------------------------------------------------------------
namespace {

// Returns the HarfBuzz script for a given Unicode codepoint.
// Uses hb_unicode_script() which reads the Unicode character database.
hb_script_t script_of(char32_t cp)
{
    return hb_unicode_script(hb_unicode_funcs_get_default(),
                             static_cast<hb_codepoint_t>(cp));
}

// Returns true for scripts that should inherit the previous run's script
// rather than start a new run (common/inherited characters like spaces,
// punctuation, diacritics).
bool is_common_or_inherited(hb_script_t s)
{
    return s == HB_SCRIPT_COMMON || s == HB_SCRIPT_INHERITED;
}

struct ScriptRun {
    std::string_view text;     // sub-view into the original UTF-8 string
    hb_script_t      script;
    hb_direction_t   direction;
};

// Splits a UTF-8 string into runs of identical HarfBuzz script.
// Common / Inherited codepoints adopt the script of the surrounding run.
std::vector<ScriptRun> segment_by_script(std::string_view utf8)
{
    std::vector<ScriptRun> runs;
    if (utf8.empty()) return runs;

    const char* p   = utf8.data();
    const char* end = utf8.data() + utf8.size();

    hb_script_t    cur_script = HB_SCRIPT_INVALID;
    const char*    run_start  = p;

    // Decode UTF-8 codepoints one at a time.
    // We track byte offsets so we can slice std::string_view sub-views.
    auto decode_one = [](const char*& ptr, const char* lim) -> char32_t {
        if (ptr >= lim) return 0;
        unsigned char c = static_cast<unsigned char>(*ptr);
        char32_t cp;
        int bytes;
        if      (c < 0x80)            { cp = c;          bytes = 1; }
        else if ((c & 0xE0) == 0xC0)  { cp = c & 0x1F;   bytes = 2; }
        else if ((c & 0xF0) == 0xE0)  { cp = c & 0x0F;   bytes = 3; }
        else                          { cp = c & 0x07;   bytes = 4; }
        for (int i = 1; i < bytes && ptr + i < lim; ++i)
            cp = (cp << 6) | (static_cast<unsigned char>(ptr[i]) & 0x3F);
        ptr += bytes;
        return cp;
    };

    while (p < end) {
        const char* cp_start = p;
        char32_t    cp       = decode_one(p, end);
        hb_script_t s        = script_of(cp);

        if (is_common_or_inherited(s))
            s = (cur_script != HB_SCRIPT_INVALID) ? cur_script : HB_SCRIPT_LATIN;

        if (s != cur_script) {
            // Flush the previous run.
            if (cur_script != HB_SCRIPT_INVALID && cp_start > run_start) {
                ScriptRun r;
                r.text      = std::string_view(run_start,
                                   static_cast<size_t>(cp_start - run_start));
                r.script    = cur_script;
                r.direction = hb_script_get_horizontal_direction(cur_script);
                runs.push_back(r);
            }
            cur_script = s;
            run_start  = cp_start;
        }
    }

    // Flush the final run.
    if (cur_script != HB_SCRIPT_INVALID && end > run_start) {
        ScriptRun r;
        r.text      = std::string_view(run_start,
                           static_cast<size_t>(end - run_start));
        r.script    = cur_script;
        r.direction = hb_script_get_horizontal_direction(cur_script);
        runs.push_back(r);
    }

    return runs;
}

// Shape a single script run and return the shaped glyphs + positions.
void shape_run(hb_font_t* font,
               const ScriptRun& run,
               const TextStyle& style,
               hb_buffer_t*& buf)
{
    hb_buffer_reset(buf);
    hb_buffer_add_utf8(buf,
                       run.text.data(),
                       static_cast<int>(run.text.size()),
                       0, -1);

    // Use caller-supplied style if valid, else fall back to per-run detection.
    hb_script_t    sc  = (style.script    != HB_SCRIPT_INVALID)   ? style.script    : run.script;
    hb_direction_t dir = (style.direction != HB_DIRECTION_INVALID) ? style.direction : run.direction;

    hb_buffer_set_script(buf, sc);
    hb_buffer_set_direction(buf, dir);

    if (style.language && style.language[0] != '\0')
        hb_buffer_set_language(buf, hb_language_from_string(style.language, -1));
    else
        hb_buffer_set_language(buf, hb_language_get_default());

    hb_shape(font, buf,
             style.features, style.feature_count);
}

} // namespace

// ---------------------------------------------------------------------------
// Batched API
// ---------------------------------------------------------------------------
void TextRenderer::begin(int viewport_width, int viewport_height)
{
    // Screen-space: origin top-left, Y increases downward (matches GLFW pixels).
    m_projection = glm::ortho(0.0f, static_cast<float>(viewport_width),
                              static_cast<float>(viewport_height), 0.0f,
                              -1.0f, 1.0f);
    m_in_batch = true;
    m_verts.clear();
}

void TextRenderer::begin(glm::mat4 projection)
{
    // World-space: the caller supplies the projection directly.
    // Typically: glm::ortho(x_min, x_max, y_min, y_max)
    // With Y-up convention (y_min < y_max), text origin is bottom-left.
    m_projection = projection;
    m_in_batch   = true;
    m_verts.clear();
}

void TextRenderer::draw(std::string_view utf8_text,
                        float x, float y,
                        float size,
                        glm::vec4 color,
                        TextStyle style)
{
    if (utf8_text.empty()) return;

    // If the caller supplied explicit script/direction they want the whole
    // string treated as a single run (e.g. a pure-Devanagari label).
    // Otherwise segment by Unicode script for mixed-script strings.
    std::vector<ScriptRun> runs;
    if (style.script != HB_SCRIPT_INVALID) {
        // Single forced run.
        ScriptRun r;
        r.text      = utf8_text;
        r.script    = style.script;
        r.direction = (style.direction != HB_DIRECTION_INVALID)
                          ? style.direction
                          : hb_script_get_horizontal_direction(style.script);
        runs.push_back(r);
    } else {
        runs = segment_by_script(utf8_text);
    }

    float cursor_x = x;
    float cursor_y = y;

    hb_buffer_t* buf = hb_buffer_create();

    for (const auto& run : runs)
    {
        shape_run(m_atlas->hb_font(), run, style, buf);

        unsigned int glyph_count = 0;
        hb_glyph_info_t*     info = hb_buffer_get_glyph_infos(buf, &glyph_count);
        hb_glyph_position_t* pos  = hb_buffer_get_glyph_positions(buf, &glyph_count);

        for (unsigned int i = 0; i < glyph_count; ++i)
        {
            FT_UInt glyph_id = info[i].codepoint; // post-shaping: glyph ID, not codepoint
            const Glyph& g = m_atlas->get_or_add(glyph_id);

            // HarfBuzz positions: 26.6 fixed-point (divide by 64 → pixels)
            float ox = pos[i].x_offset  / 64.0f;
            float oy = pos[i].y_offset  / 64.0f;
            float ax = pos[i].x_advance / 64.0f;
            float ay = pos[i].y_advance / 64.0f;

            if (g.width > 0 && g.height > 0)
            {
                float x0 = cursor_x + ox + static_cast<float>(g.bearing_x) * size;
                float y0 = cursor_y + oy - static_cast<float>(g.bearing_y) * size;
                float x1 = x0 + static_cast<float>(g.width)  * size;
                float y1 = y0 + static_cast<float>(g.height) * size;

                push_quad(x0, y0, x1, y1, g.u0, g.v0, g.u1, g.v1, color);
            }

            cursor_x += ax * size;
            cursor_y += ay * size;
        }
    }

    hb_buffer_destroy(buf);
}

void TextRenderer::flush()
{
    if (m_verts.empty()) {
        m_in_batch = false;
        return;
    }

    // --- Save relevant GL state ---
    GLint prev_program = 0, prev_vao = 0, prev_vbo = 0, prev_tex = 0;
    GLboolean prev_blend = glIsEnabled(GL_BLEND);
    GLint prev_blend_src = 0, prev_blend_dst = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM,       &prev_program);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING,  &prev_vao);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING,  &prev_vbo);
    glGetIntegerv(GL_TEXTURE_BINDING_2D,    &prev_tex);
    glGetIntegerv(GL_BLEND_SRC_ALPHA,       &prev_blend_src);
    glGetIntegerv(GL_BLEND_DST_ALPHA,       &prev_blend_dst);

    // --- Set up text GL state ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(m_program);

    // Orthographic projection set by begin()
    glUniformMatrix4fv(m_loc_projection, 1, GL_FALSE, glm::value_ptr(m_projection));

    // Bind font atlas to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_atlas->texture_id());
    glUniform1i(m_loc_font_atlas, 0);

    // Upload vertex data and draw
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(m_verts.size() * sizeof(float)),
                 m_verts.data(),
                 GL_DYNAMIC_DRAW);

    GLsizei vertex_count = static_cast<GLsizei>(m_verts.size() / 8);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);

    // --- Restore GL state ---
    glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(prev_vbo));
    glBindVertexArray(static_cast<GLuint>(prev_vao));
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(prev_tex));
    glUseProgram(static_cast<GLuint>(prev_program));
    if (!prev_blend) glDisable(GL_BLEND);
    glBlendFunc(static_cast<GLenum>(prev_blend_src),
                static_cast<GLenum>(prev_blend_dst));

    m_verts.clear();
    m_in_batch = false;
}

// ---------------------------------------------------------------------------
// Convenience
// ---------------------------------------------------------------------------
void TextRenderer::draw_immediate(std::string_view utf8_text,
                                  float x, float y,
                                  int viewport_w, int viewport_h,
                                  float size,
                                  glm::vec4 color,
                                  TextStyle style)
{
    begin(viewport_w, viewport_h);
    draw(utf8_text, x, y, size, color, style);
    flush();
}

void TextRenderer::draw_world_immediate(std::string_view utf8_text,
                                        float x, float y,
                                        glm::mat4 projection,
                                        float size,
                                        glm::vec4 color,
                                        TextStyle style)
{
    begin(projection);
    draw(utf8_text, x, y, size, color, style);
    flush();
}

// ---------------------------------------------------------------------------
// Internal: push one glyph quad (2 triangles = 6 vertices)
// ---------------------------------------------------------------------------
void TextRenderer::push_quad(float x0, float y0, float x1, float y1,
                             float u0, float v0, float u1, float v1,
                             glm::vec4 c)
{
    // Triangle 1: top-left, bottom-left, bottom-right
    // Triangle 2: top-left, bottom-right, top-right
    // Each vertex: x, y, u, v, r, g, b, a

    auto push = [&](float x, float y, float u, float v) {
        m_verts.insert(m_verts.end(), {x, y, u, v, c.r, c.g, c.b, c.a});
    };

    push(x0, y0, u0, v0);
    push(x0, y1, u0, v1);
    push(x1, y1, u1, v1);

    push(x0, y0, u0, v0);
    push(x1, y1, u1, v1);
    push(x1, y0, u1, v0);
}
