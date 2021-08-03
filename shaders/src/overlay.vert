#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in uint texIDIn;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoordsFrag;
layout(location = 2) out flat uint texID;

layout(binding = 0) uniform UniformBuffer {
    vec2 bounds;
} ubo;

void main() {
    gl_Position = vec4(position.x / ubo.bounds.x, -position.y / ubo.bounds.y, 0.0, 1.0);
    fragColor = color;
    texCoordsFrag = texCoords;
    texID = texIDIn;
}