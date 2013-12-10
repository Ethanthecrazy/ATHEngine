#ifndef ATHSCRIPTMANAGER_H
#define ATHSCRIPTMANAGER_H

extern "C" {
#include "lua\lua.h"
#include "lua\lualib.h"
#include "lua\lauxlib.h"
}

class ATHScriptManager
{

};

int ATHLUATest()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs( L );

	luaL_dofile( L, "data/test.lua" );

	lua_getglobal( L, "add" );
	lua_pushnumber( L, 9 );
	lua_pushnumber( L, 63 );

	lua_call( L, 2, 1 );

	int nSum = (int)lua_tointeger( L, -1 );
	lua_pop( L, 1 );

	lua_close( L );

	return nSum;

}

#endif