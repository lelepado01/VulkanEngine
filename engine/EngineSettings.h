//
//  EngineSettings.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 29/01/22.
//

#ifndef EngineSettings_h
#define EngineSettings_h

#include <glm/glm.hpp>

struct TerrainMaterial {
	alignas(4) float shininess;  
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 color;
    alignas(16) glm::vec3 specular;
};

struct Light {
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
};

struct AtmosphereData {
	alignas(16) glm::vec3 v3cameraDir;
	alignas(16) glm::vec3 v3LightPos;		// The direction vector to the light source
	alignas(16) glm::vec3 v3InvWavelength;	// 1 / pow(wavelength, 4) for the red, green, and blue channels
	alignas(4) float fCameraHeight;	// The camera's current height
	alignas(4) float fOuterRadius;		// The outer (atmosphere) radius
	alignas(4) float fInnerRadius;		// The inner (planetary) radius
	alignas(4) float fKrESun;			// Kr * ESun
	alignas(4) float fKmESun;			// Km * ESun
	alignas(4) float fKr4PI;			// Kr * 4 * PI
	alignas(4) float fKm4PI;			// Km * 4 * PI
	alignas(4) float fScale;			// 1 / (fOuterRadius - fInnerRadius)
	alignas(4) float fScaleDepth;		// The scale depth (i.e. the altitude at which the atmosphere's average density is found)
};

class EngineSettings {
private:
    
public:
    inline static bool DrawWireframe = true;
    inline static int MaxSubdivisions = 6;
    inline static float SphereRadius = 1000;
    inline static float CameraSpeed = 100;
    
    inline static Light LightParams {
        glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
        glm::vec4(0.5f, 0.5f, 0.5f, 0.0f),
        glm::vec4(0.5f, 0.5f, 0.5f, 0.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)
    };
    
    inline static TerrainMaterial TerrainMaterialParams {
		0.2f,
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0.5f, 0.5f, 0.5f, 0.0f),
        glm::vec4(0.8, 0.8f, 0.8f, 0.0f),
        glm::vec4(0.5f, 0.5f, 0.5f, 0.0f),
    };
    
};

#endif /* EngineSettings_h */
