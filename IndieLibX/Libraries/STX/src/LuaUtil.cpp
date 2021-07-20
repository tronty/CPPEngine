/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <STX/LuaUtil.h>

lua_State *LuaUtil::SLua::state = 0;

LuaUtil::Lua::Lua() :
    LuaUtil::Lua::ILua()
{
	if(!state)
	{
    state = luaL_newstate();                        /* Create Lua state variable */
    luaL_openlibs(state);                           /* Load Lua libraries */
	}
}

LuaUtil::Lua::~Lua()
{
}

lua_State *LuaUtil::SLua::State(void)
{
    return state;
}

void LuaUtil::SLua::DoFile(const char *afile)
{
    std::string file=stx_convertpath(afile);
    if (luaL_loadfile(state, file.c_str()))    /* Load but don't run the Lua script */
	LOG_PRINT("luaL_loadfile(%s) failed\n", file.c_str());  
}
#if 0
#include <lua.h>                                /* Always include this when calling Lua */
#include <lauxlib.h>                            /* Always include this when calling Lua */
#include <lualib.h>                             /* Always include this when calling Lua */

#include <stdlib.h>                             /* For function stx_exit() */
#include <stdio.h>                              /* For input/output */

void bail(lua_State *L, char *msg){
	LOG_PRINT("\nFATAL ERROR:\n  %s: %s\n\n",
		msg, lua_tostring(L, -1));
	stx_exit(1);
}

int main(void)
{
    lua_State *L;

    L = luaL_newstate();                        /* Create Lua state variable */
    luaL_openlibs(L);                           /* Load Lua libraries */

    if (luaL_loadfile(L, "helloscript.lua"))    /* Load but don't run the Lua script */
	bail(L, "luaL_loadfile() failed");      /* Error out if file can't be read */

    LOG_PRINT("In C, calling Lua\n");

    if (lua_pcall(L, 0, 0, 0))                  /* Run the loaded Lua script */
	bail(L, "lua_pcall() failed");          /* Error out if Lua file has an error */

    LOG_PRINT("Back in C again\n");

    lua_close(L);                               /* Clean up, free the Lua state var */

    return 0;
}
#endif

/*
unnamed_lua_binder
==================

Simple lua binding with C++ I'm currently using for my game.

**Tutorial**

**Part1**: http://eliasdaler.wordpress.com/2013/10/11/lua_cpp_binder/

**Part2**: http://eliasdaler.wordpress.com/2013/10/20/lua_and_cpp_pt2/

**Russian version of tutorial**

**Part1**: http://habrahabr.ru/post/197300/

**Part2**: In progress

This code uses zlib license. 

Copyright (C) 1995-2013 Elias Daler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#if 1
LuaScript::LuaScript(){}
bool LuaScript::Load(const std::string& filename) {
    std::string filename2=stx_convertpath(filename);
    L = luaL_newstate();
    if (luaL_loadfile(L, filename2.c_str()) || lua_pcall(L, 0, 0, 0)) {
        std::cout<<"Error: failed to load ("<<filename<<")"<<std::endl;
		L = 0;
    }

    if(L) luaL_openlibs(L);
    if(L) return false;
    return true;
}
#if 0
bool LuaScript::LoadBuff(const std::string& filename) {
    std::string filename2=stx_convertpath(filename);
    L = luaL_newstate();

    std::string buff;
    STX_Service::LoadTxtFile(filename2.c_str(), buff);
    luaL_loadbuffer(L, buff.c_str(), buff.length(), "line");

    if(L) luaL_openlibs(L);
    if(L) return false;
    return true;
}
#endif
#else
LuaScript::LuaScript(const std::string& filename) {
    std::string filename2=stx_convertpath(filename);
    L = luaL_newstate();
    if (luaL_loadfile(L, filename2.c_str()) || lua_pcall(L, 0, 0, 0)) {
        std::cout<<"Error: failed to load ("<<filename<<")"<<std::endl;
		L = 0;
    }

    if(L) luaL_openlibs(L);
}
#endif
LuaScript::~LuaScript() {
	if(L) lua_close(L);
}

void LuaScript::printError(const std::string& variableName, const std::string& reason) {
	std::cout<<"Error: can't get ["<<variableName<<"]. "<<reason<<std::endl;
}

std::vector<int> LuaScript::getIntVector(const std::string& name) {
    std::vector<int> v;
    lua_gettostack(name.c_str(), true);
    if(lua_isnil(L, -1)) { // array is not found
        return std::vector<int>();
    }
    lua_pushnil(L);
    while(lua_next(L, -2)) {
        v.push_back((int)lua_tonumber(L, -1));
        lua_pop(L, 1);
    }
    clean();
    return v;
}
#if 0
std::vector<std::string> LuaScript::getTableValues(const std::string& name) {
    std::string code = 
        "function getValues(name) "
        "s = \"\""
        "for k, v in pairs(_G[name]) do "
        "    s = s..v..\",\" "
        "    end "
        "return s "
        "end"; // function for getting table keys
    luaL_loadstring(L, 
        code.c_str()); // execute code
    lua_pcall(L,0,0,0);
    lua_getglobal(L, "getValues"); // get function
    lua_pushstring(L, name.c_str());
    lua_pcall(L, 1 , 1, 0); // execute function
    std::string test = lua_tostring(L, -1);
    std::vector<std::string> strings;
    std::string temp = "";
    std::cout<<"TEMP:"<<test<<std::endl;
    for(unsigned int i = 0; i < test.size(); i++) {     
        if(test.at(i) != ',') {
            temp += test.at(i);
        } else {
            strings.push_back(temp);
            temp= "";
        }
    }
    clean();
    return strings;
}
#endif
std::vector<std::string> LuaScript::getTableKeys(const std::string& name) {
    std::string code = 
        "function getKeys(name) "
        "s = \"\""
        "for k, v in pairs(_G[name]) do "
        "    s = s..k..\",\" "
        "    end "
        "return s "
        "end"; // function for getting table keys
    luaL_loadstring(L, 
        code.c_str()); // execute code
    lua_pcall(L,0,0,0);
    lua_getglobal(L, "getKeys"); // get function
    lua_pushstring(L, name.c_str());
    lua_pcall(L, 1 , 1, 0); // execute function
    std::string test = lua_tostring(L, -1);
    std::vector<std::string> strings;
    std::string temp = "";
    std::cout<<"TEMP:"<<test<<std::endl;
    for(unsigned int i = 0; i < test.size(); i++) {     
        if(test.at(i) != ',') {
            temp += test.at(i);
        } else {
            strings.push_back(temp);
            temp= "";
        }
    }
    clean();
    return strings;
}

