
#version 450

//layout (location = 0) in vec2 position;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec3 fragPosWorld;
layout (location = 2) out vec3 fragNormalWorld;

layout (set = 0, binding = 0) uniform GlobalUniformBuffer {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    
    vec4 ambientColor;
    vec3 lightPosition;
    vec4 lightColor;
    
    vec3 cameraPosition;
} ubo;

layout (push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * vec4(position, 1.0f);
    
    fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
    fragPosWorld = position;
    fragColor = color;
}
