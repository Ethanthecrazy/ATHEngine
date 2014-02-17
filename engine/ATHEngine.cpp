#include "ATHEngine.h"

#include "ATHUtil\FileUtil.h"
#include "ATHRenderer/ATHRenderer.h"
#include "ATHObjectSystem/ATHObjectManager.h"
#include "ATHScriptManager/ATHScriptManager.h"
#include "ATHInputManager/ATHInputManager.h"
#include "ATHEventSystem/ATHEventManager.h"
#include "ATHSoundSystem/ATHAudio.h"

// Test includes
#include "ATHObjectSystem\ATHObject.h"

#define CONFIG_FILE_PATH "config.xml"

ATHEngine::ATHEngine ()
{
	m_pRenderer = nullptr;
	m_pObjectManager = nullptr;
	m_pScriptManager = nullptr;
	m_pInputManager = nullptr;
	m_pEventManager = nullptr;
	m_pAudioManager = nullptr;

	m_bFullscreen = false;
	m_bVsync = false;
	m_bShutdown = true;

	m_nScreenWidth = 640;
	m_nScreenHeight = 480;

	m_fGameTime = 0.0f;

#ifdef _WIN32
	m_hWnd = 0;
	m_hInstance = 0;
	m_winProc = NULL;
	m_szClassName = nullptr;
	m_nCmdShow = 0;
#endif

}
//================================================================================
void ATHEngine::Init()
{
	// Load from config
	LoadConfig();

	// Create the operating system construct for the game
	if (!CreateViewport())
		return;

#ifdef _WIN32
	// Start the renderer
	m_pRenderer = ATHRenderer::GetInstance();
	m_pRenderer->Initialize(m_hWnd, m_hInstance, m_nScreenWidth, m_nScreenHeight, m_bFullscreen, m_bVsync);

	// Setup input management
	m_pInputManager = ATHInputManager::GetInstance();
	m_pInputManager->Init(m_hWnd, m_hInstance, m_nScreenWidth, m_nScreenWidth, m_nScreenWidth / 2, m_nScreenWidth / 2);

	// Start Audio Manager
	m_pAudioManager = ATHAudio::GetInstance();
	m_pAudioManager->InitXAudio2();
#endif // _WIN32

	// Create event manager
	m_pEventManager = ATHEventManager::GetInstance();

	// Setup Object manager
	m_pObjectManager = new ATHObjectManager();
	m_pObjectManager->Init();

	// Test init code
	TestInit();

	m_bShutdown = false;
}
//================================================================================
void ATHEngine::LoadConfig()
{
	char* szConfig = ATHGetFileAsText(CONFIG_FILE_PATH);
	if (!szConfig) return;

	rapidxml::xml_document<> ConfigDoc;
	ConfigDoc.parse<0>(szConfig);

	rapidxml::xml_node<>* nodeConfig = ConfigDoc.first_node();
	if (!nodeConfig)
	{
		delete szConfig;
		return;
	}

	rapidxml::xml_node<>* nodeGraphics = nodeConfig->first_node("Graphics");
	if (nodeGraphics)
	{
		rapidxml::xml_attribute<>* attrFullScreen = nodeGraphics->first_attribute("Fullscreen");
		if (attrFullScreen)
		{
			std::string strFullscreen = attrFullScreen->value();
			if (strcmp(strFullscreen.c_str(), "true") == 0)
				m_bFullscreen = true;
		}

		rapidxml::xml_attribute<>* attrVsync = nodeGraphics->first_attribute("Vsync");
		if (attrVsync)
		{
			std::string strVsync = attrVsync->value();
			if (strcmp(strVsync.c_str(), "true") == 0)
				m_bVsync = true;

		}

		rapidxml::xml_node<>* nodeResolution = nodeGraphics->first_node("Resolution");
		if (nodeResolution)
		{
			rapidxml::xml_attribute<>* attrResX = nodeResolution->first_attribute("X");
			rapidxml::xml_attribute<>* attrResY = nodeResolution->first_attribute("Y");

			if (attrResX && attrResY)
			{
				m_nScreenWidth = atoi(attrResX->value());
				m_nScreenHeight = atoi(attrResY->value());
			}
		}
	}

	delete szConfig;
}
//================================================================================
bool ATHEngine::CreateViewport()
{

#ifdef _WIN32
	return ViewportWindows();
#endif // _WIN32

}
//================================================================================
//================================================================================
#ifdef _WIN32
bool ATHEngine::ViewportWindows()
{
	WNDCLASSEX	winClassEx;	//	This will describe the window class we will create.

	//	First fill in the window class structure
	winClassEx.cbSize = sizeof(winClassEx);
	winClassEx.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winClassEx.lpfnWndProc = m_winProc;
	winClassEx.cbClsExtra = 0;
	winClassEx.cbWndExtra = 0;
	winClassEx.hInstance = m_hInstance;
	winClassEx.hIcon = LoadIcon(m_hInstance, IDC_APPSTARTING);
	winClassEx.hIconSm = NULL;
	winClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClassEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClassEx.lpszMenuName = NULL;
	winClassEx.lpszClassName = m_szClassName;

	//	Register the window class
	if (!RegisterClassEx(&winClassEx))
		return false;

	// Setup window style flags
	DWORD dwWindowStyleFlags = WS_VISIBLE;

	if (!m_bFullscreen)
	{
		dwWindowStyleFlags = WS_BORDER | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
	}
	else
	{
		dwWindowStyleFlags |= WS_POPUP;
		ShowCursor(FALSE);	// Stop showing the mouse cursor
	}

	// Setup the desired client area size
	RECT rWindow;
	rWindow.left = 0;
	rWindow.top = 0;
	rWindow.right = m_nScreenWidth;
	rWindow.bottom = m_nScreenHeight;

	// Get the dimensions of a window that will have a client rect that
	// will really be the resolution we're looking for.
	AdjustWindowRectEx(&rWindow,
		dwWindowStyleFlags,
		FALSE,
		WS_EX_APPWINDOW);

	// Calculate the width/height of that window's dimensions
	int nWindowWidth = rWindow.right - rWindow.left;
	int nWindowHeight = rWindow.bottom - rWindow.top;

	//	Create the window
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW,											//	Extended Style flags.
		m_szClassName,									//	Window Class Name.
		m_szClassName,											//	Title of the Window.
		dwWindowStyleFlags,										//	Window Style Flags.
		(GetSystemMetrics(SM_CXSCREEN) / 2) - (nWindowWidth / 2),		//	Window Start Point (x, y). 
		(GetSystemMetrics(SM_CYSCREEN) / 2) - (nWindowHeight / 2),	//		-Does the math to center the window over the desktop.
		nWindowWidth,												//	Width of Window.
		nWindowHeight,											//	Height of Window.
		NULL,														//	Handle to parent window.
		NULL,														//	Handle to menu.
		m_hInstance,												//	Application Instance.
		NULL);													//	Creation parameters.

	if (m_hWnd == nullptr)
		return false;

	ShowWindow(m_hWnd, m_nCmdShow);
	UpdateWindow(m_hWnd);

	return true;
}
#endif
//================================================================================
void ATHEngine::TestInit()
{
	ATHObject* pTemp = new ATHObject();
	m_pEventManager->RegisterClient(AET_KEYBOARD, pTemp);
	m_pObjectManager->AddObject(pTemp);

	m_pObjectManager->InstanceObject(float3(0.0f, 9.0f, 0.0f), "CircleTest");

	for (int i = 0; i < 10; ++i)
	{
		ATHObject* pNewObj = m_pObjectManager->InstanceObject(float3(0.0f, -100.0f - i * 10.0f, 0.0f), "TestObject");
		pNewObj->GetBody()->ApplyLinearImpulse(b2Vec2(0.0f, 200.0f), pNewObj->GetBody()->GetWorldCenter());
	}
}
//================================================================================
bool ATHEngine::Update(float _fDT)
{
	bool bReturn = true;

	m_fGameTime += _fDT;
	m_pInputManager->Update();

	// Camera movemnt test code //////
	m_pRenderer->GetCamera()->ViewTranslateLocalX(m_pInputManager->GetMouseDiffThisFrame().vX  * _fDT);
	m_pRenderer->GetCamera()->ViewTranslateLocalY(-m_pInputManager->GetMouseDiffThisFrame().vY  * _fDT);
	m_pRenderer->GetCamera()->ViewTranslateLocalZ(m_pInputManager->GetMouseDiffThisFrame().vZ * _fDT);
	//////////////////////////////////

	m_pObjectManager->Update(_fDT);
	m_pEventManager->ProcessEvents();

	m_pAudioManager->Update();

	return bReturn;
}
//================================================================================
void ATHEngine::Render()
{
	m_pRenderer->DRXBegin();

	m_pRenderer->CommitDraws();

	m_pRenderer->DRXEnd();

	m_pRenderer->DRXPresent();
}
//================================================================================
void ATHEngine::Shutdown()
{
	if (m_bShutdown)
		return;

	m_pAudioManager->ShutdownXAudio2();
	m_pAudioManager->DeleteInstance();

	m_pEventManager->Shutdown();
	m_pEventManager->DeleteInstance();

	m_pInputManager->Shutdown();
	m_pInputManager->DeleteInstance();

	m_pObjectManager->Shutdown();
	delete m_pObjectManager;

	m_pRenderer->Shutdown();
	m_pRenderer->DeleteInstance();

#ifdef _WIN32
	UnregisterClass(m_szClassName, m_hInstance);
#endif 

	m_bShutdown = true;

}
//================================================================================
#ifdef _WIN32
void ATHEngine::WindowsArgs(HINSTANCE _hInstance, WNDPROC _winProc, char* _szClassName, int _nCmdShow)
{
	m_hInstance = _hInstance;
	m_winProc = _winProc;
	m_szClassName = _szClassName;
	m_nCmdShow = _nCmdShow;
}
#endif 
//================================================================================