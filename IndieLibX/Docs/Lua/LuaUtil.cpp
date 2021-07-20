#include "LuaUtil.h"
LuaState *LuaUtil::SLua::   state = 0;

LuaUtil::Lua::Lua() :
    LuaUtil::Lua::ILua()
{
    state = LuaState::Create();
}

LuaUtil::Lua::~Lua()
{
}

LuaState *LuaUtil::SLua::State(void)
{
    return state;
}

void LuaUtil::SLua::DoFile(const char *afile)
{
    state->DoFile(afile);
}

