#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragCoords;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 toColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = texture(texSampler,fragCoords);
    outColor = vec4(toColor,1);
}