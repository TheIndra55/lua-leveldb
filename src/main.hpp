#include <lua.hpp>
#include <leveldb/db.h>

#define LUA_LEVELDB_CHECKSTATUS(L, fmt, status)  \
	if (!status.ok()) return luaL_error(L, fmt, status.ToString().c_str());