//
//  Engine.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 14/01/22.
//

#include "Engine.h"

Engine::Engine(){    
    EngineInput::SetWindow(window.GetGLFWwindow());
    
    globalPool = EngineDescriptorPool::Builder(device)
        .setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
        .build();
    
    for (int i = 0; i < globalUniformBuffers.size(); i++) {
        globalUniformBuffers[i] = std::make_unique<EngineBuffer>(device,
                                                                 sizeof(GlobalUniformBuffer),
                                                                 1,
                                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        globalUniformBuffers[i]->map();
    }
    
    auto globalSetLayout = EngineDescriptorSetLayout::Builder(device)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
        .build();
    
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = globalUniformBuffers[i]->descriptorInfo();
        EngineDescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }
    
    simpleRenderSystem = std::make_unique<SimpleRenderSystem>(device, renderer.GetSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
    terrainRenderSystem = std::make_unique<TerrainRenderSystem>(device, renderer.GetSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());    
}

Engine::~Engine(){}

void Engine::Update(){
    glfwPollEvents();
    
    EngineInput::Update();
    EngineTime::Update();
}

float angle = 0.1f;  
void Engine::Draw(EngineCamera& camera){
    
    if (auto commandBuffer = renderer.BeginFrame()){
        int frameIndex = renderer.GetFrameIndex();

        GlobalUniformBuffer ubo{};
        ubo.terrainMaterial = EngineSettings::TerrainMaterialParams;
        ubo.light = EngineSettings::LightParams;

        EngineSettings::LightParams.direction = glm::rotate(EngineSettings::LightParams.direction, glm::radians(angle), glm::vec3(0,0,1)); 
        EngineSettings::LightParams.direction = glm::rotate(EngineSettings::LightParams.direction, glm::radians(angle), glm::vec3(1,0,0)); 
        
		globalUniformBuffers[frameIndex]->writeToIndex(&ubo, frameIndex);
		globalUniformBuffers[frameIndex]->flush();
        
        renderer.BeginSwapChainRenderPass(commandBuffer);
        
        FrameInfo frameInfo{
            frameIndex,
            EngineTime::DeltaTime(),
            commandBuffer,
            camera,
            globalDescriptorSets[frameIndex],
            GetGameObjects(GameObjectTag::SIMPLE)};
        simpleRenderSystem->RenderGameObjects(frameInfo);
        
        TerrainRenderFrameInfo terrainRenderframeInfo{
            frameIndex,
            EngineTime::DeltaTime(),
            commandBuffer,
            camera,
            globalDescriptorSets[frameIndex],
            GetGameObjects(GameObjectTag::TERRAIN)};
        terrainRenderSystem->RenderGameObjects(terrainRenderframeInfo);

        renderer.EndSwapChainRenderPass(commandBuffer);
        renderer.EndFrame();
    }
}

void Engine::Quit(){
    vkDeviceWaitIdle(device.device());
}

void Engine::AddGameObject(EngineGameObject &gameObject){
    gameObjects[gameObject.tag].emplace(gameObject.getId(), std::move(gameObject));
}

EngineGameObject::Map& Engine::GetGameObjects(GameObjectTag searchedTag){
    return gameObjects[searchedTag];
};

std::unique_ptr<EngineModel> Engine::LoadModelFromFile(const std::string &path){
    return EngineModel::createModelFromFile(device, path);
}

std::unique_ptr<EngineModel> Engine::LoadModelFromVertices(const std::vector<TerrainVertex> &vertices, const std::vector<unsigned int> &indices){
    return EngineModel::createModelFromVertices(device, vertices, indices);
}
