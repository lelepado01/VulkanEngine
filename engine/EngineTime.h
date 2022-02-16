//
//  EngineTime.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 19/01/22.
//

#ifndef EngineTime_h
#define EngineTime_h

#include <stdio.h>
#include <chrono>
#include <iostream>
#include <cmath>

class EngineTime {
private:
    static std::chrono::steady_clock::time_point lastFrameTime;

    inline static float deltaTime = 0;
    
public:
    static void Update();
    
    static float DeltaTime() { return deltaTime; };
    static float FramesPerSecond() { return 1 / deltaTime; };
};

#endif /* EngineTime_hpp */
