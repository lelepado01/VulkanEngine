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
	recalculateNormals(); 

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
	minVertex.y = fmin(v.x, minVertex.y);
	minVertex.z = fmin(v.x, minVertex.z);

	maxVertex.x = fmax(v.x, maxVertex.x);
	maxVertex.y = fmax(v.x, maxVertex.y);
	maxVertex.z = fmax(v.x, maxVertex.z);
}

void IcospherePatch::addNoiseToPatch(){
	float scale = 305.1f;
    float lacunarity = 1.5f;
    float persistance = 0.55f;
	const SimplexNoise noise1(scale, 0.5f, lacunarity, persistance);
	const SimplexNoise noise2(500.0f, 0.5f, 0.1f, 0.9f);

	for (int i = 0; i < vertices.size(); i++) {
		float noiseHeight = 6.0f * noise1.noise(0.1f *vertices[i].position.x,0.1f * vertices[i].position.y, 0.1f *vertices[i].position.z);
		noiseHeight += 2.0f*noise2.noise(vertices[i].position.x, vertices[i].position.y, vertices[i].position.z);
		vertices[i].position = vertices[i].position + glm::vec3(noiseHeight); 
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

float DistancePlaneToPoint(glm::vec3 center, glm::vec3 extents, glm::vec4 plane) {
	glm::vec3 n = glm::abs(glm::vec3(plane.x, plane.y, plane.z));
	float r = glm::dot(extents, n);
	float s = glm::dot(glm::vec4(center, 1.0f), plane);
	std::cout << s+r << "\n"; 
	return s + r;
}

bool AABBOutsideFrustumTest(glm::vec3 left, glm::vec3 right, glm::vec4 planes[6]) {
	for(int i = 0; i < 6; i++) {
		if (DistancePlaneToPoint(left, right, planes[i]) < 0.0f){
			return true;
		}
  	}
  	return false;
}


void IcospherePatch::Update(const EngineCamera& camera){
	float orientedAngle = glm::orientedAngle(glm::normalize(camera.GetPosition()), 
											faceNormal,
                           					glm::normalize(glm::cross(camera.GetPosition(), faceNormal))); 
	
	std::vector<glm::vec4> p = camera.GetFrustumPlanes(); 
	glm::vec3 boxCenter = 0.5f * (minVertex + maxVertex);
    glm::vec3 boxExtents = 0.5f * (maxVertex - minVertex);

	glm::mat4 mvp = camera.GetViewMatrix()*camera.GetProjectionMatrix();
	glm::vec4 v4bCenter = mvp * glm::vec4(boxCenter.x, boxCenter.y, boxCenter.z, 1.0f);
	glm::vec4 v4bExtents = mvp * glm::vec4(boxExtents.x, boxExtents.y, boxExtents.z, 1.0f);
	boxCenter = glm::vec3(v4bCenter.x, v4bCenter.y, v4bCenter.z); 
	boxExtents = glm::vec3(v4bExtents.x, v4bExtents.y, v4bExtents.z); 
	bool outOfFrustrum = AABBOutsideFrustumTest(boxCenter, boxExtents, p.data()); 
	frustrumCulled = outOfFrustrum; 

	SetFrustrumCulled(orientedAngle > glm::radians(45.0f) || outOfFrustrum);
}