#version 450

layout(binding = 0) uniform UniformBuffer {
    mat4x4 modelMatrix;
    mat4x4 viewMatrix;
    mat4x4 projectionMatrix;
    vec3 tint;
} ubo;

layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 worldPosition;

void main() {
    //solve z-fighting
    float scale = 1.0005;
    vec3 position = modelPosition * scale + worldPosition;
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.modelMatrix * vec4(-position.x, -position.y, -position.z, 1.0);
}