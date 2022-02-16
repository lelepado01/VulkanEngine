//
//  EngineRenderer.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 18/01/22.
//

#ifndef EngineRenderer_h
#define EngineRenderer_h

#include <stdio.h>
#include <memory>
#include <vector>
#include <stdexcept>
#include <array>
#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "EngineWindow.h"
#include "EnginePipeline.h"
#include "EngineDevice.h"
#include "EngineSwapChain.h"
#include "EngineModel.h"
#include "EngineGameObject.h"


class EngineRenderer {
private:
    EngineWindow& window;
    EngineDevice& device;
    std::unique_ptr<EngineSwapChain> swapChain;
    
    std::vector<VkCommandBuffer> commandBuffers;
    
    uint32_t currentImageIndex;
    int currentFrameIndex = 0;
    bool isFrameStarted = false;

private:
    void createCommandBuffers();
    void freeCommandBuffers();
        
    void recreateSwapChain(); 
    
public:
    EngineRenderer(EngineWindow& window, EngineDevice& device);
    ~EngineRenderer();
    
    VkCommandBuffer BeginFrame();
    void EndFrame();
    void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);
    
    bool IsFrameInProgress() const { return isFrameStarted; };
    
    VkCommandBuffer GetCurrentCommandBuffer() const {
        assert(isFrameStarted && "Cannot get command buffer if frame is not started");
        return commandBuffers[currentFrameIndex];
    };
    
    VkRenderPass GetSwapChainRenderPass() const { return swapChain->getRenderPass(); };
    int GetFrameIndex() const {
        assert(isFrameStarted && "Cannot get frame index if frame is not started");
        return currentFrameIndex;
    };
    
    float GetAspectRatio(){ return swapChain->extentAspectRatio(); };

};

#endif /* EngineRenderer_h */
