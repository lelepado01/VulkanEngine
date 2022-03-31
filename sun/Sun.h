
#include "../engine/EngineSettings.h"

class Sun {
private:
	float rotationAngle = 0.0f;
	float rotationSpeed = 0.9f; 
	float radiusOfOrbit = 4000.0f;

public:
	Sun();

	void Update(float deltaTime); 
};
