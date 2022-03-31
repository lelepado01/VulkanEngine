
#include "engine/Engine.h"
#include "engine/EngineCamera.h"

#include "terrain/IcosphereClipmap.h"
#include "terrain/GeoClipmap.h"

#include "sun/Sun.h"

int main() {
	
    Engine engine;
    EngineCamera camera{glm::vec3(0.0f, -1000.0f, 0.0f), engine.GetWindowAspectRatio()};
    
	IcosphereClipmap clipmap{engine};
	Sun sun;  

    while (engine.IsRunning()) {
        engine.Update();
        
        float frameTime = EngineTime::DeltaTime();

        camera.Update(frameTime);
		sun.Update(frameTime); 
        clipmap.Update(camera); 
        
        engine.Draw(camera);
    }
    
    engine.Quit();

    return EXIT_SUCCESS;
}