// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core

layout(location = 0) in vec4 a_position; // xyz = position, w = type/attribute (-1=proton, -2=neutron, [0,1]=electron attribute)

layout(location = 0) uniform float rect_width;
layout(location = 1) uniform float rect_height;
layout(location = 2) uniform vec2 area_w; // Camera orbit angle X
layout(location = 3) uniform vec2 area_h; // Camera orbit angle Y

layout(location = 10) uniform float u_proton_radius;
layout(location = 11) uniform float u_neutron_radius;
layout(location = 12) uniform float u_electron_radius;

layout(location = 0) out vec4 v_color_attr; // xyz = base color, w = physical radius
layout(location = 1) out vec3 v_view_pos;   // 3D position in camera/view space

// Orbit rotation helpers
vec3 rotateY(vec3 p, float a) {
    float c = cos(a), s = sin(a);
    return vec3(c * p.x - s * p.z, p.y, s * p.x + c * p.z);
}

vec3 rotateX(vec3 p, float a) {
    float c = cos(a), s = sin(a);
    return vec3(p.x, c * p.y - s * p.z, s * p.y + c * p.z);
}

void main()
{
    // Decode shape type/attributes to assign colors and base particle sizes
    vec3 base_color;
    float r_val;
    
    if (a_position.w == -1.0) {
        // Proton (Red)
        base_color = vec3(0.95, 0.2, 0.25);
        r_val = u_proton_radius;
    } else if (a_position.w == -2.0) {
        // Neutron (Blue)
        base_color = vec3(0.2, 0.45, 0.95);
        r_val = u_neutron_radius;
    } else if (a_position.w == -3.0) {
        // Neutron (Blue)
        base_color = vec3(0.8, 0.85, 0.95);
        r_val = u_neutron_radius;
    } else {
        // Electron (Dynamic gradient: Cyan to Magenta based on w attribute)
        base_color = mix(vec3(0.1, 0.9, 0.6), vec3(0.9, 0.2, 0.8), a_position.w);
        r_val = u_electron_radius;
    }

    v_color_attr = vec4(base_color, r_val);

    // Camera orbital navigation and zoom
    float angle_x = (area_w.x + area_w.y) * 0.5;
    float angle_y = (area_h.x + area_h.y) * 0.5;
    
    float zoom_scale = (area_w.y - area_w.x) / 3.0; 
    float dist = max(zoom_scale * 2.5, 1.0);

    // Transform particle coordinates to view space (camera space)
    vec3 pos = a_position.xyz;
    pos = rotateY(pos, -angle_x);
    pos = rotateX(pos, -angle_y);
    pos.z -= dist; // Move camera away

    v_view_pos = pos;

    // Perspective projection calculations
    float aspect = rect_width / rect_height;
    float fov = 1.0 / tan(45.0 * 0.5 * 3.14159265 / 180.0); // FOV = 45 degrees

    gl_Position = vec4(
        pos.x * fov / aspect,
        pos.y * fov,
        pos.z * (15.0 + 0.1) / (0.1 - 15.0) + (2.0 * 15.0 * 0.1 / (0.1 - 15.0)),
        -pos.z
    );

    // Scale particle point size mathematically based on perspective distance and viewport height
    gl_PointSize = (r_val * fov * rect_height) / (-pos.z);
}
