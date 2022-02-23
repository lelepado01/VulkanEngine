//
//  EngineComputePipeline.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 14/01/22.
//

#ifndef EngineComputePipeline_h
#define EngineComputePipeline_h

#include <stdio.h>
#include <string>
#include <vector>
#include <ios>
#include <fstream>
#include <stdexcept>
#include <cassert>

#include "EngineDevice.h"
#include "EngineModel.h"
#include "EngineSettings.h"
#include "EngineVertex.h"

struct ComputePipelineConfigInfo {
    ComputePipelineConfigInfo(const ComputePipelineConfigInfo&) = delete;
    ComputePipelineConfigInfo& operator=(const ComputePipelineConfigInfo&) = delete;
    
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo; 
    
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
	VkDescriptorSetLayout computeSetLayout = nullptr;
    uint32_t subpass = 0;
};

class EngineComputePipeline {
private:
    EngineDevice& device;
    VkPipeline computePipeline;
    VkShaderModule computeShaderModule;
    
private:
    void createComputePipeline(const std::string& source, const ComputePipelineConfigInfo& configInfo);
    
    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
    
    static std::vector<char> readFile(const std::string& filename);
    
public:
    EngineComputePipeline(EngineDevice& device,
							const std::string& source,
							const ComputePipelineConfigInfo& configInfo);
    ~EngineComputePipeline();
    
    EngineComputePipeline(const EngineComputePipeline&) = delete;
    EngineComputePipeline& operator=(const EngineComputePipeline& ) = delete;
    
    void Bind(VkCommandBuffer commandBuffer); 
    
    static void DefaultComputePipelineConfigInfo(ComputePipelineConfigInfo& configInfo);
};

#endif /* EnginePipeline_h */
