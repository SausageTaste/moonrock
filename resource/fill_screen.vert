#version 330 core


vec2 POSITIONS[3] = vec2[](
    vec2(-1.0,  1.0),
    vec2(-1.0, -3.0),
    vec2( 3.0,  1.0)
);

vec2 UV_COORDS[3] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.0, 2.0),
    vec2(2.0, 0.0)
);

out vec2 v_uv_coord;


void main() {
    gl_Position = vec4(POSITIONS[gl_VertexID], 0.0, 1.0);
    v_uv_coord = UV_COORDS[gl_VertexID];
}
