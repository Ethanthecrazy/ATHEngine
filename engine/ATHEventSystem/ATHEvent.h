#ifndef ATHEVENT_H
#define ATHEVENT_H

enum ATHEventType{ AET_SYSTEM, AET_KEYDOWN, AET_MOUSEPOS };
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
			char KEY_szKeysDown[8];
			char KEY_szMouseButtonsDown[8];
		};

		struct
		{
			unsigned int MSE_unPosX;
			unsigned int MSE_unPosY; 
		};
	};
};

#endif
