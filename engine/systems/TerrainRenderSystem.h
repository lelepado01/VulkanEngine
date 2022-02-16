//
//  TerrainRenderSystem.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 30/01/22.
//

#ifndef TerrainRenderSystem_h
#define TerrainRenderSystem_h

#include <stdio.h>
#include <vector>
#include <memory>

#include <glm/gtx/vector_angle.hpp>

#include "../EngineDevice.h"
#include "../EngineRenderer.h"
#include "../EngineGameObject.h"
#include "../EngineCamera.h"
#include "../EngineFrameInfo.h"

struct TerrainPushConstantData {
    glm::mat4 PVMatrix{};
    glm::vec3 cameraPosition{};
};

class TerrainRenderSystem {
private:
    EngineDevice& device;
    
    std::unique_ptr<EnginePipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    
private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

public:
    TerrainRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~TerrainRenderSystem();
    
    TerrainRenderSystem( const TerrainRenderSystem& ) = delete;
    TerrainRenderSystem& operator=(const TerrainRenderSystem& ) = delete;

    void RenderGameObjects(TerrainRenderFrameInfo& frameInfo);
    
};

#endif /* TerrainRenderSystem_hpp */
