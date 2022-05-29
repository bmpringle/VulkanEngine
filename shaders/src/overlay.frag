#version 450

#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoordsFrag;
layout(location = 2) in flat uint texID;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler[];

void main() {
    outColor = texture(texSampler[texID], vec2(texCoordsFrag.x, -texCoordsFrag.y)) * vec4(fragColor.x, fragColor.y, fragColor.z, 1);
}