//
//  TerrainRenderSystem.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 30/01/22.
//

#include "TerrainRenderSystem.h"


TerrainRenderSystem::TerrainRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device){
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

TerrainRenderSystem::~TerrainRenderSystem(){
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}


void TerrainRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout){
    
    VkPushConstantRange pushConstantRange;
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(TerrainPushConstantData);
    
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    
    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=  VK_SUCCESS){
        throw std::runtime_error("failed to create pipeline layout");
    }
    
}

void TerrainRenderSystem::createPipeline(VkRenderPass renderPass){
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    EnginePipeline::TessellationPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipelineConfig.meshType = MeshType::Terrain;
    
    pipeline = std::make_unique<EnginePipeline>(device,
                                                "shaders/terrain.vert.spv",
                                                "shaders/terrain.tesc.spv",
                                                "shaders/terrain.tese.spv",
                                                "shaders/terrain.frag.spv",
                                                pipelineConfig);
}


void TerrainRenderSystem::RenderGameObjects(TerrainRenderFrameInfo& frameInfo){
    pipeline->Bind(frameInfo.commandBuffer);
    
    vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout,
                            0, 1,
                            &frameInfo.globalDescriptorSet,
                            0, nullptr);
    
    for (auto& kv : frameInfo.gameObjects) {
        auto &obj = kv.second;
        if (obj.model->IsCulled) continue;
        
        TerrainPushConstantData push{};
        push.cameraPosition = frameInfo.camera.GetPosition();
        push.PVMatrix = frameInfo.camera.GetProjectionMatrix() * frameInfo.camera.GetViewMatrix(); 
        push.time = EngineTime::GetTime(); 

        vkCmdPushConstants(frameInfo.commandBuffer,
                           pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
                           0, sizeof(TerrainPushConstantData), &push);
        
        obj.model->Bind(frameInfo.commandBuffer);
        obj.model->Draw(frameInfo.commandBuffer);
    }
}
