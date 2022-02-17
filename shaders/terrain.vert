#version 450

layout (location = 0) in vec3 position;

struct TerrainMaterial {
    vec4 color;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Light {
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

layout (set = 0, binding = 0) uniform GlobalUniformBuffer {
    Light lightParams;
    TerrainMaterial terrainMaterialParams;
	// vec4 frustumPlanes[6]; 
} ubo;

layout (push_constant) uniform Push {
    mat4 PVMatrix;
    vec3 cameraPosition;
} push;


void main() {
    gl_Position = vec4(position, 1.0f);
}
