//
//  Engine.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 14/01/22.
//

#ifndef Engine_h
#define Engine_h

#include <stdio.h>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "EngineWindow.h"
#include "EngineDevice.h"
#include "EngineRenderer.h"
#include "EngineGameObject.h"
#include "EngineCamera.h"
#include "EngineTime.h"
#include "EngineInput.h"
#include "EngineBuffer.h"
#include "EngineDescriptor.h"
#include "EngineSettings.h"
#include "systems/SimpleRenderSystem.h"
#include "systems/TerrainRenderSystem.h"
#include "systems/ComputeSystem.h"

struct GlobalUniformBuffer {
    Light light;
	TerrainMaterial terrainMaterial;
};


class Engine {
private:
    EngineWindow window;
    EngineDevice device{window};
    EngineRenderer renderer{window, device};
    
    std::unique_ptr<SimpleRenderSystem> simpleRenderSystem{};
    std::unique_ptr<TerrainRenderSystem> terrainRenderSystem{};
    // std::unique_ptr<ComputeSystem> computeSystem{};
    
    std::unique_ptr<EngineDescriptorPool> globalPool{}; 
    std::vector<std::unique_ptr<EngineBuffer>> globalUniformBuffers = std::vector<std::unique_ptr<EngineBuffer>>(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
    std::vector<VkDescriptorSet> globalDescriptorSets = std::vector<VkDescriptorSet>(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);

    std::vector<EngineGameObject::Map> gameObjects = std::vector<EngineGameObject::Map>(GameObjectTag::Number);

private: 
	void updateWindowTitle(); 

public:
    Engine();
    ~Engine();
    
    Engine( const Engine& ) = delete;
    Engine& operator=(const Engine& ) = delete;
        
    void Update(); 
    void Draw(EngineCamera& camera);
    void Quit();
    
    void AddGameObject(EngineGameObject& gameObject);
    EngineGameObject::Map& GetGameObjects(GameObjectTag searchedTag);
    
    std::unique_ptr<EngineModel> LoadModelFromFile(const std::string& path);
    std::unique_ptr<EngineModel> LoadModelFromVertices(const std::vector<TerrainVertex> &vertices, const std::vector<unsigned int> &indices);

    bool IsRunning() { return window.IsRunning(); };
    
    float GetWindowAspectRatio() { return renderer.GetAspectRatio(); };
    VkDevice GetDevice() { return device.device(); };
};

#endif /* Engine_hpp */
