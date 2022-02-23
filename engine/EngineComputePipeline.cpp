//
//  EnginePipeline.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 14/01/22.
//

#include "EngineComputePipeline.h"

EngineComputePipeline::EngineComputePipeline(EngineDevice& device,
											const std::string& source,
											const ComputePipelineConfigInfo& configInfo) : device(device){
    createComputePipeline(source, configInfo);
}

EngineComputePipeline::~EngineComputePipeline(){
    vkDestroyShaderModule(device.device(), computeShaderModule, nullptr);
    vkDestroyPipeline(device.device(), computePipeline, nullptr);
}

void EngineComputePipeline::createComputePipeline(const std::string &source, const ComputePipelineConfigInfo& configInfo){
    assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: pipelineLayout invalid");

    auto computeCode = readFile(source);
    
    createShaderModule(computeCode, &computeShaderModule);
    
	VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = computeShaderModule;
    computeShaderStageInfo.pName = "main";

	VkComputePipelineCreateInfo info {};
	info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO; 
	info.stage = computeShaderStageInfo; 
	info.layout = configInfo.pipelineLayout;

    if (vkCreateComputePipelines(device.device(), VK_NULL_HANDLE, 1, &info, nullptr, &computePipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline!");
    }
}


void EngineComputePipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule){
    VkShaderModuleCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
    if (vkCreateShaderModule(device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS){
        throw std::runtime_error("failed to create shader module");
    }
}

std::vector<char> EngineComputePipeline::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

void EngineComputePipeline::DefaultComputePipelineConfigInfo(ComputePipelineConfigInfo& configInfo){
  	// VkDescriptorSetLayoutBinding ComputeSet[1];
	// ComputeSet[0].binding = 0;
	// ComputeSet[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	// ComputeSet[0].descriptorCount = 1;
	// ComputeSet[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	// ComputeSet[0].pImmutableSamplers = (VkSampler *)nullptr;

	// VkDescriptorSetLayoutCreateInfo vdslc;
	// vdslc0.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	// vdslc0.pNext = nullptr;
	// vdslc0.flags = 0;
	// vdslc0.bindingCount = 1;
	// vdslc0.pBindings = &ComputeSet[0];

	// vkCreateDescriptorSetLayout(device.device(), IN &vdslc, nullptr, OUT &configInfo.computeSetLayout);

}

void EngineComputePipeline::Bind(VkCommandBuffer commandBuffer){
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline); 
}
