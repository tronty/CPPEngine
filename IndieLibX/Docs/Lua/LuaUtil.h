#ifndef _LUAUTIL_
#define _LUAUTIL_
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;
#include "Libraries/lua-5.3.1/src/LuaPlus.h"
using namespace LuaPlus;
extern "C"
{
#include "Libraries/lua-5.3.1/src/lua.h"
#include "Libraries/lua-5.3.1/src/lualib.h"
#include "Libraries/lua-5.3.1/src/lauxlib.h"
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
    virtual LuaState    *State(void) = 0;
    virtual void        DoFile(const char *afile) = 0;
};
class   SLua
{

public:
    static LuaState *state;

public:
    static LuaState *State(void);
    static void     DoFile(const char *afile);
};
class Lua :
    public ILua,
    SLua
{

public:
    Lua();
    ~Lua();

    LuaState *State(void)
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

