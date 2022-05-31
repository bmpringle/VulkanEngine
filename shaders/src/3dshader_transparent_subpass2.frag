#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragTexCoord;

layout(location = 0) out vec4 accum;
layout(location = 1) out float revealage;

layout(binding = 1) uniform sampler2DArray texSampler;

void main() {
    vec4 texColor = texture(texSampler, vec3(fragTexCoord.x, 1-fragTexCoord.y, fragTexCoord.z));

    float weight = clamp(pow(min(1.0, texColor.a * 10.0) + 0.01, 3.0) * 1e8 * 
                         pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

    // store pixel color accumulation
    accum = vec4(texColor.rgb * texColor.a, texColor.a) * weight;

    // store pixel revealage threshold
    revealage = texColor.a;
}