#include "ATHInputManager.h"

#include <iostream>
#include "../ATHEventSystem/ATHEventManager.h"

ATHInputManager* ATHInputManager::m_pInstance = nullptr;

ATHInputManager::ATHInputManager()
{
	m_hWnd = NULL;
	m_unScreenWidth = 0;
	m_unScreenHeight = 0;
	m_pInputInterface = nullptr;
	m_pKeyboard = nullptr;
	m_pMouse = nullptr;

	m_pEventManager = nullptr;

	memset( m_chKeyboardState, 0, sizeof( m_chKeyboardState ) );
	memset( m_chPrevKeyboardState, 0, sizeof( m_chPrevKeyboardState ) );

	memset( m_diMouseState.rgbButtons, 0,  ATH_MAX_MOUSE_BUTTONS * sizeof(BYTE) ); 
	memset (m_diPrevMouseState.rgbButtons, 0, ATH_MAX_MOUSE_BUTTONS * sizeof(BYTE) );

	m_fMouseDiffX = 0.0f;
	m_fMouseDiffY = 0.0f;
	m_fSensitivity = 1.0f;

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

	m_pEventManager = ATHEventManager::GetInstance();

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
	m_nMouseX = _nMouseStartX;
	m_nMouseY = _nMouseStartY;

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
		m_fMouseDiffX = lXMovement * m_fSensitivity;
		m_nMouseX += (int)(m_fMouseDiffX);

		LONG lYMovement = m_diMouseState.lY;
		m_fMouseDiffY = lYMovement * m_fSensitivity;
		m_nMouseY += (int)(m_fMouseDiffY);

		if( m_nMouseX < 0 )
			m_nMouseX = 0;

		if (m_nMouseY < 0 )
			m_nMouseY = 0;

		if( m_nMouseX > (int)m_unScreenWidth )
			m_nMouseX = m_unScreenWidth;

		if( m_nMouseY > (int)m_unScreenHeight )
			m_nMouseY = m_unScreenHeight;

		std::cout << m_nMouseX << " " << m_nMouseY << '\n';
	}

	SendKeyboardEvent();
	SendMouseEvent();

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
void ATHInputManager::SendKeyboardEvent()
{
	ATHKeyList m_liKeysDown = CheckKeys();

	ATHKeyList::iterator itrCurr = m_liKeysDown.begin();
	ATHKeyList::iterator itrEnd = m_liKeysDown.end();

	ATHEvent keyEvent( AET_KEYBOARD );
	unsigned int unKeyDownIndex = 0;
	unsigned int unKeyUpIndex = 0;
	while( itrCurr != itrEnd )
	{
		unsigned int szDIKKey = (*itrCurr);
		if (KeyPressed( szDIKKey ) && unKeyDownIndex < 8)
		{
			BYTE chAsciiKeys[ATH_NUM_KEYS] = {};
			if (GetKeyboardState(chAsciiKeys))  
			{
				unsigned short szAsciiKey = 0;
				// Why do I have to do both opposite conversions????
				int nCharCount = ToAsciiEx(MapVirtualKeyEx(szDIKKey, MAPVK_VSC_TO_VK, NULL), MapVirtualKeyEx(szDIKKey, MAPVK_VK_TO_VSC, NULL), chAsciiKeys, &szAsciiKey, 0, NULL);
				
				if (nCharCount > 0)
				{
					keyEvent.KEY_szKeysPressed[unKeyDownIndex] = (char)szAsciiKey;
					unKeyDownIndex++;
				}
			}
		}
	
		itrCurr++;
	}

	if( unKeyDownIndex > 0|| unKeyUpIndex > 0 )
	{
		keyEvent.m_EventID = AEI_KEYDOWN;
		m_pEventManager->SendEvent( keyEvent, AEP_IMMEDIATE );
	}
}
//================================================================================
void ATHInputManager::SendMouseEvent()
{
	ATHKeyList m_liButtonsDown = CheckMouseButtons();
	ATHKeyList::iterator itrCurr = m_liButtonsDown.begin();
	ATHKeyList::iterator itrEnd = m_liButtonsDown.end();

	ATHEvent mouseEvent( AET_MOUSE );
	unsigned int unButtonDownIndex = 0;

	while( itrCurr != itrEnd )
	{
		char szButton;
		if( MouseButtonPressed( szButton = (*itrCurr ) ) && unButtonDownIndex < 8 )
		{
			mouseEvent.MSE_szMouseButtonsDown[ unButtonDownIndex ] = szButton;
			unButtonDownIndex++;
		}

		itrCurr++;
	}

	if( unButtonDownIndex > 0 )
	{
		mouseEvent.MSE_unPosX = (unsigned int)m_nMouseX;
		mouseEvent.MSE_unPosY = (unsigned int)m_nMouseY;
		mouseEvent.m_EventID = AEI_MOUSEDOWN;

		m_pEventManager->SendEvent( mouseEvent, AEP_IMMEDIATE );
	}
}
//================================================================================
bool ATHInputManager::KeyState(unsigned char _chDIKButton )
{
	return (m_chKeyboardState[_chDIKButton] & 0x80) ? true : false;
}
//================================================================================
bool ATHInputManager::KeyPressed(unsigned char _chDIKButton )
{
	return KeyState(_chDIKButton) && !((m_chPrevKeyboardState[_chDIKButton] & 0x80));
}
//================================================================================
bool ATHInputManager::KeyReleased(unsigned char _chDIKButton )
{
	return !KeyState(_chDIKButton) && ((m_chPrevKeyboardState[_chDIKButton] & 0x80));
}
//================================================================================
ATHKeyList ATHInputManager::CheckKeys()
{
	std::list< unsigned int > liKeysDown;

	BYTE chAsciiKeys[ ATH_NUM_KEYS ] = {};
	if( GetKeyboardState( chAsciiKeys ) )
	{
		for(unsigned int i = 0; i < ATH_NUM_KEYS; i++)
		{
			if( KeyState( (unsigned char)(i) ) )
			{
				unsigned int nVKCode = MapVirtualKeyEx(i, MAPVK_VSC_TO_VK, NULL );

				liKeysDown.push_back(i);
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
ATHKeyList ATHInputManager::CheckMouseButtons()
{
	std::list< unsigned int > liButtonsDown;
	
	for( unsigned int i = 0; i < ATH_MAX_MOUSE_BUTTONS; ++i )
	{
		if( MouseButtonState( (unsigned char)(i) ) )
		{
			liButtonsDown.push_back( i );
		}
	}

	return liButtonsDown;
}
//================================================================================
float2 ATHInputManager::GetMousePos()
{
	return float2(float(m_nMouseX), float(m_nMouseY));
}
//================================================================================
float2 ATHInputManager::GetMouseDiffThisFrame()
{
	return float2( m_fMouseDiffX, m_fMouseDiffY );
}
//================================================================================