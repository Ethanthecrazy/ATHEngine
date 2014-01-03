#include "ATHInputManager.h"

#include <iostream>

ATHInputManager* ATHInputManager::m_pInstance = nullptr;

ATHInputManager::ATHInputManager()
{
	m_hWnd = NULL;
	m_unScreenWidth = 0;
	m_unScreenHeight = 0;
	m_pInputInterface = nullptr;
	m_pKeyboard = nullptr;
	m_pMouse = nullptr;

	memset( m_chKeyboardState, 0, sizeof( m_chKeyboardState ) );
	memset( m_chPrevKeyboardState, 0, sizeof( m_chPrevKeyboardState ) );

	memset( m_diMouseState.rgbButtons, 0,  ATH_MAX_MOUSE_BUTTONS * sizeof(BYTE) ); 
	memset (m_diPrevMouseState.rgbButtons, 0, ATH_MAX_MOUSE_BUTTONS * sizeof(BYTE) );

	m_nMouseX = 0;
	m_nMouseY = 0;
	float m_fSensitivity = 1.0f;

}
//================================================================================
ATHInputManager::~ATHInputManager()
{

}
//================================================================================
ATHInputManager* ATHInputManager::GetInstance()
{
	if( !m_pInstance )
	{
		m_pInstance = new ATHInputManager();
	}

	return m_pInstance;
}
//================================================================================
void ATHInputManager::DeleteInstance()
{
	if( m_pInstance )
	{
		delete m_pInstance;
	}

	m_pInstance = nullptr;
}
//================================================================================
bool ATHInputManager::Init( HWND _hWnd, HINSTANCE _hInstance, unsigned int _unScreenWidth, unsigned int _unScreenHeight, int _nMouseStartX, int _nMouseStartY )
{
	HRESULT hr;

	m_hWnd = _hWnd;
	m_unScreenWidth = _unScreenWidth;
	m_unScreenHeight = _unScreenHeight;

	// Init the core interface
	hr = DirectInput8Create( _hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputInterface, NULL );
	if( FAILED( hr ) )
		return false;
	
	// Clear the key registers
	ZeroMemory( m_chKeyboardState, ATH_NUM_KEYS );
	ZeroMemory( m_chPrevKeyboardState, ATH_NUM_KEYS );

	// Init the keyboard interface
	hr = m_pInputInterface->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL);
	if(FAILED(hr))
		return false;

	// Set keyboard to data mode
	hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
		return false;

	// Make the keyboard exclusive
	hr = m_pKeyboard->SetCooperativeLevel( _hWnd, DISCL_FOREGROUND|DISCL_EXCLUSIVE );
	if(FAILED(hr))
		return false;

	m_pKeyboard->Acquire();

	// Init the mouse interface
	hr = m_pInputInterface->CreateDevice( GUID_SysMouse, &m_pMouse, NULL );
	if(FAILED(hr))
		return false;

	// Set mouse to data mode
	hr = m_pMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(hr))
		return false;

	// Make the mouse exclusive
	hr = m_pMouse->SetCooperativeLevel( _hWnd, DISCL_FOREGROUND|DISCL_EXCLUSIVE );
	if(FAILED(hr))
		return false;

	m_pMouse->Acquire();

	SetCursorPos( _nMouseStartX, _nMouseStartY );

	return true;
}
//================================================================================
void ATHInputManager::Shutdown()
{
	// Release the mouse
	if (m_pMouse )
	{
		m_pMouse->Unacquire();
		m_pMouse->Release();
		m_pMouse = NULL;
	}

	// Release the keyboard
	if( m_pKeyboard )
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
		m_pKeyboard = NULL;
	}

	// Release the main interface to direct input
	if( m_pInputInterface )
	{
		m_pInputInterface->Release();
		m_pInputInterface = NULL;
	}
}
//================================================================================
void ATHInputManager::SethWnd( HWND _hWnd )
{
	m_hWnd = _hWnd;
}
//================================================================================
void ATHInputManager::SetScreenDimensions( unsigned int _unScreenWidth, unsigned int _unScreenHeight )
{
	m_unScreenWidth = _unScreenWidth;
	m_unScreenHeight = _unScreenHeight;
}
//================================================================================
unsigned int ATHInputManager::Update()
{
	ReadKeyboard();

	if( ReadMouse() )
	{
		LONG lXMovement = m_diMouseState.lX;
		m_nMouseX += (int)(lXMovement * m_fSensitivity);

		LONG lYMovement = m_diMouseState.lY;
		m_nMouseY += (int)(lYMovement * m_fSensitivity);

		if( m_nMouseX < 0 )
			m_nMouseX = 0;

		if (m_nMouseY < 0 )
			m_nMouseY = 0;

		if( m_nMouseX > (int)m_unScreenWidth )
			m_nMouseX = m_unScreenWidth;

		if( m_nMouseY > (int)m_unScreenHeight )
			m_nMouseY = m_unScreenHeight;

	}

	return 0;
}
//================================================================================
bool ATHInputManager::ReadKeyboard()
{
	HRESULT hr;

	// Copy the previous frame
	memcpy(m_chPrevKeyboardState, m_chKeyboardState, sizeof(m_chPrevKeyboardState));

	// Read the keyboard
	hr = m_pKeyboard->GetDeviceState(sizeof(m_chKeyboardState), (LPVOID)&m_chKeyboardState);

	if(FAILED(hr))
	{
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			m_pKeyboard->Acquire();

		return false;
	}

	return true;
}
//================================================================================
bool ATHInputManager::ReadMouse()
{
	HRESULT hr;

	// Copy the previous frame
	memcpy(&m_diPrevMouseState, &m_diMouseState, sizeof(m_diPrevMouseState));

	// Read the mouse
	hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_diMouseState);
	if(FAILED(hr))
	{
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			m_pMouse->Acquire();

		return false;
	}

	return true;
}
//================================================================================
bool ATHInputManager::KeyState(unsigned char _chButton )
{
	return ( m_chKeyboardState[ _chButton ] & 0x80 ) ? true : false;
}
//================================================================================
bool ATHInputManager::KeyPressed(unsigned char _chButton )
{
	return KeyState(_chButton) && !(m_chPrevKeyboardState[_chButton] & 0x80);
}
//================================================================================
bool ATHInputManager::KeyReleased(unsigned char _chButton )
{
	return !KeyState(_chButton) && (m_chPrevKeyboardState[_chButton] & 0x80);
}
//================================================================================
std::list< char > ATHInputManager::CheckKeys()
{
	std::list< char > liKeysDown;

	BYTE chAsciiKeys[ ATH_NUM_KEYS ] = {};
	if( GetKeyboardState( chAsciiKeys ) )
	{
		for(unsigned int i = 0; i < ATH_NUM_KEYS; i++)
		{
			if( KeyState( (unsigned char)(i) ) )
			{
				unsigned int nVKCode = MapVirtualKeyEx(i, 1, NULL );
				unsigned short sNum = 0;

				ToAsciiEx( nVKCode, i, chAsciiKeys, &sNum, 0, NULL );

				liKeysDown.push_back( (char)sNum ); 
			}
		}
	}

	return liKeysDown;
}
//================================================================================
bool ATHInputManager::MouseButtonState( unsigned char _chButton )
{
	return (m_diMouseState.rgbButtons[_chButton] & 0x80) ? true : false;
}
//================================================================================
bool ATHInputManager::MouseButtonPressed( unsigned char _chButton )
{
	return (m_diMouseState.rgbButtons[_chButton] & 0x80) && !(m_diPrevMouseState.rgbButtons[_chButton] & 0x80);
}
//================================================================================
bool ATHInputManager::MouseButtonReleased( unsigned char _chButton )
{
	return !(m_diMouseState.rgbButtons[_chButton] & 0x80) && (m_diPrevMouseState.rgbButtons[_chButton] & 0x80);
}
//================================================================================
std::list< char > ATHInputManager::CheckMouseButtons()
{
	std::list< char > liButtonsDown;
	
	for( unsigned int i = 0; i < ATH_MAX_MOUSE_BUTTONS; ++i )
	{
		if( MouseButtonState( (unsigned char)(i) ) )
		{
			liButtonsDown.push_back( (char)i );
		}
	}

	return liButtonsDown;
}
//================================================================================