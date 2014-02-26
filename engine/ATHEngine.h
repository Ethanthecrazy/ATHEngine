#ifndef ATHENGINE_H
#define ATHENGINE_H

// Windows dependent :/
#include <Windows.h>

class ATHRenderer;
class ATHObjectManager;
class ATHScriptManager;
class ATHInputManager;
class ATHEventManager;
class ATHAudio;

class ATHEngine
{

private:

	ATHRenderer*		m_pRenderer;
	ATHObjectManager*	m_pObjectManager;
	ATHScriptManager*   m_pScriptManager;
	ATHInputManager*	m_pInputManager;
	ATHEventManager*	m_pEventManager;
	ATHAudio*			m_pAudioManager;

	bool m_bFullscreen;
	bool m_bVsync;
	bool m_bShutdown;
	bool m_bDebugLines;

	int m_nScreenWidth;
	int m_nScreenHeight;

	float m_fGameTime;

#ifdef _WIN32
	HWND		m_hWnd;
	HINSTANCE	m_hInstance;
	WNDPROC		m_winProc;
	char*		m_szClassName;
	int			m_nCmdShow;
	bool		ViewportWindows();
#endif


public:

	ATHEngine();

	void Init();
	void LoadConfig();
	bool CreateViewport();
	void TestInit();
	bool Update( float _fDT );
	void Render();
	void Shutdown();

	ATHRenderer*		GetRenderer(){ return m_pRenderer; }
	ATHObjectManager*	GetObjectManager(){ return m_pObjectManager; }
	ATHScriptManager*   GetScriptManager(){ return m_pScriptManager; }
	ATHInputManager*	GetInputManager(){ return m_pInputManager; }
	ATHEventManager*	GetEventManager(){ return m_pEventManager; }
	ATHAudio*			GetAudioManager(){ return m_pAudioManager; }

	bool isFullScreen() { return m_bFullscreen; }
	bool isShutdown() { return m_bShutdown; }
	unsigned int GetScreenwidth() { return m_nScreenWidth; }
	unsigned int GetScreenheight() { return m_nScreenHeight; }
	float GetGameTime() { return m_fGameTime; }

#ifdef _WIN32
	void WindowsArgs(HINSTANCE _hInstance, WNDPROC _winProc, char* _szClassName, int _nCmdShow );
#endif

};

#endif
