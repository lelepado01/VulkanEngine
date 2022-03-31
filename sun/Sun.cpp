

#include "Sun.h"

Sun::Sun() {
	EngineSettings::LightParams.position = glm::vec3(radiusOfOrbit,0,0); 
}

void Sun::Update(float deltaTime){
	rotationAngle += rotationSpeed * deltaTime;
	if(rotationAngle > 360.0f) rotationAngle = 0.0f;

	float radian = glm::radians(rotationAngle);

	float x = radiusOfOrbit * cosf(radian);
	float y = radiusOfOrbit * sinf(radian);

	EngineSettings::LightParams.position = glm::vec3(x, y, 0); 
}