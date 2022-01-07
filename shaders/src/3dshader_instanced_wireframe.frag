#version 450

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2DArray texSampler;

void main() {
    outColor = vec4(0, 0, 0, 1);
}