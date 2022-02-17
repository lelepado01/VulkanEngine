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
    alignas(16) glm::vec4 ambient;
    alignas(16) glm::vec4 diffuse;
    alignas(16) glm::vec4 color;
    alignas(16) glm::vec4 specular;
};

struct Light {
    glm::vec4 direction;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

class EngineSettings {
private:
    
public:
    inline static bool DrawWireframe = false;
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
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0.5f, 0.5f, 0.5f, 0.0f),
        glm::vec4(0.8, 0.8f, 0.8f, 0.0f),
        glm::vec4(0.5f, 0.5f, 0.5f, 0.0f),
    };
    
};

#endif /* EngineSettings_h */
