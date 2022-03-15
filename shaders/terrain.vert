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
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct AtmosphereData {
	vec3 v3LightPos;		// The direction vector to the light source
	vec3 v3InvWavelength;	// 1 / pow(wavelength, 4) for the red, green, and blue channels
	float fCameraHeight;	// The camera's current height
	float fOuterRadius;		// The outer (atmosphere) radius
	float fInnerRadius;		// The inner (planetary) radius
	float fKrESun;			// Kr * ESun
	float fKmESun;			// Km * ESun
	float fKr4PI;			// Kr * 4 * PI
	float fKm4PI;			// Km * 4 * PI
	float fScale;			// 1 / (fOuterRadius - fInnerRadius)
	float fScaleDepth;		// The scale depth (i.e. the altitude at which the atmosphere's average density is found)
};


layout (set = 0, binding = 0) uniform GlobalUniformBuffer {
    Light lightParams;
    TerrainMaterial terrainMaterialParams;
	// vec4 frustumPlanes[6]; 
	AtmosphereData atmosphereParams; 
} ubo;


layout (push_constant) uniform Push {
    mat4 PVMatrix;
    vec3 cameraPosition;
	int time; 
} push;


float scale(float fCos) {
	float x = 1.0 - fCos;
	return ubo.atmosphereParams.fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main() {
	vertexNormal = normal;  
    gl_Position = vec4(position, 1.0f);
}
