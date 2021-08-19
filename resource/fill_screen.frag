#version 330 core


out vec4 f_color;

in vec2 v_uv_coord;

uniform sampler2D u_image;


void main() {
    f_color = texture(u_image, v_uv_coord);
}
