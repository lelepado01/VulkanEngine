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
#include "noise/SimplexNoise.h"

class IcospherePatch {
public:
    glm::vec3 faceNormal; 

	bool frustrumCulled = false; 

private:
    std::unordered_map<glm::vec3, unsigned int> indexVertexPair; 
    std::vector<TerrainVertex> vertices{};
    std::vector<unsigned int> indices{};
    
    unsigned int vertexIndex = 0;
    int maxSubdivisionLevel;

	glm::vec3 minVertex; 
	glm::vec3 maxVertex; 
    
    std::shared_ptr<EngineModel> terrainPatchModel;
    
private:
    void subdivideTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, int subdivisionLevel);
    void addVertexAndIndex(glm::vec3 v); 
    void addNoiseToPatch(); 
	void updateMinMax(glm::vec3 v); 
	void recalculateNormals(); 
	glm::vec3 computeVertexNormal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

public:
    IcospherePatch(Engine& engine, glm::vec3 a, glm::vec3 b, glm::vec3 c, int maxsubs);
    
	void Update(const EngineCamera& camera); 

    void SetFrustrumCulled(bool v);
    
    unsigned int GetVertexNumber();
};

#endif /* IcospherePatch_hpp */
