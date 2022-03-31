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
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
};

class EngineSettings {
private:
    
public:
    inline static bool DrawWireframe = false;
    inline static int MaxSubdivisions = 6;
    inline static float SphereRadius = 1000;
    inline static float CameraSpeed = 100;
    
    inline static Light LightParams {
        glm::vec3(4000.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    };
    
    inline static TerrainMaterial TerrainMaterialParams {
		0.2f,
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.8, 0.8f, 0.8f),
        glm::vec3(0.5f, 0.5f, 0.5f),
    };
    
};

#endif /* EngineSettings_h */
