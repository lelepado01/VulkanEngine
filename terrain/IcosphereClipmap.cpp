//
//  IcosphereClipmap.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 01/02/22.
//

#include "IcosphereClipmap.h"

IcosphereClipmap::IcosphereClipmap(Engine& engine){
        
    float t = (1.0 + sqrt(5.0)) / 2.0;
    
    std::vector<glm::vec3> initialPoints{};

    initialPoints.push_back(glm::vec3(-1,  t,  0));
    initialPoints.push_back(glm::vec3( 1,  t,  0));
    initialPoints.push_back(glm::vec3(-1, -t,  0));
    initialPoints.push_back(glm::vec3( 1, -t,  0));
    initialPoints.push_back(glm::vec3( 0, -1,  t));
    initialPoints.push_back(glm::vec3( 0,  1,  t));
    initialPoints.push_back(glm::vec3( 0, -1, -t));
    initialPoints.push_back(glm::vec3( 0,  1, -t));
    initialPoints.push_back(glm::vec3( t,  0, -1));
    initialPoints.push_back(glm::vec3( t,  0,  1));
    initialPoints.push_back(glm::vec3(-t,  0, -1));
    initialPoints.push_back(glm::vec3(-t,  0,  1));
    
    addIcosphereSubPatch(engine, initialPoints[1], initialPoints[5], initialPoints[9]);
    addIcosphereSubPatch(engine, initialPoints[5], initialPoints[11],  initialPoints[4]);
    addIcosphereSubPatch(engine, initialPoints[11], initialPoints[10],  initialPoints[2]);
    addIcosphereSubPatch(engine, initialPoints[10], initialPoints[7],  initialPoints[6]);
    addIcosphereSubPatch(engine, initialPoints[7], initialPoints[1], initialPoints[8]);

    addIcosphereSubPatch(engine, initialPoints[3], initialPoints[9], initialPoints[4]);
    addIcosphereSubPatch(engine, initialPoints[3], initialPoints[4],  initialPoints[2]);
    addIcosphereSubPatch(engine, initialPoints[3], initialPoints[2],  initialPoints[6]);
    addIcosphereSubPatch(engine, initialPoints[3], initialPoints[6],  initialPoints[8]);
    addIcosphereSubPatch(engine, initialPoints[3], initialPoints[8], initialPoints[9]);
    
    addIcosphereSubPatch(engine, initialPoints[4], initialPoints[9], initialPoints[5]);
    addIcosphereSubPatch(engine, initialPoints[2], initialPoints[4],  initialPoints[11]);
    addIcosphereSubPatch(engine, initialPoints[6], initialPoints[2],  initialPoints[10]);
    addIcosphereSubPatch(engine, initialPoints[8], initialPoints[6],  initialPoints[7]);
    addIcosphereSubPatch(engine, initialPoints[9], initialPoints[8], initialPoints[1]);

    addIcosphereSubPatch(engine, initialPoints[0], initialPoints[11], initialPoints[5]);
    addIcosphereSubPatch(engine, initialPoints[0], initialPoints[5], initialPoints[1]);
    addIcosphereSubPatch(engine, initialPoints[0], initialPoints[1], initialPoints[7]);
    addIcosphereSubPatch(engine, initialPoints[0], initialPoints[7], initialPoints[10]);
    addIcosphereSubPatch(engine, initialPoints[0], initialPoints[10], initialPoints[11]);
    
}

void IcosphereClipmap::Update(const EngineCamera& camera){
    for (int i = 0; i < terrainPatches.size(); i++) {
        terrainPatches[i].Update(camera); 
    }
}


void IcosphereClipmap::addIcosphereSubPatch(Engine& engine, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3){
	glm::vec3 p12 = (p1 + p2) / 2.0f; 
	glm::vec3 p23 = (p2 + p3) / 2.0f; 
	glm::vec3 p31 = (p3 + p1) / 2.0f; 
	terrainPatches.push_back(IcospherePatch(engine, p1, p12, p31, EngineSettings::MaxSubdivisions));
	terrainPatches.push_back(IcospherePatch(engine, p2, p23, p12, EngineSettings::MaxSubdivisions));
	terrainPatches.push_back(IcospherePatch(engine, p3, p31, p23, EngineSettings::MaxSubdivisions));
	terrainPatches.push_back(IcospherePatch(engine, p12, p23, p31, EngineSettings::MaxSubdivisions));
}


unsigned int IcosphereClipmap::GetVertexNumber(){
    unsigned int v = 0;
    
    for (int i = 0; i < terrainPatches.size(); i++) {
        v += terrainPatches[i].GetVertexNumber();
    }
    
    return v;
};

unsigned int IcosphereClipmap::GetCulledFacesNumber(){
	unsigned int v = 0;
    
    for (int i = 0; i < terrainPatches.size(); i++) {
        if (terrainPatches[i].frustrumCulled) v++; 
    }
    
    return v;
}