//
//  EnginePipeline.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 14/01/22.
//

#ifndef EnginePipeline_h
#define EnginePipeline_h

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

enum MeshType{
    Terrain,
    Model
};

struct PipelineConfigInfo {
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
    
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
    uint32_t subpass = 0;
    
    MeshType meshType;
};

class EnginePipeline {
private:
    EngineDevice& device;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule tessControlShaderModule;
    VkShaderModule tessEvalShaderModule;
    VkShaderModule fragShaderModule;
public:
    bool HasTessellationPhase = false; 
    
private:
    void createGraphicsPipeline(const std::string& vertSource, const std::string& fragSource, const PipelineConfigInfo& configInfo);
    void createGraphicsPipeline(const std::string &vertSource,
                                const std::string &tessControlSource,
                                const std::string &tessEvalSource,
                                const std::string &fragSource,
                                const PipelineConfigInfo& configInfo);
    
    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
    
    static std::vector<char> readFile(const std::string& filename);
    
public:
    EnginePipeline(EngineDevice& device,
                   const std::string& vertSource,
                   const std::string& fragSource,
                   const PipelineConfigInfo& configInfo);
    EnginePipeline(EngineDevice& device,
                   const std::string& vertSource,
                   const std::string& tessControlSource,
                   const std::string& tessEvalSource,
                   const std::string& fragSource,
                   const PipelineConfigInfo& configInfo);
    ~EnginePipeline();
    
    EnginePipeline(const EnginePipeline&) = delete;
    EnginePipeline& operator=(const EnginePipeline& ) = delete;
    
    void Bind(VkCommandBuffer commandBuffer); 
    
    static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
    static void TessellationPipelineConfigInfo(PipelineConfigInfo& configInfo);
};

#endif /* EnginePipeline_h */
