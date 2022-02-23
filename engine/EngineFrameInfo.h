//
//  EngineFrameInfo.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 21/01/22.
//

#ifndef EngineFrameInfo_h
#define EngineFrameInfo_h

#include "EngineCamera.h"
#include "EngineGameObject.h"

struct FrameInfo {
    int frameInfo;
    float frameTime;
    VkCommandBuffer commandBuffer;
    EngineCamera& camera;
    VkDescriptorSet globalDescriptorSet;
    EngineGameObject::Map &gameObjects; 
};


struct TerrainRenderFrameInfo {
    int frameInfo;
    float frameTime;
    VkCommandBuffer commandBuffer;
    EngineCamera& camera;
    VkDescriptorSet globalDescriptorSet;
    EngineGameObject::Map &gameObjects; 
};


struct ComputeFrameInfo {
    int frameInfo;
    float frameTime;
    VkCommandBuffer commandBuffer;
    EngineCamera& camera;
    VkDescriptorSet globalDescriptorSet;
    EngineGameObject::Map &gameObjects; 
};


#endif /* EngineFrameInfo_h */
