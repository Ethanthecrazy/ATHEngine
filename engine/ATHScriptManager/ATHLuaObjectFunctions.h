#ifndef ATHLUAOBJECTFUNCTIONS_H
#define ATHLUAOBJECTFUNCTIONS_H

class ATHObject;
struct lua_State;

static void SetObject( ATHObject* _pCurrObject );

static int Object_SetAlive(lua_State *L);
static int Object_GetAlive(lua_State *L);

static int Object_SetActive(lua_State *L);
static int Object_GetActive(lua_State *L);

#endif