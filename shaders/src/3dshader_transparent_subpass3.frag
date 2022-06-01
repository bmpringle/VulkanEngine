#version 450

layout (input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput accumInput;
layout (input_attachment_index = 1, set = 0, binding = 2) uniform subpassInput revealageInput;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 accum = subpassLoad(accumInput).rgba;
    float r = subpassLoad(revealageInput).r;

    outColor = vec4(accum.rgb / clamp(accum.a, 1e-4, 5e4), 1 - r);
}