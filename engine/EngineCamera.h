//
//  EngineCamera.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 18/01/22.
//

#ifndef EngineCamera_h
#define EngineCamera_h

#include <stdio.h>
#include <cassert>
#include <limits>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "EngineInput.h"
#include "EngineSettings.h"

class EngineCamera {
private:
    const float movementSpeed = EngineSettings::CameraSpeed;

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 direction;
    
    float pitch;
    float yaw;
    float aspectRatio;
    
    const float maxDistance = EngineSettings::SphereRadius * 2;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    
public:
    
private:
    void recalculateCameraView();
    void recalculateCameraDirection();
    void updatePosition(float deltaTime);
    void updateDirection();

public:
    EngineCamera(glm::vec3 initialPosition, float aspectRatio);
    
    const glm::mat4& GetProjectionMatrix() const { return projectionMatrix; };
    const glm::mat4& GetViewMatrix() const { return viewMatrix; };
    const glm::vec3& GetPosition() const { return position; };
    const glm::vec3& GetDirection() const { return direction; };

    void Update(float deltaTime);        
    void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up);

	std::vector<glm::vec4> GetFrustumPlanes() const; 

};

#endif /* EngineCamera_h */
