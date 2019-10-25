#version 330 core

in vec3 position;
in vec3 color;
in vec2 texcoord;

out vec3 fragColor;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    fragColor = color;
}
