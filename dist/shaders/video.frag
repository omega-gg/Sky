#version 440

layout(binding = 1) uniform sampler2D y;
layout(binding = 2) uniform sampler2D u;
layout(binding = 3) uniform sampler2D v;

layout(location = 0) in vec2 vector;

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf 
{
    mat4 matrix;
    mat4 matrixColor;
    float opacity;
};

void main() {
    vec4 color = vec4(texture(y, vector).r,
                      texture(u, vector).r,
                      texture(v, vector).r, 1.0);
					  
    fragColor = matrixColor * color * opacity;
}
