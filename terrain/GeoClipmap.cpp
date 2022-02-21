//
//  GeoClipmap.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 26/01/22.
//

#include "GeoClipmap.h"

GeoClipmap::GeoClipmap(Engine& engine){
    init();

	terrainPatchModel = engine.LoadModelFromVertices(vertices, indices);
    EngineGameObject gameObject = EngineGameObject::createGameObject();
    gameObject.model = terrainPatchModel;
    gameObject.tag = GameObjectTag::TERRAIN;
    
    engine.AddGameObject(gameObject);
}

void GeoClipmap::init(){
    
    int centerOffset = blockSize/2;
    
    createBlock(-centerOffset, -centerOffset, 1);
    
    for (int x = -1; x < 2; x++) {
        for (int y = -1; y < 2; y++) {
            if (x != 0 || y != 0){
                createBlock(x * blockSize - centerOffset, y*blockSize - centerOffset, 2);
            }
        }
    }
    
    for (int x = -2; x < 3; x++) {
        for (int y = -2; y < 3; y++) {
            if (x == -2 || y == -2 || x == 2 || y == 2) {
                createBlock(x * blockSize - centerOffset, y*blockSize - centerOffset, 4);
            }
        }
    }
    
    for (int x = -3; x < 4; x++) {
        for (int y = -3; y < 4; y++) {
            if (x == -3 || y == -3 || x == 3 || y == 3) {
                createBlock(x * blockSize - centerOffset, y*blockSize - centerOffset, 8);
            }
        }
    }
}

void GeoClipmap::createBlock(int xStart, int yStart, int lod){

    int verticesPerSide = blockSize / lod;
    float distanceBetweenVertices = blockSize / verticesPerSide;
    int vertexIndex = static_cast<int>(vertices.size());
    
    for (int x = 0; x <= verticesPerSide; x++) {
        for (int y = 0; y <= verticesPerSide; y++) {
            int xPos = xStart + x*distanceBetweenVertices;
            int yPos = yStart + y*distanceBetweenVertices;
            vertices.push_back({{xPos, 0, yPos}, {0,0,0}});
        }
    }
    
    for (int z = 0; z <= verticesPerSide; z++) {
        for (int x = 0; x <= verticesPerSide; x++) {
            if (x < verticesPerSide && z < verticesPerSide){
                
                indices.push_back(vertexIndex);
                indices.push_back(vertexIndex + verticesPerSide + 1);
                indices.push_back(vertexIndex + 1);

                indices.push_back(vertexIndex + 1);
                indices.push_back(vertexIndex + verticesPerSide + 1);
                indices.push_back(vertexIndex + verticesPerSide + 2);

                vertexIndex++;
            }
        }
        vertexIndex++;
    }
}

void GeoClipmap::Update(const EngineCamera &camera){
    
}
