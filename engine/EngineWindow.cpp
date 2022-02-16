//
//  EngineWindow.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 14/01/22.
//

#include "EngineWindow.h"

EngineWindow::EngineWindow(){
    initWindow();
}

EngineWindow::~EngineWindow(){
    glfwDestroyWindow(window);
    glfwTerminate();
}

void EngineWindow::initWindow(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, frameBufferResizedCallback);
}

void EngineWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
    if (glfwCreateWindowSurface(instance, window, nullptr, surface)){
        throw std::runtime_error("failed to create window surface");
    }
}

void EngineWindow::frameBufferResizedCallback(GLFWwindow *window, int width, int height){
    auto resizedWindow = reinterpret_cast<EngineWindow*>(glfwGetWindowUserPointer(window));
    resizedWindow->frameBufferResized = true;
    resizedWindow->WIDTH = width;
    resizedWindow->HEIGHT = height;
}
