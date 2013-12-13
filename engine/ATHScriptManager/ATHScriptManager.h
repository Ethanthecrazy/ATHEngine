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
	void RegisterFunc( lua_CFunction _pFunc , char* _szFuncName );

	// Runs a function
	int RunFunc( char* _szName, unsigned int _unNumArgs, ... );

	// Runs a function after loading the objects data into the scope
	int RunFuncObjectScope( char* _szName, ATHObject* _pObject, unsigned int _unNumArgs, ... );


};

static int average(lua_State *L)
{
		/* get number of arguments */
	int n = lua_gettop(L);
	double sum = 0;
	int i;

	/* loop through each argument */
	for (i = 1; i <= n; i++)
	{
		/* total the arguments */
		sum += lua_tonumber(L, i);
	}

	/* push the average */
	lua_pushnumber(L, sum / n);

	/* return the number of results */
	return 1;
}

#endif