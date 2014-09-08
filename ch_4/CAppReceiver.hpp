#ifndef CAPPRECEIVER_H
#define CAPPRECEIVER_H

#include"Header.hpp"

class CAppReceiver : public IEventReceiver
{
private:
	bool keyDown[KEY_KEY_CODES_COUNT];
	bool keyUp[KEY_KEY_CODES_COUNT];

public:
	CAppReceiver() 
	{
		for(int i = 0; i < KEY_KEY_CODES_COUNT; ++i)
		{
			keyDown[i] = false;
		}
	}

	virtual bool OnEvent(const SEvent& event) override
	{
		switch(event.EventType)
		{
			case EET_KEY_INPUT_EVENT:
				keyDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
				keyUp[event.KeyInput.Key] = !event.KeyInput.PressedDown;
			break;

			default:
			break;
		}

		return false;
	}

	virtual bool isKeyDown(EKEY_CODE keyCode) const
	{
		return keyDown[keyCode];
	}

	virtual bool isKeyUp(EKEY_CODE keyCode) const
	{
		return keyUp[keyCode];
	}
};

#endif