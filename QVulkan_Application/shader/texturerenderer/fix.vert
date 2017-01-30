#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform uboobject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 lightPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inCoords;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragCoords;
layout(location = 2) out vec3 fragNormal;

out gl_PerVertex {
   vec4 gl_Position;
};


void main() {
    vec3 lightPos = ubo.lightPos;
    vec3 N = inNormal;
    float NdotL = dot(N, normalize(lightPos));
    vec3 color = vec3(0.7,0.7,0.75) * NdotL;
    
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragCoords = inCoords;
    fragNormal = color;
    //fragNormal = ubo.lightPos;
}