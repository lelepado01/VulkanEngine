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
    
public:
    IcosphereClipmap(Engine& engine);
    
    void Update(const EngineCamera& camera);
    
    unsigned int GetVertexNumber();

};


#endif /* IcosphereClipmap_hpp */
