// luainterpreter.h: Lua interpreter declarations.
//
//////////////////////////////////////////////////////////////////////

#ifndef P51_LUA_INTERPRETER
#define P51_LUA_INTERPRETER

#include "base.h"

extern "C"
{
   #include <lua.h>
}

namespace P51
{
   /**
    * Provides access to the Lua interpreter.
    */
   class LuaInterpreter : public Base
   {
      DECLARE_MANAGEMENT_LAYER(LuaInterpreter)


      public:

         /**
          * Constructor.
          *
          * @param name the name of the instance
          */
         LuaInterpreter(std::string name = "LuaInterpreter");

         /**
          * Destructor.
          */
         virtual ~LuaInterpreter();

         /**
          * Runs in an interactive loop, interpreting statements
          * entered by the user on the standard input device.
          */
         void RunInteractiveLoop();


      private:

         /**
          * The Lua state.
          */
         lua_State* mState;
   };
}

#endif // P51_LUA_INTERPRETER