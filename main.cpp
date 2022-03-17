
#include "engine/Engine.h"
#include "engine/EngineCamera.h"

#include "terrain/IcosphereClipmap.h"
#include "terrain/GeoClipmap.h"

#include "utils/StatusPrinter.h"
 
int calculateTriangles(int n) {    
    if(n < 0)   return 1;    
    if(n == 0)  return 0;
    return ((2*n -2) *3) + calculateTriangles(n-2);   
}

int main() {
    Engine engine;
    
    EngineCamera camera{glm::vec3(0.0f, -1000.0f, 0.0f), engine.GetWindowAspectRatio()};
    IcosphereClipmap clipmap{engine};

    StatusPrinter statusPrinter; 

	float rotationAngle = 0.01f;

    while (engine.IsRunning()) {
        engine.Update();
        
		EngineSettings::LightParams.direction = glm::rotate(EngineSettings::LightParams.direction, glm::radians(rotationAngle), glm::vec3(0,0,1)); 
        EngineSettings::LightParams.direction = glm::rotate(EngineSettings::LightParams.direction, glm::radians(rotationAngle), glm::vec3(1,0,0)); 

		statusPrinter.AddParam(std::string("Vertex Num"), clipmap.GetVertexNumber()); 
		statusPrinter.AddParam(std::string("Total Vertex Num"), clipmap.GetVertexNumber() * calculateTriangles(4)); 
		statusPrinter.AddParam(std::string("Culled Faces"), clipmap.GetCulledFacesNumber()); 
		statusPrinter.AddParam(std::string("FPS"), EngineTime::FramesPerSecond()); 
        statusPrinter.Print(); 
        
        float frameTime = EngineTime::DeltaTime();
        camera.Update(frameTime);
        clipmap.Update(camera); 
        
        engine.Draw(camera);
    }
    
    engine.Quit();

    return EXIT_SUCCESS;
}