#include "ATHScriptManager.h"


int ATHObjectLuaTest()
{
	ATHScriptManager TestManager;
	TestManager.LoadScriptFromFile( "data/test.lua" );

	TestManager.RegisterFunc( average, "average" );

	return TestManager.RunFunc( "AverageTest", 2, 0, 20 );
}
//================================================================================
ATHScriptManager::ATHScriptManager()
{
	m_pLuaState = luaL_newstate();
	luaL_openlibs( m_pLuaState );
	m_pLastObject = nullptr;
}
//================================================================================
ATHScriptManager::~ATHScriptManager()
{
	lua_close( m_pLuaState );
}
//================================================================================
int ATHScriptManager::LoadScriptFromFile( char* _szPath )
{
	return luaL_dofile( m_pLuaState, _szPath );
}
//================================================================================
void ATHScriptManager::RegisterFunc( lua_CFunction _pFunc, char* _szFuncName )
{
	lua_register( m_pLuaState, _szFuncName, _pFunc );
}
//================================================================================
int ATHScriptManager::RunFunc( char* _szName, unsigned int _unNumArgs, ... )
{
	// Setup standard variables
	// Expose Global Functions
	// Load the function on to the stack
	lua_getglobal( m_pLuaState, _szName );

	// Pass in the variables
	va_list arguments;
	va_start( arguments, _unNumArgs );
	for( unsigned int i = 0; i < _unNumArgs; ++i )
	{
		lua_pushnumber( m_pLuaState, va_arg( arguments, int ) );
	}
	va_end( arguments );

	// Call the function
	lua_call( m_pLuaState, _unNumArgs, 0 );

	// Get the return value
	int toReturn = (int)lua_tointeger( m_pLuaState, 0 );

	// Clean up the stack 
	lua_pop( m_pLuaState, 1 );

	return toReturn;
}
//================================================================================
int ATHScriptManager::RunFuncObjectScope( char* _szName, ATHObject* _pObject, unsigned int _unNumArgs, ... )
{
	// Setup standard variables
	// Expose Global Functions
	// Cleanup Object properties if necessary
	// Setup Object Variables
	// Expose Object Functions
	// Load the function on to the stack
	lua_getglobal( m_pLuaState, _szName );

	// Pass in the variables
	// Call the function
	lua_call( m_pLuaState, _unNumArgs, 1 );

	// Copy back the changed properties
	// Get the return value
	int toReturn = (int)lua_tointeger( m_pLuaState, -1 );
	lua_pop( m_pLuaState, 1 );

	return toReturn;
}
//================================================================================