#include "ATHLuaObjectFunctions.h"
#include "../Objects/ATHObject.h"

extern "C" {
#include "lua\lua.h"
#include "lua\lualib.h"
#include "lua\lauxlib.h"
}

ATHObject* g_pCurrObject = nullptr;

static void SetObject( ATHObject* _pCurrObject )
{
	g_pCurrObject = _pCurrObject;
}
//================================================================================
static int Object_SetAlive(lua_State *L)
{
	if( !g_pCurrObject )
		return 0;
	int nArgCount = lua_gettop( L );

	if( nArgCount > 0 ) // Use the equivalence to cast as a bool
		g_pCurrObject->SetAlive( lua_toboolean( L, 1 ) != 0 );

	return 0;
}
//================================================================================
static int Object_GetAlive(lua_State *L)
{
	if( !g_pCurrObject )
		return 0;
	
	lua_pushboolean( L, g_pCurrObject->GetAlive() );

	return 1;
}
//================================================================================
static int Object_SetActive(lua_State *L)
{
	if( !g_pCurrObject )
		return 0;
	int nArgCount = lua_gettop( L );

	if( nArgCount > 0 )
		g_pCurrObject->SetActive( lua_toboolean( L, 1 ) != 0 );

	return 0;

}
//================================================================================
static int Object_GetActive(lua_State *L)
{
	if( !g_pCurrObject )
		return 0;

	lua_pushboolean( L, g_pCurrObject->GetActive() );

	return 1;
}
//================================================================================