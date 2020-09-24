#include <shlwapi.h>

#include "main.hpp"

leveldb::DB* db;

LUALIB_API int leveldb_open(lua_State* L)
{
    // windows specific apis
    char path[MAX_PATH];
    char dir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, dir);

    PathCombineA(path, dir, luaL_checkstring(L, 1));

    leveldb::Options options;
    options.create_if_missing = true;

    auto status = leveldb::DB::Open(options, path, &db);
    if (!status.ok())
    {
        return luaL_error(L, "Failed to open database: %s", status.ToString().c_str());
    }

    return 1;
}

LUALIB_API int leveldb_set(lua_State* L)
{
    auto key = luaL_checkstring(L, 1);

    auto status = db->Put(leveldb::WriteOptions(), key, luaL_checkstring(L, 2));

    LUA_LEVELDB_CHECKSTATUS(L, "Failed to set: %s", status);

    return 1;
}

LUALIB_API int leveldb_get(lua_State* L)
{
    auto key = luaL_checkstring(L, 1);
    std::string value;

    auto status = db->Get(leveldb::ReadOptions(), key, &value);
    if (status.IsNotFound())
    {
        // return nil if key not found
        lua_pushnil(L);
        return 1;
    }

    LUA_LEVELDB_CHECKSTATUS(L, "Failed to get: %s", status);

    lua_pushstring(L, value.c_str());

    return 1;
}

LUALIB_API int leveldb_delete(lua_State* L)
{
    auto key = luaL_checkstring(L, 1);

    auto status = db->Delete(leveldb::WriteOptions(), key);
    if (!status.ok() && !status.IsNotFound())
    {
        return luaL_error(L, "Failed to delete: %s", status.ToString().c_str());
    }

    return 1;
}

LUALIB_API int leveldb_find(lua_State* L)
{
    auto prefix = luaL_checkstring(L, 1);
    auto iterator = db->NewIterator(leveldb::ReadOptions());

    lua_newtable(L);
    for (iterator->SeekToFirst(); iterator->Valid(); iterator->Next())
    {
        if (iterator->key().starts_with(prefix))
        {
            lua_pushstring(L, iterator->key().ToString().c_str());
            lua_pushstring(L, iterator->value().ToString().c_str());
            lua_settable(L, -3);
        }
    }

    delete iterator;

    return 1;
}

static const struct luaL_Reg functions[] = {
    {"open", leveldb_open},
    {"set", leveldb_set},
    {"get", leveldb_get},
    {"delete", leveldb_delete},
    {"find", leveldb_find},
    {NULL, NULL}
};

extern "C" __declspec(dllexport) int luaopen_leveldb(lua_State* L)
{
    luaL_newlib(L, functions);
    return 1;
}