#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoordsFrag;
layout(location = 2) in flat uint texID;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D [2] texSampler;

void main() {
    outColor = texture(texSampler[texID], vec2(texCoordsFrag.x, -texCoordsFrag.y));
}