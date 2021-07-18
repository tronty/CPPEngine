#include <stdio.h>
#include <STX/STX.h>
#include <STX/LuaUtil.h>

lua_State* __L__=0;
extern int luaopen_example(lua_State* __L__); // declare the wrapped module

int main(int argc,char* argv[])
{
    std::string filename2=stx_convertpath(argv[0]);
    __L__ = luaL_newstate();
    if (luaL_loadfile(__L__, argv[0]) || lua_pcall(__L__, 0, 0, 0)) {
        std::cout<<"Error: failed to load ("<<argv[0]<<")"<<std::endl;
		__L__ = 0;
    }
    if(__L__) luaL_openlibs(__L__);
    //if(__L__) return false;
 luaopen_IndieLib(__L__);	// load the wrappered module
 if (luaL_loadfile(__L__,argv[1])==0) // load and run the file
  lua_pcall(__L__,0,0,0);
 else
  printf("unable to load %s\n",argv[1]);
 lua_close(__L__);
 return 0;
}
