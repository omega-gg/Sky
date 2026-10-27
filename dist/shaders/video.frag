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
    mat4 matrixAdjust;
    float opacity;
    float gamma;
};

void main() {
    vec4 color = matrixAdjust * vec4(texture(y, vector).r,
                                     texture(u, vector).r,
                                     texture(v, vector).r, 1.0);

    if (gamma != 1.0) color.x = pow(clamp(color.x, 0.0, 1.0), gamma);

    fragColor = matrixColor * color * opacity;
}
