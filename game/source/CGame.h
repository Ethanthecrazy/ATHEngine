#ifndef CGame_h__
#define CGame_h__

#include "../../engine/ATHUtil/UTimer.h"
#include "../../engine/ATHEngine.h"
#include "ObjectGenerator.h"

#include <vector>
#include <string>
using std::string;
using namespace std;

class CGame
{
	// Data Members ////
	bool bFullscreen;
	bool bShutdown;

	float m_fElapsedTime;
	float m_fGameTime;

	unsigned int m_unScreenWidth;
	unsigned int m_unScreenHeight;

	CTimer			m_Timer;
	float			m_fFrameTime;
	unsigned int	m_unFrameCounter;

	// default constructor
	CGame();
	// copy constructor
	CGame(const CGame&);
	// assignment operator
	CGame& operator=(const CGame&);
	// destructor
	~CGame();

	// utility functions
	void Render();

public:

	ATHEngine m_Engine;
	ObjectGenerator m_ObjectGenerator;

	static CGame* GetInstance();

	void Initialize();

	bool Main();
	void Shutdown();

	float GetElapsedTime() {return m_fElapsedTime; }
	void SetElapsedTime(float t) { m_fElapsedTime = t; }

};


#endif // CGame_h__