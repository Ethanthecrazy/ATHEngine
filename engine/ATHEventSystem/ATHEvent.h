#ifndef ATHEVENT_H
#define ATHEVENT_H

// I dont understand why I had to include this to use memset, but whatever.
#include <memory>

enum ATHEventType{ AET_SYSTEM, AET_KEYBOARD, AET_MOUSE, AET_OBJECT };
enum ATHEventPriority{ AEP_NORMAL, AEP_IMMEDIATE };
enum ATHEventID{ AEI_NONE, AEI_KEYDOWN, AEI_KEYUP, AEI_MOUSEDOWN, AEI_MOUSEUP };

class ATHEvent
{
private:

	ATHEvent();

public:

	ATHEventType m_EventType;
	ATHEventID m_EventID;
	
	// Different prefixes for each type of event
	union
	{
		struct
		{
			char SYS_szGenericData[32];
		};

		struct
		{ 
			char KEY_szKeysPressed[8];
			char KEY_szKeysReleased[8];
		};

		struct
		{
			char MSE_szMouseButtonsDown[8];
			float MSE_unPosX;
			float MSE_unPosY; 
		};
	};


	ATHEvent(ATHEventType _eventType) : m_EventType(_eventType), m_EventID( AEI_NONE )
	{
		memset( SYS_szGenericData, -1, 32 );
	}

};

#endif
