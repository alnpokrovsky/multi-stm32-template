#ifndef __CONTROLS_MILUA_H__
#define __CONTROLS_MILUA_H__

#ifndef LUA_32BITS
#error "lua library wasn't included"
#endif

#include <lua.h>

void milua_init(void);

void milua_execLine(const char * str);

void milua_addFunc(char * name, lua_CFunction f);

#endif
