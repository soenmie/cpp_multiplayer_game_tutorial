#include "Header.hpp"
#include "CAppReceiver.hpp"

int main()
{
	testLoop();
	return 0;
}

void testLoop()
{
	irr::IrrlichtDevice* device = createDevice(video::EDT_OPENGL, core::dimension2d<irr::u32>(640, 480), 32, false, false, false, 0);

	if(!device)
		exit(EXIT_FAILURE);

	video::IVideoDriver* driver = device->getVideoDriver();

	while(device->run())
	{
		driver->beginScene(true, false, video::SColor(255, 255, 255, 255));
		driver->endScene();
	}

	device->drop();
}

void basicSceneWithMovement()
{
	IrrlichtDevice* device = irr::createDevice(EDT_OPENGL, dimension2d<u32>(640, 480), 32, false, false, false, 0);

	if(!device)
		exit(EXIT_FAILURE);

	CAppReceiver receiver;
	device->setEventReceiver(&receiver);

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	IGUIFont* fontLucida = guienv->getFont("./media/lucida.xml");
	const wchar_t* title = L"Chapter 4 Movement Demo";

	smgr->loadScene("./media/irrTest.irr");
	irr::scene::ICameraSceneNode* cam = smgr->addCameraSceneNode(0, vector3df(0, 5, -40), vector3df(0, 5, 0));

    u32 backThen = device->getTimer()->getTime();

	// This is the movement speed in units per second.
	const irr::f32 MOVEMENT_SPEED = 30.0f;
	const irr::f32 FOCUS_LENGTH = 10000;
	const float milliToSecondsRatio = 1 / 1000.0f;

	while(device->run())
	{
		 // Work out a frame delta time.
        const irr::u32 rightNow = device->getTimer()->getTime();
        const irr::f32 frameDeltaTime = (irr::f32)(rightNow - backThen) * milliToSecondsRatio; // Time in seconds
        backThen = rightNow;

		irr::core::vector3d<irr::f32> oldTarget = cam->getTarget();
		irr::core::vector3d<irr::f32> focusPoint = (oldTarget -
			cam->getAbsolutePosition()).setLength(FOCUS_LENGTH) + cam->getAbsolutePosition();

		core::vector3df nodePosition = cam->getPosition();

		if(receiver.isKeyDown(irr::KEY_KEY_W))
            nodePosition.Z += MOVEMENT_SPEED * frameDeltaTime;
        else if(receiver.isKeyDown(irr::KEY_KEY_S))
            nodePosition.Z -= MOVEMENT_SPEED * frameDeltaTime;

        if(receiver.isKeyDown(irr::KEY_KEY_A))
            nodePosition.X -= MOVEMENT_SPEED * frameDeltaTime;
        else if(receiver.isKeyDown(irr::KEY_KEY_D))
            nodePosition.X += MOVEMENT_SPEED * frameDeltaTime;

		cam->setPosition(nodePosition);
		cam->setTarget(focusPoint);

		driver->beginScene(true, true, irr::video::SColor(255, 255, 255, 255));

		smgr->drawAll();
		fontLucida->draw(title, irr::core::rect<irr::s32>(0, 0, 300, 0), irr::video::SColor(255, 0, 200, 0));

		driver->endScene();
	}

	device->drop();
}