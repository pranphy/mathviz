// -*- coding: glsl -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl
// Text fragment shader — compiled at runtime from source (not SPIR-V).
// The font atlas is a single-channel GL_R8 texture; the red channel
// is the glyph alpha mask.

#version 460 core

in  vec2 v_uv;
in  vec4 v_color;
out vec4 frag_color;

uniform sampler2D u_font_atlas;

void main()
{
    float alpha = texture(u_font_atlas, v_uv).r;
    frag_color  = vec4(v_color.rgb, v_color.a * alpha);
}
