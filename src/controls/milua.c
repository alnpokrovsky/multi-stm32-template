#ifdef LUA_32BITS

#include "milua.h"
#include "usb_cdc.h"
#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "minmax.h"

#define BUFFER_SIZE 128
static lua_State *L;

void milua_init(void) {
    usb_core_init();
    L = luaL_newstate();    /* Create Lua state variable */
    luaL_openlibs(L);       /* Load Lua libraries */
}

void milua_addFunc(char * name, lua_CFunction f) {
	lua_register(L, name, f);
}

void milua_execLine(const char * str) {
	if (luaL_dostring(L, str) != 0) {
		printf("%s\n", lua_tostring(L,-1)); //error
	}
}

void usb_cdc_rx_handler(char * buf, uint16_t len)
{
	static char bufRx[BUFFER_SIZE];
	static uint16_t bufLen = 0;

	usb_cdc_tx(buf, len);

	for (int i = 0; i < len; ++i) {
		switch (buf[i]) {
		case '\n':
		case '\r':
			bufRx[bufLen] = '\0';
			if (bufLen > 0) milua_execLine(bufRx);
			bufLen = 0;
			break;
		case '\b':
			if (bufLen > 0) bufLen--;
			break;
		default:
			bufRx[bufLen++] = buf[i];
			break;
		}
	}
}

////////////////////////////////////////////////////////


/// Called by libc stdio fwrite functions
int _write(int fd, char *ptr, int len);
int _write(int fd, char *ptr, int len)
{
	// Only work for STDOUT, STDIN, and STDERR
	if (fd > 2) return -1;

	uint16_t txlen = MIN((size_t)len, strlen(ptr));
	for (int i = txlen; i > 0; i -= usb_cdc_tx(ptr, i)) ;
	return txlen;
}

// /*
//  * Called by the libc stdio fread fucntions
//  * Implements a buffered read with line editing.
//  */
// int _read(int fd, char *ptr, int len);
// int _read(int fd, char *ptr, int len)
// {
// 	if (fd > 2) return -1;

// 	uint16_t rxlen = MIN((size_t)len, strlen(BUFFER_RX));
// 	strncpy(ptr, BUFFER_RX, rxlen); // copy to input
// 	strncpy(&ptr[0], &ptr[rxlen], BUFFER_SIZE-rxlen); // shift buffer
// 	return rxlen; /* return the length we got */
// }

#endif
