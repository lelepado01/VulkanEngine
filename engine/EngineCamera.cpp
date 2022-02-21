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

std::vector<glm::vec4> EngineCamera::GetFrustumPlanes() const {
	std::vector<glm::vec4> planes = std::vector<glm::vec4>(6);

    glm::mat4 v = GetViewMatrix();
    glm::mat4 p = GetProjectionMatrix();
	// glm::mat4 mvp = proj * view; 

	glm::mat4 clipMatrix;
 
    clipMatrix[0][0] = v[0][0]*p[0][0]+v[0][1]*p[1][0]+v[0][2]*p[2][0]+v[0][3]*p[3][0];
    clipMatrix[1][0] = v[0][0]*p[0][1]+v[0][1]*p[1][1]+v[0][2]*p[2][1]+v[0][3]*p[3][1];
    clipMatrix[2][0] = v[0][0]*p[0][2]+v[0][1]*p[1][2]+v[0][2]*p[2][2]+v[0][3]*p[3][2];
    clipMatrix[3][0] = v[0][0]*p[0][3]+v[0][1]*p[1][3]+v[0][2]*p[2][3]+v[0][3]*p[3][3];
    clipMatrix[0][1] = v[1][0]*p[0][0]+v[1][1]*p[1][0]+v[1][2]*p[2][0]+v[1][3]*p[3][0];
    clipMatrix[1][1] = v[1][0]*p[0][1]+v[1][1]*p[1][1]+v[1][2]*p[2][1]+v[1][3]*p[3][1];
    clipMatrix[2][1] = v[1][0]*p[0][2]+v[1][1]*p[1][2]+v[1][2]*p[2][2]+v[1][3]*p[3][2];
    clipMatrix[3][1] = v[1][0]*p[0][3]+v[1][1]*p[1][3]+v[1][2]*p[2][3]+v[1][3]*p[3][3];
    clipMatrix[0][2] = v[2][0]*p[0][0]+v[2][1]*p[1][0]+v[2][2]*p[2][0]+v[2][3]*p[3][0];
    clipMatrix[1][2] = v[2][0]*p[0][1]+v[2][1]*p[1][1]+v[2][2]*p[2][1]+v[2][3]*p[3][1];
    clipMatrix[2][2] = v[2][0]*p[0][2]+v[2][1]*p[1][2]+v[2][2]*p[2][2]+v[2][3]*p[3][2];
    clipMatrix[3][2] = v[2][0]*p[0][3]+v[2][1]*p[1][3]+v[2][2]*p[2][3]+v[2][3]*p[3][3];
    clipMatrix[0][3] = v[3][0]*p[0][0]+v[3][1]*p[1][0]+v[3][2]*p[2][0]+v[3][3]*p[3][0];
    clipMatrix[1][3] = v[3][0]*p[0][1]+v[3][1]*p[1][1]+v[3][2]*p[2][1]+v[3][3]*p[3][1];
    clipMatrix[2][3] = v[3][0]*p[0][2]+v[3][1]*p[1][2]+v[3][2]*p[2][2]+v[3][3]*p[3][2];
    clipMatrix[3][3] = v[3][0]*p[0][3]+v[3][1]*p[1][3]+v[3][2]*p[2][3]+v[3][3]*p[3][3];
 
    planes[0].x = clipMatrix[3][0]-clipMatrix[0][0];
    planes[0].y = clipMatrix[3][1]-clipMatrix[0][1];
    planes[0].z = clipMatrix[3][2]-clipMatrix[0][2];
    planes[0].w = clipMatrix[3][3]-clipMatrix[0][3];
 
    planes[1].x = clipMatrix[3][0]+clipMatrix[0][0];
    planes[1].y = clipMatrix[3][1]+clipMatrix[0][1];
    planes[1].z = clipMatrix[3][2]+clipMatrix[0][2];
    planes[1].w = clipMatrix[3][3]+clipMatrix[0][3];
 
    planes[2].x = clipMatrix[3][0]+clipMatrix[1][0];
    planes[2].y = clipMatrix[3][1]+clipMatrix[1][1];
    planes[2].z = clipMatrix[3][2]+clipMatrix[1][2];
    planes[2].w = clipMatrix[3][3]+clipMatrix[1][3];
 
    planes[3].x = clipMatrix[3][0]-clipMatrix[1][0];
    planes[3].y = clipMatrix[3][1]-clipMatrix[1][1];
    planes[3].z = clipMatrix[3][2]-clipMatrix[1][2];
    planes[3].w = clipMatrix[3][3]-clipMatrix[1][3];
 
    planes[4].x = clipMatrix[3][0]-clipMatrix[2][0];
    planes[4].y = clipMatrix[3][1]-clipMatrix[2][1];
    planes[4].z = clipMatrix[3][2]-clipMatrix[2][2];
    planes[4].w = clipMatrix[3][3]-clipMatrix[2][3];
 
    planes[5].x = clipMatrix[3][0]+clipMatrix[2][0];
    planes[5].y = clipMatrix[3][1]+clipMatrix[2][1];
    planes[5].z = clipMatrix[3][2]+clipMatrix[2][2];
    planes[5].w = clipMatrix[3][3]+clipMatrix[2][3];

	for( int i = 0; i < 6; i++ ){
        planes[i] = glm::normalize( planes[i] );
    }
	
	return planes; 
}