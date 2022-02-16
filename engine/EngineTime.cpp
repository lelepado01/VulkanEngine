//
//  EngineTime.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 19/01/22.
//

#include "EngineTime.h"

std::chrono::steady_clock::time_point EngineTime::lastFrameTime = std::chrono::high_resolution_clock::now();

void EngineTime::Update() {
    std::chrono::steady_clock::time_point newTime = std::chrono::high_resolution_clock::now();
    float frametime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - lastFrameTime).count();
    lastFrameTime = newTime;
    
    deltaTime = frametime;
}
