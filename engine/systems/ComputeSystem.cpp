//
//  ComputeSystem.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 18/01/22.
//

#include "ComputeSystem.h"

ComputeSystem::ComputeSystem(EngineDevice& device, VkRenderPass renderPass) : device(device){
    createPipelineLayout();
    createPipeline(renderPass);
}

ComputeSystem::~ComputeSystem(){
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}


void ComputeSystem::createPipelineLayout(){
	VkDescriptorSetLayoutBinding computeSet[1];
	computeSet[0].binding = 0;
	computeSet[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	computeSet[0].descriptorCount = 1;
	computeSet[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	computeSet[0].pImmutableSamplers = (VkSampler *)nullptr;

	VkDescriptorSetLayoutCreateInfo vdslc;
	vdslc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	vdslc.pNext = nullptr;
	vdslc.flags = 0;
	vdslc.bindingCount = 1;
	vdslc.pBindings = &computeSet[0];

	if (vkCreateDescriptorSetLayout(device.device(), &vdslc, nullptr, &pipelineConfig.computeSetLayout)){
        throw std::runtime_error("failed to create descriptor set layout");
	}

    VkPushConstantRange pushConstantRange;
    pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ComputeSystemPushConstantData);
    
    // std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = nullptr;
	pipelineLayoutInfo.flags = 0;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &pipelineConfig.computeSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    
    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=  VK_SUCCESS){
        throw std::runtime_error("failed to create pipeline layout");
    }
    
}

void ComputeSystem::createPipeline(VkRenderPass renderPass){
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    // EngineComputePipeline::DefaultComputePipelineConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    
    pipeline = std::make_unique<EngineComputePipeline>(device, "shaders/simple.comp.spv", pipelineConfig);
}

void ComputeSystem::RunCompute(ComputeFrameInfo& frameInfo){
    pipeline->Bind(frameInfo.commandBuffer);
    
    vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout,
                            0,
                            1,
                            &frameInfo.globalDescriptorSet,
                            0,
                            nullptr);
    
	ComputeSystemPushConstantData push{};
    push.test={};  
	vkCmdPushConstants(frameInfo.commandBuffer,
						pipelineLayout,
						VK_SHADER_STAGE_COMPUTE_BIT,
						0,
						sizeof(ComputeSystemPushConstantData),
						&push);

	vkCmdDispatch(frameInfo.commandBuffer, 1, 1, 1);
    // for (auto& kv : frameInfo.gameObjects) {
    //     auto &obj = kv.second;
        
    //     ComputeSystemPushConstantData push{};
        
    //     vkCmdPushConstants(frameInfo.commandBuffer,
    //                        pipelineLayout,
    //                     	VK_SHADER_STAGE_COMPUTE_BIT,
    //                        0,
    //                        sizeof(ComputeSystemPushConstantData),
    //                        &push);
        
    //     obj.model->Bind(frameInfo.commandBuffer);
    //     obj.model->Draw(frameInfo.commandBuffer);
    // }
}
