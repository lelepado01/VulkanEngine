//
//  GeoClipmap.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 26/01/22.
//

#ifndef GeoClipmap_h
#define GeoClipmap_h

#include <stdio.h>
#include <vector>
#include <memory>

#include "../engine/Engine.h"
#include "../engine/EngineModel.h"
#include "../engine/EngineCamera.h"
#include "../engine/EngineVertex.h"

class GeoClipmap {
private:
    std::vector<TerrainVertex> vertices{};
    std::vector<unsigned int> indices{};
    
    int blockSize = 64;

	std::shared_ptr<EngineModel> terrainPatchModel;
    
private:
    void init();
    void createBlock(int xStart, int yStart, int lod);
    
public:
    GeoClipmap(Engine& engine);
    
    void Update(const EngineCamera& camera);
    
    std::vector<TerrainVertex>& GetVertices() { return vertices; };
    std::vector<unsigned int>& GetIndices() { return indices; };
    
    unsigned int GetVertexNumber() { return (unsigned int)vertices.size(); }; 

};

#endif /* GeoClipmap_h */
