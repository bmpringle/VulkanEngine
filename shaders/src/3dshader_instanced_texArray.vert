#version 450

layout(binding = 0) uniform UniformBuffer {
    mat4x4 modelMatrix;
    mat4x4 viewMatrix;
    mat4x4 projectionMatrix;
} ubo;

layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 inTexCoord;
layout(location = 3) in vec3 worldPosition;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 outTexCoord;

void main() {
    vec3 position = modelPosition + worldPosition;
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.modelMatrix * vec4(-position.x, -position.y, -position.z, 1.0);
    fragColor = color;
    outTexCoord = inTexCoord;
}