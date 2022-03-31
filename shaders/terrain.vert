#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

layout (location = 0) out vec3 vertexNormal;  

struct TerrainMaterial {
	float shininess; 
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


layout (set = 0, binding = 0) uniform GlobalUniformBuffer {
    Light lightParams;
    TerrainMaterial terrainMaterialParams;
} ubo;


layout (push_constant) uniform Push {
    mat4 PVMatrix;
    vec3 cameraPosition;
	int time; 
} push;


void main() {
	vertexNormal = normal;  
    gl_Position = vec4(position, 1.0f);
}
