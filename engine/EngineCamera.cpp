//
//  EngineCamera.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 18/01/22.
//

#include "EngineCamera.h"
#include "glm/gtx/string_cast.hpp"

EngineCamera::EngineCamera(glm::vec3 initialPosition, float aspectRatio){
    yaw = 0;
    pitch = 0;
    this->aspectRatio = aspectRatio;
    
    position = initialPosition;
    up = glm::vec3(0.0f, 1.0f,  0.0f);
    front = glm::vec3(0.0f, 0.0f, 1.0f);
    direction = front; 
    projectionMatrix = glm::perspective(glm::radians(45.f), aspectRatio, 0.1f, 10000.0f);
    
    recalculateCameraView();
    recalculateCameraDirection();
	
	cameraFrustum = Frustum(GetProjectionMatrix() * GetViewMatrix()); 
}

void EngineCamera::Update(float deltaTime){
    updateDirection();
    updatePosition(deltaTime);
}

void EngineCamera::updateDirection(){    
    yaw   += EngineInput::GetMouseOffsetX();
    pitch += EngineInput::GetMouseOffsetY();
    
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    recalculateCameraDirection();
}


void EngineCamera::updatePosition(float deltaTime){
            
    if (EngineInput::KeyIsPressed(GLFW_KEY_W)){
        position += movementSpeed * front * deltaTime;
    }
    if (EngineInput::KeyIsPressed(GLFW_KEY_S)){
        position -= movementSpeed * front * deltaTime;
    }
    if (EngineInput::KeyIsPressed(GLFW_KEY_A)){
        position -= movementSpeed * glm::normalize(glm::cross(front, up)) * deltaTime;
    }
    if (EngineInput::KeyIsPressed(GLFW_KEY_D)){
        position += movementSpeed * glm::normalize(glm::cross(front, up)) * deltaTime;
    }
    if (EngineInput::KeyIsPressed(GLFW_KEY_SPACE)){
        position -= movementSpeed * glm::normalize(up) * deltaTime;
    }
    if (EngineInput::KeyIsPressed(GLFW_KEY_LEFT_SHIFT)){
        position += movementSpeed * glm::normalize(up) * deltaTime;
    }
        
    recalculateCameraView();
	cameraFrustum = Frustum(GetProjectionMatrix() * GetViewMatrix()); 
}

void EngineCamera::recalculateCameraView(){
    viewMatrix = glm::lookAt(position, position + front, up);
}

void EngineCamera::recalculateCameraDirection(){
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}

bool EngineCamera::BoxIsVisible(glm::vec3 minVertex, glm::vec3 maxVertex) const{
	return cameraFrustum.IsBoxVisible(minVertex, maxVertex); 
}