//
//  EngineVertex.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 30/01/22.
//

#ifndef EngineVertex_h
#define EngineVertex_h

struct Vertex{
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};
    
    bool operator==(const Vertex& other) const {
        return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
    }
    
    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions(){
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }
    
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(){
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        
//        attributeDescriptions.push_back({0,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({0,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back({2,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDescriptions.push_back({3,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

        return attributeDescriptions;
    }
};

struct TerrainVertex{
    glm::vec3 position{};
    glm::vec3 normal{};
	int lod; 
    
    bool operator==(const TerrainVertex& other) const {
        return position == other.position && normal == other.normal && lod == other.lod;
    }
    
    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions(){
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(TerrainVertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }
    
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(){
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        
        attributeDescriptions.push_back({0,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(TerrainVertex, position)});
        attributeDescriptions.push_back({1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(TerrainVertex, normal)});
        attributeDescriptions.push_back({2,0,VK_FORMAT_R8_SINT, offsetof(TerrainVertex, lod)});

        return attributeDescriptions;
    }
};

namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const &vertex) const {
    size_t seed = 0;
      hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
    return seed;
  }
};

template <>
struct hash<TerrainVertex> {
  size_t operator()(TerrainVertex const &vertex) const {
    size_t seed = 0;
      hashCombine(seed, vertex.position, vertex.normal, vertex.lod);
    return seed;
  }
};
}  // namespace std

#endif /* EngineVertex_h */
