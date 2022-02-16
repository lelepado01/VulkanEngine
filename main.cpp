
#include "engine/Engine.h"
#include "engine/EngineCamera.h"

#include "terrain/IcosphereClipmap.h"

#include "utils/StatusPrinter.h"
 
int main() {
    Engine engine;
    
    EngineCamera camera{engine.GetWindowAspectRatio()};
    IcosphereClipmap clipmap{engine};

    StatusPrinter statusPrinter; 
    
    while (engine.IsRunning()) {
        engine.Update();
        
        statusPrinter.Print(clipmap.GetVertexNumber(), EngineTime::FramesPerSecond()); 
        
        float frameTime = EngineTime::DeltaTime();
        camera.Update(frameTime);
        clipmap.Update(camera); 
        
        engine.Draw(camera);
    }
    
    engine.Quit();

    return EXIT_SUCCESS;
}