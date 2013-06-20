#ifndef CGame_h__
#define CGame_h__

#include <Windows.h>

#include <vector>
#include <string>
using std::string;
using namespace std;

class ATHRenderer;

class CGame
{
	// Data Members ////
	bool bFullscreen;
	bool bShutdown;

	float m_fElapsedTime;
	float m_fGameTime;

	int m_nScreenWidth;
	int m_nScreenHeight;

	ATHRenderer*	m_pRenderer;

	////////////////////

	// default constructor
	CGame();
	// copy constructor
	CGame(const CGame&);
	// assignment operator
	CGame& operator=(const CGame&);
	// destructor
	~CGame();

	// utility functions
	void PreUpdate( float fDT );
	bool Update( float fDT );
	void PostUpdate( float fDT );
	void Render();

public:


	static CGame* GetInstance();

	void Initialize(HWND hWnd, HINSTANCE hInstance, 
		int nScreenWidth, int nScreenHeight, bool bIsWindowed);

	bool Main();
	void Shutdown();

	float GetElapsedTime() {return m_fElapsedTime; }
	void SetElapsedTime(float t) { m_fElapsedTime = t; }
	
	int GetScreenWidth() {return m_nScreenWidth;  }
	int GetScreenHeight() {return m_nScreenHeight; }
};


#endif // CGame_h__