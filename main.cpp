
#include "engine/Engine.h"
#include "engine/EngineCamera.h"

#include "terrain/IcosphereClipmap.h"

#include "utils/StatusPrinter.h"
 
int main() {
    Engine engine;
    
    EngineCamera camera{engine.GetWindowAspectRatio()};
    IcosphereClipmap clipmap{engine};

    StatusPrinter statusPrinter; 

	float rotationAngle = 0.01f;  

    while (engine.IsRunning()) {
        engine.Update();
        
		EngineSettings::LightParams.direction = glm::rotate(EngineSettings::LightParams.direction, glm::radians(rotationAngle), glm::vec3(0,0,1)); 
        EngineSettings::LightParams.direction = glm::rotate(EngineSettings::LightParams.direction, glm::radians(rotationAngle), glm::vec3(1,0,0)); 

        statusPrinter.Print(clipmap.GetVertexNumber(), clipmap.GetCulledFacesNumber(), EngineTime::FramesPerSecond()); 
        
        float frameTime = EngineTime::DeltaTime();
        camera.Update(frameTime);
        clipmap.Update(camera); 
        
        engine.Draw(camera);
    }
    
    engine.Quit();

    return EXIT_SUCCESS;
}