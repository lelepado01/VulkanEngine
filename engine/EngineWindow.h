//
//  EngineWindow.hpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 14/01/22.
//

#ifndef EngineWindow_h
#define EngineWindow_h

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdexcept>

class EngineWindow {
public:
    int WIDTH = 1200;
    int HEIGHT = 800;

    bool frameBufferResized = false;
    
private:
    GLFWwindow* window;
    
public:
    EngineWindow();
    ~EngineWindow();
    
    EngineWindow(const EngineWindow&) = delete;
    EngineWindow& operator=(const EngineWindow& ) = delete; 
    
    bool IsRunning() { return !glfwWindowShouldClose(window); };
    bool WasWindowResized() { return frameBufferResized; };
    
    VkExtent2D GetExtent() { return {static_cast<uint32_t>(WIDTH), static_cast<uint32_t>(HEIGHT)};}
    GLFWwindow* GetGLFWwindow() const { return window; };
    
    void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
    void resetWindowResizedFlag() { frameBufferResized = false; };
    
private:
    void initWindow();

    static void frameBufferResizedCallback(GLFWwindow* window, int width, int height); 

};

#endif /* EngineWindow_hpp */
