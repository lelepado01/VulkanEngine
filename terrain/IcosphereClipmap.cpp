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
    
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[1], initialPoints[5], initialPoints[9],    EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[5], initialPoints[11],  initialPoints[4],  EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[11], initialPoints[10],  initialPoints[2], EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[10], initialPoints[7],  initialPoints[6],  EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[7], initialPoints[1], initialPoints[8],    EngineSettings::MaxSubdivisions));

    terrainPatches.push_back(IcospherePatch(engine, initialPoints[3], initialPoints[9], initialPoints[4],  EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[3], initialPoints[4],  initialPoints[2], EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[3], initialPoints[2],  initialPoints[6], EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[3], initialPoints[6],  initialPoints[8], EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[3], initialPoints[8], initialPoints[9],  EngineSettings::MaxSubdivisions));
    
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[4], initialPoints[9], initialPoints[5],   EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[2], initialPoints[4],  initialPoints[11], EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[6], initialPoints[2],  initialPoints[10], EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[8], initialPoints[6],  initialPoints[7],  EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[9], initialPoints[8], initialPoints[1],   EngineSettings::MaxSubdivisions));

    terrainPatches.push_back(IcospherePatch(engine, initialPoints[0], initialPoints[11], initialPoints[5],  EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[0], initialPoints[5], initialPoints[1],   EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[0], initialPoints[1], initialPoints[7],   EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[0], initialPoints[7], initialPoints[10],  EngineSettings::MaxSubdivisions));
    terrainPatches.push_back(IcospherePatch(engine, initialPoints[0], initialPoints[10], initialPoints[11], EngineSettings::MaxSubdivisions));
    
}

void IcosphereClipmap::Update(const EngineCamera& camera){
    for (int i = 0; i < terrainPatches.size(); i++) {
        terrainPatches[i].SetFrustrumCulled(glm::orientedAngle(glm::normalize(camera.GetPosition()),
                                                               terrainPatches[i].faceNormal,
                                                               glm::normalize(glm::cross(camera.GetPosition(), terrainPatches[i].faceNormal))) > glm::radians(90.0f));
    }
}


unsigned int IcosphereClipmap::GetVertexNumber(){
    unsigned int v = 0;
    
    for (int i = 0; i < terrainPatches.size(); i++) {
        v += terrainPatches[i].GetVertexNumber();
    }
    
    return v;
};
