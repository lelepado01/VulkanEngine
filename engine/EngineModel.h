//
//  EngineModel.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 16/01/22.
//

#pragma once

#ifndef EngineModel_h
#define EngineModel_h

#include <stdio.h>
#include <vector>
#include <cassert>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "EngineUtils.h"
#include "EngineDevice.h"
#include "EngineBuffer.h"
#include "EngineVertex.h"

class EngineModel {
public:
    bool IsCulled = false;
    
private:
    EngineDevice& device;
    
    bool hasIndexBuffer = false;
    
    std::unique_ptr<EngineBuffer> vertexBuffer;
    uint32_t vertexCount;
    
    std::unique_ptr<EngineBuffer> indexBuffer;
    uint32_t indexCount;
    
public:

    struct Builder {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};
        
        void LoadModel(const std::string& modelPath);
    };
    
    struct TerrainBuilder {
        std::vector<TerrainVertex> vertices{};
        std::vector<uint32_t> indices{};
        
        void LoadModel(const std::vector<TerrainVertex> &vertices, const std::vector<unsigned int> &indices);
    };
    
private:
    void createVertexBuffer(const std::vector<Vertex>& vertices);
    void createVertexBuffer(const std::vector<TerrainVertex>& vertices);
    void createIndexBuffer(const std::vector<uint32_t>& indices);

public:
    EngineModel(EngineDevice& device, const Builder& builder);
    EngineModel(EngineDevice& device, const TerrainBuilder& builder);
    ~EngineModel();
    
    EngineModel(const EngineModel&) = delete;
    EngineModel& operator=(const EngineModel&) = delete;
    
    void Bind(VkCommandBuffer commandBuffer);
    void Draw(VkCommandBuffer commandBuffer);
    
    static std::unique_ptr<EngineModel> createModelFromFile(EngineDevice& device, const std::string& modelPath);
    static std::unique_ptr<EngineModel> createModelFromVertices(EngineDevice& device, const std::vector<TerrainVertex> &vertices, const std::vector<unsigned int> &indices);
};

#endif /* EngineModel_h */
