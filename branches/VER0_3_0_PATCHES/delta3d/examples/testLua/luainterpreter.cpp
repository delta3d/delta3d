// luainterpreter.cpp: Lua interpreter implementation.
//
//////////////////////////////////////////////////////////////////////

#include "luainterpreter.h"

#include "base.h"
#include "transform.h"
#include "transformable.h"
#include "object.h"

#include <signal.h>

extern "C"
{
   #include <lualib.h>
   #include <lauxlib.h>
}

#include <luabind/luabind.hpp>

using namespace std;
using namespace P51;
using namespace luabind;


IMPLEMENT_MANAGEMENT_LAYER(LuaInterpreter)


static Object* GetObjectInstance(string name)
{
   return Object::GetInstance(name);
}

static void SetTransform(Transformable* self, Transform* transform)
{
   self->SetTransform(transform);
}

/**
 * Constructor.
 *
 * @param name the name of the instance
 */
LuaInterpreter::LuaInterpreter(string name)
   : Base(name)
{
   RegisterInstance(this);

   // Create a new Lua state
   mState = lua_open();

   // Load the standard libraries
   luaopen_base(mState);
   luaopen_table(mState);
   luaopen_io(mState);
   luaopen_string(mState);
   luaopen_math(mState);
   luaopen_debug(mState);
   luaopen_loadlib(mState);

   // Initialize Luabind
   luabind::open(mState);

   void (Transform::*TransformST)(float, float, float) = &Transform::SetTranslation;
   void (Transform::*TransformSR)(float, float, float) = &Transform::SetRotation;

   // Initialize the Lua bindings

   module(mState, "P51")
   [
      def("GetObjectInstance", &GetObjectInstance),
      class_<Base>("Base"),
      class_<Transform>("Transform")
         .def(constructor<>())
         .def("SetTranslation", TransformST)
         .def("SetRotation", TransformSR),
      class_<Transformable, Base>("Transformable")
         .def("SetTransform", &SetTransform),
      class_<Object, Transformable>("Object")
   ];
}

/**
 * Destructor.
 */
LuaInterpreter::~LuaInterpreter()
{
   lua_close(mState);

   DeregisterInstance(this);
}


/******************************************************************************
* The following static functions are from the Lua distribution.  Please refer
* to the closing copyright notice.
******************************************************************************/

#ifndef PROMPT
#define PROMPT		"> "
#endif

#ifndef PROMPT2
#define PROMPT2		">> "
#endif

static void lstop (lua_State *l, lua_Debug *ar) {
  (void)ar;  /* unused arg. */
  lua_sethook(l, NULL, 0, 0);
  luaL_error(l, "interrupted!");
}

static void laction (int i) {
  signal(i, SIG_DFL); /* if another SIGINT happens before lstop,
                              terminate process (default action) */
  //lua_sethook(L, lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}

static void l_message (const char *pname, const char *msg) {
  if (pname) fprintf(stderr, "%s: ", pname);
  fprintf(stderr, "%s\n", msg);
}

static int report (lua_State* L, int status) {
  const char *msg;
  if (status) {
    msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error with no message)";
    l_message(NULL, msg);
    lua_pop(L, 1);
  }
  return status;
}

static int lcall (lua_State* L, int narg, int clear) {
  int status;
  int base = lua_gettop(L) - narg;  /* function index */
  lua_pushliteral(L, "_TRACEBACK");
  lua_rawget(L, LUA_GLOBALSINDEX);  /* get traceback function */
  lua_insert(L, base);  /* put it under chunk and args */
  signal(SIGINT, laction);
  status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
  signal(SIGINT, SIG_DFL);
  lua_remove(L, base);  /* remove traceback function */
  return status;
}

static const char *get_prompt (lua_State* L, int firstline) {
  const char *p = NULL;
  lua_pushstring(L, firstline ? "_PROMPT" : "_PROMPT2");
  lua_rawget(L, LUA_GLOBALSINDEX);
  p = lua_tostring(L, -1);
  if (p == NULL) p = (firstline ? PROMPT : PROMPT2);
  lua_pop(L, 1);  /* remove global */
  return p;
}

static int incomplete (lua_State* L, int status) {
  if (status == LUA_ERRSYNTAX &&
         strstr(lua_tostring(L, -1), "near `<eof>'") != NULL) {
    lua_pop(L, 1);
    return 1;
  }
  else
    return 0;
}

/*
** this macro can be used by some `history' system to save lines
** read in manual input
*/
#ifndef lua_saveline
#define lua_saveline(L,line)	/* empty */
#endif

/*
** this macro defines a function to show the prompt and reads the
** next line for manual input
*/
#ifndef lua_readline
#define lua_readline(L,prompt)		readline(L,prompt)
#endif

/* maximum length of an input line */
#ifndef MAXINPUT
#define MAXINPUT	512
#endif

static int readline (lua_State *l, const char *prompt) {
  static char buffer[MAXINPUT];
  if (prompt) {
    fputs(prompt, stdout);
    fflush(stdout);
  }
  if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    return 0;  /* read fails */
  else {
    lua_pushstring(l, buffer);
    return 1;
  }
}

static int load_string (lua_State* L) {
  int status;
  lua_settop(L, 0);
  if (lua_readline(L, get_prompt(L, 1)) == 0)  /* no input? */
    return -1;
  if (lua_tostring(L, -1)[0] == '=') {  /* line starts with `=' ? */
    lua_pushfstring(L, "return %s", lua_tostring(L, -1)+1);/* `=' -> `return' */
    lua_remove(L, -2);  /* remove original line */
  }
  for (;;) {  /* repeat until gets a complete line */
    status = luaL_loadbuffer(L, lua_tostring(L, 1), lua_strlen(L, 1), "=stdin");
    if (!incomplete(L, status)) break;  /* cannot try to add lines? */
    if (lua_readline(L, get_prompt(L, 0)) == 0)  /* no more input? */
      return -1;
    lua_concat(L, lua_gettop(L));  /* join lines */
  }
  lua_saveline(L, lua_tostring(L, 1));
  lua_remove(L, 1);  /* remove line */
  return status;
}

static void manual_input (lua_State* L) {
  int status;
  while ((status = load_string(L)) != -1) {
    if (status == 0) status = lcall(L, 0, 0);
    report(L, status);
    if (status == 0 && lua_gettop(L) > 0) {  /* any result to print? */
      lua_getglobal(L, "print");
      lua_insert(L, 1);
      if (lua_pcall(L, lua_gettop(L)-1, 0, 0) != 0)
        l_message(NULL, lua_pushfstring(L, "error calling `print' (%s)",
                                               lua_tostring(L, -1)));
    }
  }
  lua_settop(L, 0);  /* clear stack */
  fputs("\n", stdout);
}

/******************************************************************************
* Copyright (C) 1994-2003 Tecgraf, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/


/**
 * Runs in an interactive loop, interpreting statements
 * entered by the user on the standard input device.
 */
void LuaInterpreter::RunInteractiveLoop()
{
   manual_input(mState);
}


