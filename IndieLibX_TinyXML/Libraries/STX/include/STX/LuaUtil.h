/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef _LUAUTIL_
#define _LUAUTIL_
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}
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
class LuaScript {
public:
#if 1
    LuaScript();
	bool Load(const std::string& filename);
	//bool LoadBuff(const std::string& filename);
#else
	LuaScript(const std::string& filename);
#endif
    ~LuaScript();
    void printError(const std::string& variableName, const std::string& reason);
    std::vector<int> getIntVector(const std::string& name);
    std::vector<std::string> getTableKeys(const std::string& name);
    //std::vector<std::string> getTableValues(const std::string& name);
    
    inline void clean() {
      int n = lua_gettop(L);
      lua_pop(L, n);
    }

    template<typename T>
    T get(const std::string& variableName) {
      if(!L) {
        printError(variableName, "Script is not loaded");
        return lua_getdefault<T>();
      }
      
      T result;
      if(lua_gettostack(variableName, true)) { // variable succesfully on top of stack
        result = lua_get<T>(variableName);  
      } else {
        result = lua_getdefault<T>();
      }

     
      clean();
      return result;
    }

    bool lua_gettostack(const std::string& variableName, bool silent=true) {
      level = 0;
      std::string var = "";
        for(unsigned int i = 0; i < variableName.size(); i++) {
          if(variableName.at(i) == '.') {
            if(level == 0) {
              lua_getglobal(L, var.c_str());
            } else {
              lua_getfield(L, -1, var.c_str());
            }
            
            if(lua_isnil(L, -1)) {
              if(!silent) printError(variableName, var + " is not defined");
              return false;
            } else {
              var = "";
              level++;
            }
          } else {
            var += variableName.at(i);
          }
        }
        if(level == 0) {
          lua_getglobal(L, var.c_str());
        } else {
          lua_getfield(L, -1, var.c_str());
        }
        if(lua_isnil(L, -1)) {
            if(!silent) printError(variableName, var + " is not defined");
            return false;
        }

        return true;       
    }

    // Generic get
    template<typename T>
    T lua_get(const std::string& variableName) {
      return 0;
    }

    template<typename T>
    T lua_getdefault() {
      return 0;
    }
   
public:
    lua_State* L;
private:
    std::string filename;
    int level;
};

 // Specializations

template <> 
inline bool LuaScript::lua_get<bool>(const std::string& variableName) {
    return (bool)lua_toboolean(L, -1);
}

template <> 
inline float LuaScript::lua_get<float>(const std::string& variableName) {
    if(!lua_isnumber(L, -1)) {
      printError(variableName, "Not a number");
    }
    return (float)lua_tonumber(L, -1);
}

template <>
inline int LuaScript::lua_get<int>(const std::string& variableName) {
    if(!lua_isnumber(L, -1)) {
      printError(variableName, "Not a number");
    }
    return (int)lua_tonumber(L, -1);
}

template <>
inline std::string LuaScript::lua_get<std::string>(const std::string& variableName) {
    std::string s = "null";
    if(lua_isstring(L, -1)) {
      s = std::string(lua_tostring(L, -1));
    } else {
      printError(variableName, "Not a string");
    }
    return s;
}

template<>
inline std::string LuaScript::lua_getdefault<std::string>() {
  return "null";
}

namespace    LuaUtil
{
class   ILua
{

public:

    ILua()
    {
    };

    virtual ~ILua()
    {
    };
    virtual lua_State    *State(void) = 0;
    virtual void        DoFile(const char *afile) = 0;
};
class   SLua
{

public:
    static lua_State *state;

public:
    static lua_State *State(void);
    static void     DoFile(const char *afile);
};
class Lua :
    public ILua,
    SLua
{

public:
    Lua();
    ~Lua();

    lua_State *State(void)
    {
        return SLua::State();
    }

    void DoFile(const char *afile)
    {
        SLua::DoFile(afile);
    }
};
}
#endif

