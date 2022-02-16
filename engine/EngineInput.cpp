//
//  EngineKeyboard.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 19/01/22.
//

#include "EngineInput.h"

bool EngineInput::isFirstFrame = true;
bool EngineInput::mousePositionUpdatedThisFrame = false;

float EngineInput::lastFrameMousePositionX = 0;
float EngineInput::lastFrameMousePositionY = 0;
float EngineInput::mouseOffsetX = 0;
float EngineInput::mouseOffsetY = 0;
float EngineInput::mouseSensitivity = 0.1f;

GLFWwindow* EngineInput::window = nullptr;

void EngineInput::Update(){
    if (!mousePositionUpdatedThisFrame){
        mouseOffsetX = 0;
        mouseOffsetY = 0;
    }
    
    mousePositionUpdatedThisFrame = false;
}

bool EngineInput::KeyIsPressed(int keyTested){
    if (window == nullptr)
        std::cout << "Engine Window needs to be set, call SetWindow(GLFWwindow*)\n";
    return glfwGetKey(window, keyTested);
}

void EngineInput::mouseHandlerFunction(GLFWwindow* window, double xpos, double ypos){
    mousePositionUpdatedThisFrame = true;
    
    if (isFirstFrame) {
        lastFrameMousePositionX = xpos;
        lastFrameMousePositionY = ypos;
        isFirstFrame = false;
    }
    
    mouseOffsetX = xpos - lastFrameMousePositionX;
    mouseOffsetY = ypos - lastFrameMousePositionY;
        
    lastFrameMousePositionX = xpos;
    lastFrameMousePositionY = ypos;
    
    mouseOffsetX *= mouseSensitivity;
    mouseOffsetY *= mouseSensitivity;
}

void EngineInput::SetWindow(GLFWwindow *glfwWindow){
   window = glfwWindow;
   glfwSetCursorPosCallback(window, mouseHandlerFunction);
}
