//
//  IcospherePatch.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 01/02/22.
//

#ifndef IcospherePatch_h
#define IcospherePatch_h

#include <stdio.h>
#include <memory>
#include <unordered_map>

#include "../engine/Engine.h"
#include "../engine/EngineGameObject.h"
#include "../engine/EngineSettings.h"

class IcospherePatch {
public:
    glm::vec3 center; 
    glm::vec3 faceNormal; 
    
private:
    std::unordered_map<glm::vec3, unsigned int> indexVertexPair; 
    std::vector<TerrainVertex> vertices{};
    std::vector<unsigned int> indices{};
    
    unsigned int vertexIndex = 0;
    int maxSubdivisionLevel;
    bool isCulled = false;
    
    std::shared_ptr<EngineModel> terrainPatchModel;
    
private:
    void subdivideTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, int subdivisionLevel);
    void addVertexAndIndex(glm::vec3 v); 
    
public:
    IcospherePatch(Engine& engine, glm::vec3 a, glm::vec3 b, glm::vec3 c, int maxsubs);
    
    void SetFrustrumCulled(bool v);
    
    unsigned int GetVertexNumber() { return (unsigned int)vertices.size(); };
};

#endif /* IcospherePatch_hpp */
