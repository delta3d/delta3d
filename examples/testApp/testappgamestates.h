/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2009, Alion Science and Technology, BMH Operation
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
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
