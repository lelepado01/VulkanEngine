//
//  ComputeSystem.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 18/01/22.
//

#ifndef ComputeSystem_h
#define ComputeSystem_h

#include <stdio.h>
#include <vector>
#include <memory>

#include "../EngineDevice.h"
#include "../EngineRenderer.h"
#include "../EngineGameObject.h"
#include "../EngineCamera.h"
#include "../EngineFrameInfo.h"
#include "../EngineComputePipeline.h"

struct ComputeSystemPushConstantData {
	glm::vec4 test;
};

class ComputeSystem {
private:
    EngineDevice&  device;
    
    std::unique_ptr<EngineComputePipeline> pipeline;
    VkPipelineLayout pipelineLayout;
	ComputePipelineConfigInfo pipelineConfig{};

    
private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

public:
    ComputeSystem(EngineDevice& device, VkRenderPass renderPass);
    ~ComputeSystem();
    
    ComputeSystem( const ComputeSystem& ) = delete;
    ComputeSystem& operator=(const ComputeSystem& ) = delete;

    void RunCompute(ComputeFrameInfo& frameInfo);
    
};


#endif /* ComputeSystem_h */
