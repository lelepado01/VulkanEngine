//
//  IcosphereClipmap.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 01/02/22.
//

#ifndef IcosphereClipmap_h
#define IcosphereClipmap_h

#include <stdio.h>
#include <vector>

#include "IcospherePatch.h"
#include "../engine/EngineSettings.h"

class IcosphereClipmap {
private:
    
    std::vector<IcospherePatch> terrainPatches;
    
private:
	void addIcosphereSubPatch(Engine& engine, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3); 

public:
    IcosphereClipmap(Engine& engine);
    
    void Update(const EngineCamera& camera);
    
    unsigned int GetVertexNumber();
    unsigned int GetCulledFacesNumber();

};


#endif /* IcosphereClipmap_hpp */
