#ifndef ATHSCRIPTMANAGER_H
#define ATHSCRIPTMANAGER_H

extern "C" {
#include "lua\lua.h"
#include "lua\lualib.h"
#include "lua\lauxlib.h"
}

class ATHObject;
class ATHScriptManager
{
private:

	lua_State* m_pLuaState;
	ATHObject* m_pLastObject;

public:

	ATHScriptManager();
	~ATHScriptManager();

	int LoadScriptFromFile( char* _szPath );
	
	// Runs a function
	int RunFunc( char* _szName, unsigned int _unNumArgs, ... );

	// Runs a function after loading the objects data into the scope
	int RunFuncObjectScope( char* _szName, ATHObject* _pObject, unsigned int _unNumArgs, ... );
};

int ATHObjectLuaTest();


#endif