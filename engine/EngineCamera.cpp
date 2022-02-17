//
//  EngineCamera.cpp
//  VulkanEngine
//
//  Created by Gabriele Padovani on 18/01/22.
//

#include "EngineCamera.h"
#include "glm/gtx/string_cast.hpp"

EngineCamera::EngineCamera(float aspectRatio){
    yaw = 0;
    pitch = 0;
    this->aspectRatio = aspectRatio;
    
    position = glm::vec3(0.0f, -1000.0f, 0.0f);
    up = glm::vec3(0.0f, 1.0f,  0.0f);
    front = glm::vec3(0.0f, 0.0f, 1.0f);
    direction = front; 
    projectionMatrix = glm::perspective(glm::radians(45.f), aspectRatio, 0.1f, 10000.0f);
    
    recalculateCameraView();
    recalculateCameraDirection();

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
        
    recalculateCameraProjection();
    recalculateCameraView();
}


void EngineCamera::recalculateCameraProjection(){
    float distanceFromSurface = glm::distance(position, glm::vec3(0,0,0)) - EngineSettings::SphereRadius;
    float currentFOV = (maxFOV - minFOV) * (distanceFromSurface / maxDistance) + minFOV;
    projectionMatrix = glm::perspective(glm::radians(currentFOV), aspectRatio, 0.1f, 10000.0f);
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

std::vector<glm::vec4> EngineCamera::GetFrustumPlanes(){
	std::vector<glm::vec4> planes = std::vector<glm::vec4>(6);

	glm::mat4 mvp = GetViewMatrix() * GetProjectionMatrix(); 

	planes[0].x = mvp[3][0] + mvp[0][0];
	planes[0].y = mvp[3][1] + mvp[0][1];
	planes[0].z = mvp[3][2] + mvp[0][2];
	planes[0].w = mvp[3][3] + mvp[0][3];
	// Right clipping plane
	planes[1].x = mvp[3][0] - mvp[0][0];
	planes[1].y = mvp[3][1] - mvp[0][1];
	planes[1].z = mvp[3][2] - mvp[0][2];
	planes[1].w = mvp[3][3] - mvp[0][3];
	// Top clipping plane
	planes[2].x = mvp[3][0] - mvp[1][0];
	planes[2].y = mvp[3][1] - mvp[1][1];
	planes[2].z = mvp[3][2] - mvp[1][2];
	planes[2].w = mvp[3][3] - mvp[1][3];
	// Bottom clipping plane
	planes[3].x = mvp[3][0] + mvp[1][0];
	planes[3].y = mvp[3][1] + mvp[1][1];
	planes[3].z = mvp[3][2] + mvp[1][2];
	planes[3].w = mvp[3][3] + mvp[1][3];
	// Near clipping plane
	planes[4].x = mvp[3][0] + mvp[2][0];
	planes[4].y = mvp[3][1] + mvp[2][1];
	planes[4].z = mvp[3][2] + mvp[2][2];
	planes[4].w = mvp[3][3] + mvp[2][3];
	// Far clipping plane
	planes[5].x = mvp[3][0] - mvp[2][0];
	planes[5].y = mvp[3][1] - mvp[2][1];
	planes[5].z = mvp[3][2] - mvp[2][2];
	planes[5].w = mvp[3][3] - mvp[2][3];

	return planes; 
}