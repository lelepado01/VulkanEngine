//
//  IcospherePatch.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 01/02/22.
//

#include "IcospherePatch.h"


IcospherePatch::IcospherePatch(Engine& engine, glm::vec3 a, glm::vec3 b, glm::vec3 c, int maxSubs) {
    maxSubdivisionLevel = maxSubs;
    glm::vec3 center = (a + b + c) * EngineSettings::SphereRadius / 3.0f;
    faceNormal = glm::normalize(center);

    subdivideTriangle(a, b, c, 0);
	addNoiseToPatch(); 
	recalculateNormals(); 

	minVertex = (minVertex*3.0f + center) / 4.0f; 
	maxVertex = (maxVertex*3.0f + center) / 4.0f;

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
		updateMinMax(v);  

        vertexIndex++;
    }
    indices.push_back(indexVertexPair[v]);
}

void IcospherePatch::updateMinMax(glm::vec3 v){
	minVertex.x = fmin(v.x, minVertex.x);
	minVertex.y = fmin(v.y, minVertex.y);
	minVertex.z = fmin(v.z, minVertex.z);

	maxVertex.x = fmax(v.x, maxVertex.x);
	maxVertex.y = fmax(v.y, maxVertex.y);
	maxVertex.z = fmax(v.z, maxVertex.z);
}

void IcospherePatch::addNoiseToPatch(){
	float scale = 405.1f;
    float lacunarity = 1.5f;
    float persistance = 0.55f;
	const SimplexNoise noise1(scale, 0.5f, lacunarity, persistance);
	const SimplexNoise noise2(500.0f, 0.5f, 0.1f, 0.9f);

	float l1Scale = 0.005f; 
	float l2Scale = 0.6f; 

	for (int i = 0; i < vertices.size(); i++) {
		float noiseHeight = 7.0f * noise1.noise(l1Scale *vertices[i].position.x, l1Scale * vertices[i].position.y, l1Scale *vertices[i].position.z);
		noiseHeight += 4.5f*noise2.noise(l2Scale *vertices[i].position.x, l2Scale *vertices[i].position.y, l2Scale *vertices[i].position.z) * noise2.noise(l2Scale *vertices[i].position.x, l2Scale *vertices[i].position.y, l2Scale *vertices[i].position.z);
		vertices[i].position += vertices[i].position * fmax( 0.002f * noiseHeight, -0.002f); 
	}
}

glm::vec3 IcospherePatch::computeVertexNormal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    return glm::cross(b-a, c-a);
}

void IcospherePatch::recalculateNormals(){
	for (int normalTriangleIndex = 0; normalTriangleIndex < indices.size(); normalTriangleIndex+=3) {
        int vertexIndexA = indices.at(normalTriangleIndex);
        int vertexIndexB = indices.at(normalTriangleIndex + 1);
        int vertexIndexC = indices.at(normalTriangleIndex + 2);

        glm::vec3 normal = computeVertexNormal(vertices.at(vertexIndexA).position,
                                               vertices.at(vertexIndexB).position,
                                               vertices.at(vertexIndexC).position);
        
        vertices[vertexIndexA].normal += normal;
        vertices[vertexIndexB].normal += normal;
        vertices[vertexIndexC].normal += normal;
    }
    
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].normal = glm::normalize(vertices[i].normal);
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

void IcospherePatch::Update(const EngineCamera& camera){
	float orientedAngle = glm::orientedAngle(glm::normalize(camera.GetPosition()), 
											faceNormal,
                           					glm::normalize(glm::cross(camera.GetPosition(), faceNormal))); 

	bool inFrustrum = camera.BoxIsVisible(minVertex, maxVertex); 
	frustrumCulled = orientedAngle > glm::radians(90.0f);// || !inFrustrum; 
	SetFrustrumCulled(frustrumCulled);
}