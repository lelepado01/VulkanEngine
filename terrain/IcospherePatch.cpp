//
//  IcospherePatch.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 01/02/22.
//

#include "IcospherePatch.h"


IcospherePatch::IcospherePatch(Engine& engine, glm::vec3 a, glm::vec3 b, glm::vec3 c, int maxSubs) {
    maxSubdivisionLevel = maxSubs;
    center = (a + b + c) * EngineSettings::SphereRadius / 3.0f;
    faceNormal = glm::normalize(center);
    subdivideTriangle(a, b, c, 0);

	addNoiseToPatch(); 

    terrainPatchModel = engine.LoadModelFromVertices(vertices, indices);
    EngineGameObject gameObject = EngineGameObject::createGameObject();
    gameObject.model = terrainPatchModel;
    gameObject.tag = GameObjectTag::TERRAIN;
    
    engine.AddGameObject(gameObject);
}

void IcospherePatch::addVertexAndIndex(glm::vec3 v){
    if (indexVertexPair.find(v) == indexVertexPair.end()){

        float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        v = v * EngineSettings::SphereRadius / len;
        
        vertices.push_back(TerrainVertex{v, glm::vec3()});
        indexVertexPair.emplace(v, vertexIndex);
        vertexIndex++;
    }
    indices.push_back(indexVertexPair[v]);
}


void IcospherePatch::addNoiseToPatch(){
	float scale = 405.1f;
    float lacunarity = 1.5f;
    float persistance = 0.55f;
	const SimplexNoise noise(scale, 0.5f, lacunarity, persistance);

	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].position = vertices[i].position + 6.0f* glm::vec3(noise.noise(0.1f *vertices[i].position.x,0.1f * vertices[i].position.y, 0.1f *vertices[i].position.z)); 
	}
	

}

void IcospherePatch::subdivideTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, int subdivisionLevel){
    if (maxSubdivisionLevel == subdivisionLevel){
        addVertexAndIndex(a);
        addVertexAndIndex(b);
        addVertexAndIndex(c);
    } else {
        
        glm::vec3 ab = (a + b) / 2.0f;
        glm::vec3 bc = (c + b) / 2.0f;
        glm::vec3 ac = (a + c) / 2.0f;
        
        subdivideTriangle(a, ab, ac, subdivisionLevel+1);
        subdivideTriangle(b, bc, ab, subdivisionLevel+1);
        subdivideTriangle(c, ac, bc, subdivisionLevel+1);
        subdivideTriangle(ab, bc, ac, subdivisionLevel+1);
    }
}

void IcospherePatch::SetFrustrumCulled(bool v) {
    terrainPatchModel->IsCulled = v;    
};

unsigned int IcospherePatch::GetVertexNumber() { 
	if (terrainPatchModel->IsCulled) {
		return 0; 
	}
	return (unsigned int)vertices.size(); 
};