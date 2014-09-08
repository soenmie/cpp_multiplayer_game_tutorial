#include <iostream>
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace video;

int main(int argc, const char * argv[])
{
    auto device = createDevice(EDT_OPENGL, dimension2d<u32>(640, 480), false, false, false, 0);
    
    if(!device)
        return 1;
    
    auto driver = device->getVideoDriver();
    
    while(device->run())
    {
        driver->beginScene(true, true, SColor(255, 128, 128, 128));
        driver->endScene();
    }
    
    device->drop();
    
    return 0;
}
