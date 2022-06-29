#version 450

layout(binding = 0) uniform UniformBuffer {
    mat4x4 modelMatrix;
    mat4x4 viewMatrix;
    mat4x4 projectionMatrix;
    vec3 tint;
} ubo;

layout(location = 0) in vec2 pos;

void main() {
    gl_Position = vec4(pos.xy, 0, 1);
}