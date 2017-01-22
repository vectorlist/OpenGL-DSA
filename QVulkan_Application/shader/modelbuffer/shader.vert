#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inCoords;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragCoords;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec3 pos = vec3(inPosition.x,-inPosition.y,inPosition.z);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragCoords = inCoords;
}