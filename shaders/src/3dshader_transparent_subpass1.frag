#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2DArray texSampler;

void main() {
    outColor = texture(texSampler, vec3(fragTexCoord.x, 1-fragTexCoord.y, fragTexCoord.z));
    
    if(outColor.a != 1) {
        discard;
    }
}