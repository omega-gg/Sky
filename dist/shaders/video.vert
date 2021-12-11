#version 440

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 texture;

layout(location = 0) out vec2 vector;

layout(std140, binding = 0) uniform buf 
{
    mat4 matrix;
    mat4 matrixColor;
    float opacity;
};

void main() 
{
    vector = texture;
	
    gl_Position = matrix * vertex;
}
