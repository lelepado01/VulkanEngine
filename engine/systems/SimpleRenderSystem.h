//
//  SimpleRenderSystem.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 18/01/22.
//

#ifndef SimpleRenderSystem_h
#define SimpleRenderSystem_h

#include <stdio.h>
#include <vector>
#include <memory>

#include "../EngineDevice.h"
#include "../EngineRenderer.h"
#include "../EngineGameObject.h"
#include "../EngineCamera.h"
#include "../EngineFrameInfo.h"

struct SimplePushConstantData {
    glm::mat4 modelMatrix{1.0f};
    glm::mat4 normalMatrix{1.0f};
};

class SimpleRenderSystem {
private:
    EngineDevice&  device;
    
    std::unique_ptr<EnginePipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    
private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

public:
    SimpleRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();
    
    SimpleRenderSystem( const SimpleRenderSystem& ) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem& ) = delete;

    void RenderGameObjects(FrameInfo& frameInfo);
    
};


#endif /* SimpleRenderSystem_h */
