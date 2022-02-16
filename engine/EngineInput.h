//
//  EngineInput.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 19/01/22.
//

#ifndef EngineInput_h
#define EngineInput_h

#include <stdio.h>
#include <vector>
#include <iostream>

#include "EngineGameObject.h"
#include "EngineWindow.h"

class EngineInput {
private:
    static GLFWwindow* window;
    
    static bool mousePositionUpdatedThisFrame;
    static float lastFrameMousePositionX;
    static float lastFrameMousePositionY;
    static float mouseOffsetX;
    static float mouseOffsetY;
    static float mouseSensitivity;
    
    static bool isFirstFrame; 
    
private:
    static void mouseHandlerFunction(GLFWwindow* window, double xpos, double ypos);
    
public:
    static void Update(); 
    
    static void SetWindow(GLFWwindow* glfwWindow);
    
    static bool KeyIsPressed(int key);
    
    static float GetMouseOffsetX() { return mouseOffsetX; };
    static float GetMouseOffsetY() { return mouseOffsetY; };

};

#endif /* EngineKeyboard_h */
