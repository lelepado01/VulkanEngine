
#include "engine/Engine.h"
#include "engine/EngineCamera.h"

#include "terrain/IcosphereClipmap.h"
#include "terrain/GeoClipmap.h"

int main() {
    Engine engine;
    
    EngineCamera camera{glm::vec3(0.0f, -1000.0f, 0.0f), engine.GetWindowAspectRatio()};
    IcosphereClipmap clipmap{engine};

	float rotationAngle = 0.0f;
	float sunRot = 0.1; 
	float radius = 4000;

	EngineSettings::LightParams.position = glm::vec3(radius,0,0); 

    while (engine.IsRunning()) {
        engine.Update();
                
		rotationAngle += sunRot;
		if(rotationAngle > 360.0f) rotationAngle = 0.0f;
	
		float radian = glm::radians(rotationAngle);

		float x = radius * cosf(radian);
		float y = radius * sinf(radian);

		EngineSettings::LightParams.position = glm::vec3(x, y, 0); 

        float frameTime = EngineTime::DeltaTime();
        camera.Update(frameTime);
        clipmap.Update(camera); 
        
        engine.Draw(camera);
    }
    
    engine.Quit();

    return EXIT_SUCCESS;
}