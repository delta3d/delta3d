/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef DELTA_TEST_APP_GAME_STATES
#define DELTA_TEST_APP_GAME_STATES

/* This file contains examples on how to extend
 * game state and transition types.
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtGame/gamestate.h>



namespace dtExample
{
   //////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   //////////////////////////////////////////////////////////////////////////
   typedef dtGame::StateType GameStateType;
   class TestAppGameState : public GameStateType
   {
      public:
         typedef GameStateType BaseClass;

         /* Base States
         static StateType STATE_UNKNOWN;
         static StateType STATE_SPLASH;
         static StateType STATE_MENU;
         static StateType STATE_LOGIN;
         static StateType STATE_INTRO;
         static StateType STATE_CUTSCENE;
         static StateType STATE_LOADING;
         static StateType STATE_RUNNING;
         static StateType STATE_SHUTDOWN;*/
         static TestAppGameState STATE_GAME;
         static TestAppGameState STATE_GAME_OPTIONS;
         static TestAppGameState STATE_GAME_QUIT;
         static TestAppGameState STATE_TITLE;

         TestAppGameState(const std::string& name, bool paused = false)
            : BaseClass(name,paused)
         {}

      protected:
         virtual ~TestAppGameState()
         {}
   };




   //////////////////////////////////////////////////////////////////////////
   // TRANSITION CODE
   //////////////////////////////////////////////////////////////////////////
   typedef dtGame::EventType Transition;
   class TestAppTransition : public Transition
   {
      public:
         typedef Transition BaseClass;

         /* Base Transition Types
         static EventType TRANSITION_FORWARD;
         static EventType TRANSITION_BACK;
         static EventType TRANSITION_QUIT;
         static EventType TRANSITION_GAME_OVER;*/
         static TestAppTransition TRANSITION_MENU; // Use when connection fails anywhere during a connected state.

         TestAppTransition( const std::string& name )
            : BaseClass(name)
         {}

      protected:
         virtual ~TestAppTransition()
         {}
   };

}

#endif
