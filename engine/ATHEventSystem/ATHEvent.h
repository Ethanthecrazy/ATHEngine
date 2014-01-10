#ifndef ATHEVENT_H
#define ATHEVENT_H

enum ATHEventType{ AET_SYSTEM, AET_KEYBOARD, AET_MOUSE, AET_OBJECT };
enum ATHEventPriority{ AEP_NORMAL, AEP_IMMEDIATE };

class ATHEvent
{

public:

	ATHEventType m_EventType;
	
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
			unsigned int MSE_unPosX;
			unsigned int MSE_unPosY; 
		};
	};
};

#endif
