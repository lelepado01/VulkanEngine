//
//  EngineModel.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 16/01/22.
//

#include "EngineModel.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

EngineModel::EngineModel(EngineDevice& device, const Builder& builder) : device(device){
    createVertexBuffer(builder.vertices);
    createIndexBuffer(builder.indices);
}

EngineModel::EngineModel(EngineDevice& device, const TerrainBuilder& builder) : device(device){
    createVertexBuffer(builder.vertices);
    createIndexBuffer(builder.indices);
}

EngineModel::~EngineModel(){}

void EngineModel::createVertexBuffer(const std::vector<Vertex>& vertices){
    vertexCount = static_cast<uint32_t>(vertices.size());
      assert(vertexCount >= 3 && "Vertex count must be at least 3");
      VkDeviceSize bufferSize = sizeof(Vertex) * vertexCount;
      uint32_t vertexSize = sizeof(Vertex);

      EngineBuffer stagingBuffer{
          device,
          vertexSize,
          vertexCount,
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)vertices.data());

    vertexBuffer = std::make_unique<EngineBuffer>(
          device,
          vertexSize,
          vertexCount,
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void EngineModel::createVertexBuffer(const std::vector<TerrainVertex>& vertices){
    vertexCount = static_cast<uint32_t>(vertices.size());
      assert(vertexCount >= 3 && "Vertex count must be at least 3");
      VkDeviceSize bufferSize = sizeof(TerrainVertex) * vertexCount;
      uint32_t vertexSize = sizeof(TerrainVertex);

      EngineBuffer stagingBuffer{
          device,
          vertexSize,
          vertexCount,
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)vertices.data());

    vertexBuffer = std::make_unique<EngineBuffer>(
          device,
          vertexSize,
          vertexCount,
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void EngineModel::createIndexBuffer(const std::vector<uint32_t>& indices){
    indexCount = static_cast<uint32_t>(indices.size());
      hasIndexBuffer = indexCount > 0;

      if (!hasIndexBuffer) {
        return;
      }

      VkDeviceSize bufferSize = sizeof(uint32_t) * indexCount;
      uint32_t indexSize = sizeof(uint32_t);

      EngineBuffer stagingBuffer{
          device,
          indexSize,
          indexCount,
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      };

      stagingBuffer.map();
      stagingBuffer.writeToBuffer((void *)indices.data());

      indexBuffer = std::make_unique<EngineBuffer>(
          device,
          indexSize,
          indexCount,
          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

      device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}


void EngineModel::Draw(VkCommandBuffer commandBuffer){    
    if (hasIndexBuffer){
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    } else {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
}

void EngineModel::Bind(VkCommandBuffer commandBuffer){
    VkBuffer buffers[] = {vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    
    if (hasIndexBuffer) {
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
}

std::unique_ptr<EngineModel> EngineModel::createModelFromFile(EngineDevice &device, const std::string &modelPath){
    Builder builder{};
    builder.LoadModel(modelPath);
    
    return std::make_unique<EngineModel>(device, builder); 
}

std::unique_ptr<EngineModel> EngineModel::createModelFromVertices(EngineDevice &device, const std::vector<TerrainVertex> &vertices, const std::vector<unsigned int> &indices){
    TerrainBuilder builder{};
    builder.LoadModel(vertices, indices);
    
    return std::make_unique<EngineModel>(device, builder);
}

void EngineModel::TerrainBuilder::LoadModel(const std::vector<TerrainVertex> &vertices, const std::vector<unsigned int> &indices){
    this->vertices.clear();
    this->indices.clear();
    
    this->vertices = vertices;
    this->indices = indices;
}


void EngineModel::Builder::LoadModel(const std::string &modelPath){
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    
    std::string warn, err;
    
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str())){
        throw std::runtime_error("Failed to load model at: " + modelPath);
    }
    
    vertices.clear();
    indices.clear();
    
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex v{};
            
            if (index.vertex_index >= 0){
                v.position = {
                    attrib.vertices[3*index.vertex_index + 0],
                    attrib.vertices[3*index.vertex_index + 1],
                    attrib.vertices[3*index.vertex_index + 2]
                };
                
                v.color = {
                    attrib.colors[3*index.vertex_index + 0],
                    attrib.colors[3*index.vertex_index + 1],
                    attrib.colors[3*index.vertex_index + 2]
                };
            }
            
            if (index.normal_index >= 0){
                v.normal = {
                    attrib.normals[3*index.normal_index + 0],
                    attrib.normals[3*index.normal_index + 1],
                    attrib.normals[3*index.normal_index + 2]
                };
            }

            
            if (index.texcoord_index >= 0){
                v.uv = {
                    attrib.texcoords[2*index.texcoord_index + 0],
                    attrib.texcoords[2*index.texcoord_index + 1]
                };
            }


            if (uniqueVertices.count(v) == 0){
                uniqueVertices[v] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(v);
            }
            indices.push_back(uniqueVertices[v]); 
        }
    }
}
