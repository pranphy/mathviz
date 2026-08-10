// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core

layout(location = 0) in vec4 v_color_attr; // xyz = base color, w = base size
layout(location = 1) in vec3 v_view_pos;   // position in view space

layout(location = 0) out vec4 pixel_color;

void main()
{
    // Convert point coordinates [0, 1] to normal xy values [-1, 1]
    vec3 normal;
    normal.xy = gl_PointCoord * 2.0 - 1.0;
    
    // Discard pixels outside the circular boundary of the billboard
    float r2 = dot(normal.xy, normal.xy);
    if (r2 > 1.0) discard;
    
    // Calculate the Z component of the normal to form a hemisphere
    normal.z = sqrt(1.0 - r2);

    // Dynamic Phong Shading in View Space:
    // Directional light source coming from top-right-front in view space
    vec3 light_dir = normalize(vec3(1.0, 1.0, 1.2));
    float diff = clamp(dot(normal, light_dir), 0.0, 1.0);

    // Specular highlighting (Shininess)
    // View vector in view space is always along the Z-axis (0, 0, 1)
    vec3 half_vector = normalize(light_dir + vec3(0.0, 0.0, 1.0));
    float spec = pow(clamp(dot(normal, half_vector), 0.0, 1.0), 24.0);

    // Combine diffuse, specular, and ambient colors
    vec3 base_color = v_color_attr.xyz;
    vec3 final_color = base_color * (diff + 0.15) + vec3(0.55) * spec;
    //vec3 final_color = base_color;

    // Dark blue space depth fog
    float depth = -v_view_pos.z*0.05;;
    //float depth = 0.0;
    final_color = mix(final_color, vec3(0.02, 0.03, 0.05), 1.0 - exp(-0.02 * depth * depth));

    pixel_color = vec4(final_color, 1.0);
}
