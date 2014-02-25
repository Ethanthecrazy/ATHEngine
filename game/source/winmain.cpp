//////////////////////////////////////////////////////////////////////////////////////////////
//	File			:	"WinMain.cpp"
//
//	Author			:	Ethan pendergraft
//	Based in part on:
//		-The previous WinMain.cpp by David Brown.
//
//	Last Modified	:	8/26/2011
//
//	Purpose			:	To provide a basic window framework.
//
//
//			-----~<] Prayer of Program [>~-----
//
//	I am the heart of my functions.
//	Ciphers are my body, and calculation is my blood.
//	I have written over ten-million lines.
//	Unknown to termination.
//	Nor known to initialization.
//	I have withstood many errors to compile many programs.
//	Yet, these addresses will never hold anything.
//	So as I pray, 'Unlimited Code-Works'!
//
//			----------~<]=========[>~----------

#include <windows.h>	//	Needed for Windows Applications.

#include <iostream>
#include <fstream>
#include <ctime>

#include "CGame.h"

#ifdef _DEBUG
#pragma comment( linker, "/SUBSYSTEM:Console" )
#endif

char* g_szWINDOW_CLASS_NAME	= "Scrapmaster";				//	Window Class Name.

const char* g_szWINDOW_TITLE		= "Scrapmaster";				//	Window Title.
const int	g_nWINDOW_WIDTH			= 1280;//1024;						//	Window Width.
const int	g_nWINDOW_HEIGHT		= 720;//768;						//	Window Height.

const int g_nMEMORY_SIZE			= 268435456;

#ifdef _DEBUG
	const BOOL	g_bIS_WINDOWED			= true;	
#else
	const BOOL	g_bIS_WINDOWED			= false;	
#endif


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#endif


LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//	This is the main message handler of the system.
	PAINTSTRUCT	ps;			//	Used in WM_PAINT.
	HDC			hdc;		//	Handle to a device context.

	//	What is the message 
	switch(msg)
	{
		//	To skip ALT pop up menu (system menu)
		case WM_SYSKEYUP:
		case WM_SYSCHAR:
			return(0);
		break;
		
		//	Handle ALT+F4
		case WM_CLOSE:
		{
			// Sends us a WM_DESTROY
			CGame::GetInstance()->Shutdown();
			DestroyWindow(hWnd);			
		}
		break;

		//	and lose/gain focus
		case WM_ACTIVATE:
		{
			//	gaining focus
			if (LOWORD(wParam) != WA_INACTIVE)
			{
				// unpause game code here
				SetForegroundWindow(hWnd);
			}
			else // losing focus
			{
				SetForegroundWindow(hWnd);
				// pause game code here
			}
		}
		break;

		case WM_CREATE: 
		{
			//	Do initialization here
			return(0);
		}
		break;

		case WM_PAINT:
		{
			//	Start painting
			hdc = BeginPaint(hWnd,&ps);

			//	End painting
			EndPaint(hWnd,&ps);
			return(0);
		}
		break;

		case WM_DESTROY: 
		{
			//	Kill the application
			PostQuitMessage(0);
			return(0);
		}
		break;

		default:
		break;
	}

	//	Process any messages that we didn't take care of 
	return (DefWindowProc(hWnd, msg, wParam, lParam));
}

//	Checks to see if the game was already running in another window.
//
//	NOTE:	Don't call this function if your game needs to have more
//			than one instance running on the same computer (i.e. client/server)
BOOL CheckIfAlreadyRunning(void)
{
	//	Find a window of the same window class name and window title
	HWND hWnd = FindWindow(g_szWINDOW_CLASS_NAME, g_szWINDOW_TITLE);

	//	If one was found
	if (hWnd)
	{
		//	If it was minimized
		if (IsIconic(hWnd))
			//	restore it
			ShowWindow(hWnd, SW_RESTORE);

		//	Bring it to the front
		SetForegroundWindow(hWnd);

		return TRUE;
	}

	//	No other copies found running
	return FALSE;
}

//////////////////////////
//		WinMain			//
//////////////////////////

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG		msg;	//	Generic message.

#ifdef _DEBUG

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// this function call will set a breakpoint at the location of a leaked block
	// set the parameter to the identifier for a leaked block
	//_CrtSetBreakAlloc(1440);

#endif

	// Don't let more than one instance of the application exist
	//
	// NOTE:	Comment out the following section of code if your game needs to have more
	//			than one instance running on the same computer (i.e. client/server)
	////////////////////////////////////////////////////////////////////////
	//if (!hPrevInstance)
	{
		//if (CheckIfAlreadyRunning())
		{
			//hPrevInstance = hInstance;
			//return FALSE;
		}
	}
	////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////
	//	Game Initialization
	//////////////////////////////////////////


	//MemoryManager::GetInstance()->Init( g_nMEMORY_SIZE );
	CGame* theGame = CGame::GetInstance();
	theGame->m_Engine.WindowsArgs(hInstance, WindowProc, g_szWINDOW_CLASS_NAME, nCmdShow);
	theGame->Initialize();

	//////////////////////////////////////////

	//	Enter main event loop
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{ 
			//	Test if this is a quit
			if (msg.message == WM_QUIT)
			{
				theGame->Shutdown();
				break;
			}
		
			//	Translate any accelerator keys
			TranslateMessage(&msg);

			//	Send the message to the window proc
			DispatchMessage(&msg);
		}
		
		//////////////////////////////////
		//	Put Game Logic Here
		//////////////////////////////////
		
		if ( !theGame->Main() )
			PostQuitMessage(0);
		
		//////////////////////////////////
	}

	/////////////////////////////////////////
	//	Shutdown Game Here
	/////////////////////////////////////////
	
	theGame->Shutdown();

	//MemoryManager::GetInstance()->DebugString();
	//MemoryManager::FreeInstance();


	/////////////////////////////////////////
	
	//	Return to Windows like this.
	return (int)(msg.wParam);
}

int main(void)
{
	int toReturn = WinMain(GetModuleHandle(NULL), NULL,
		GetCommandLine(), SW_SHOWDEFAULT);

	return toReturn;
}