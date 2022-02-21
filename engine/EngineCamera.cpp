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

	glm::vec3 camera = GetPosition();
    glm::vec3 target = camera + GetDirection(); // add camera position plus target direction to get target location for view matrix function
    glm::mat4 view = glm::lookAt(camera, target, up);
    glm::mat4 proj = GetProjectionMatrix();
	glm::mat4 mvp = proj * view; 

	// left
    planes[0].x = mvp[0][3] + mvp[0][0];
    planes[0].y = mvp[1][3] + mvp[1][0];
    planes[0].z = mvp[2][3] + mvp[2][0];
    planes[0].w = mvp[3][3] + mvp[3][0];
    // right
    planes[1].x = mvp[0][3] - mvp[0][0];
    planes[1].y = mvp[1][3] - mvp[1][0];
    planes[1].z = mvp[2][3] - mvp[2][0];
    planes[1].w = mvp[3][3] - mvp[3][0];
    // bottom
    planes[2].x = mvp[0][3] + mvp[0][1];
    planes[2].y = mvp[1][3] + mvp[1][1];
    planes[2].z = mvp[2][3] + mvp[2][1];
    planes[2].w = mvp[3][3] + mvp[3][1];
    // top
    planes[3].x = mvp[0][3] - mvp[0][1];
    planes[3].y = mvp[1][3] - mvp[1][1];
    planes[3].z = mvp[2][3] - mvp[2][1];
    planes[3].w = mvp[3][3] - mvp[3][1];
    // near
    planes[4].x = mvp[0][2];
    planes[4].y = mvp[1][2];
    planes[4].z = mvp[2][2];
    planes[4].w = mvp[3][2];
    // far
    planes[5].x = mvp[0][3] - mvp[0][2];
    planes[5].y = mvp[1][3] - mvp[1][2];
    planes[5].z = mvp[2][3] - mvp[2][2];
    planes[5].w = mvp[3][3] - mvp[3][2];

	for (int i = 0; i < 6; i++){
		float mag = sqrt(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z);
		planes[i] = planes[i] / mag; 
	}
	
	return planes; 
}