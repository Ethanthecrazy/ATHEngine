#ifndef ATHINPUTMANAGER_H
#define ATHINPUTMANAGER_H

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <list>


#define KEYBOARD_BUFFER_SIZE 1
static const int ATH_NUM_KEYS = 256;
static const int ATH_MAX_MOUSE_BUTTONS = 4;

class ATHInputManager
{
private:

	static ATHInputManager* m_pInstance;
	HWND m_hWnd;

	unsigned int m_unScreenWidth;
	unsigned int m_unScreenHeight;

	IDirectInput8* m_pInputInterface;
	IDirectInputDevice8* m_pKeyboard;
	IDirectInputDevice8* m_pMouse;

	// Keyboard
	BYTE m_chKeyboardState[ATH_NUM_KEYS];
	BYTE m_chPrevKeyboardState[ATH_NUM_KEYS];


	// Mouse
	DIMOUSESTATE m_diMouseState;
	DIMOUSESTATE m_diPrevMouseState;
	int m_nMouseX, m_nMouseY;
	float m_fSensitivity;

	// This is a singleton
	ATHInputManager();
	ATHInputManager(const ATHInputManager&);
	ATHInputManager& operator=(const ATHInputManager&);
	~ATHInputManager();

public:

	static ATHInputManager* GetInstance();
	static void DeleteInstance();

	bool Init( HWND _hWnd, HINSTANCE _hInstance, unsigned int _unScreenWidth, unsigned int _unScreenHeight, int _nMouseStartX = 0, int _nMouseStartY = 0 );
	void Shutdown();
	void SethWnd( HWND _hWnd );
	void SetScreenDimensions( unsigned int _unScreenWidth, unsigned int _unScreenHeight );

	unsigned int Update();
	bool ReadKeyboard();
	bool ReadMouse();

	bool KeyState(unsigned char _chButton );
	bool KeyPressed(unsigned char _chButton );				// Down this frame, up last frame
	bool KeyReleased(unsigned char _chButton );				// Up this frame, down last frame
	std::list< char > CheckKeys();

	bool MouseButtonState( unsigned char _chButton );
	bool MouseButtonPressed( unsigned char _chButton );		// Down this frame, up last frame
	bool MouseButtonReleased( unsigned char _chButton );	// Up this frame, down last frame
	std::list< char > CheckMouseButtons();

	



};

#endif