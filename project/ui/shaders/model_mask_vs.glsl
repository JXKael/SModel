#version 330 core

in vec3 position;
// in vec2 texcoord;

// out vec2 uv;

void main() {
    gl_Position = vec4(position, 1.0);
    // uv = texcoord;
}
