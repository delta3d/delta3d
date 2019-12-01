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
 * Bradley Anderegg
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtGame/gamestate.h>
#include <algorithm>



namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   // FUNCTOR CODE
   //////////////////////////////////////////////////////////////////////////
   struct funcExecuteCmds
   {
      template <typename _Type>
      void operator()(_Type& pCommand)
      {
         pCommand->operator()();
      }
   };



   //////////////////////////////////////////////////////////////////////////
   // STATE TYPE CODE
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(StateType)
   StateType StateType::STATE_UNKNOWN("STATE_UNKNOWN");
   StateType StateType::STATE_SPLASH("STATE_SPLASH");
   StateType StateType::STATE_MENU("STATE_MENU");
   StateType StateType::STATE_LOGIN("STATE_LOGIN");
   StateType StateType::STATE_INTRO("STATE_INTRO");
   StateType StateType::STATE_CUTSCENE("STATE_CUTSCENE");
   StateType StateType::STATE_LOADING("STATE_LOADING");
   StateType StateType::STATE_RUNNING("STATE_RUNNING");
   StateType StateType::STATE_SHUTDOWN("STATE_SHUTDOWN");


   //////////////////////////////////////////////////////////////////////////
   // EVENT TYPE CODE
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(EventType)
   EventType EventType::TRANSITION_OCCURRED("TRANSITION_OCCURRED");
   EventType EventType::TRANSITION_FORWARD("TRANSITION_FORWARD");
   EventType EventType::TRANSITION_BACK("TRANSITION_BACK");
   EventType EventType::TRANSITION_QUIT("TRANSITION_QUIT");
   EventType EventType::TRANSITION_GAME_OVER("TRANSITION_GAME_OVER");



   //////////////////////////////////////////////////////////////////////////
   // GAME STATE CODE
   //////////////////////////////////////////////////////////////////////////
   GameState::GameState(const StateType* pType)
      : mState(pType)
   {
      // Visual studio complains if this is in the initializer list, Warning:C4355.
      mOnUpdate = UpdateFunctor(this, &GameState::DefaultUpdateFunctor);
   }

   //////////////////////////////////////////////////////////////////////////
   GameState::~GameState()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   const StateType* GameState::GetType() const
   {
      return mState;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& GameState::GetName() const
   {
      return mState->GetName();
   }

   //////////////////////////////////////////////////////////////////////////
   // Executes the entry commands
   void GameState::OnEntry()
   {
      ExecuteCommands(mOnStart);
   }

   //////////////////////////////////////////////////////////////////////////
   // Executes the exit commands
   void GameState::OnExit()
   {
      ExecuteCommands(mOnFinish);
   }

   //////////////////////////////////////////////////////////////////////////
   void GameState::AddEntryCommand(CommandPtr pCommand)
   {
      mOnStart.push_back(pCommand);
   }

   //////////////////////////////////////////////////////////////////////////
   void GameState::AddExitCommand(CommandPtr pCommand)
   {
      mOnFinish.push_back(pCommand);
   }

   //////////////////////////////////////////////////////////////////////////
   void GameState::RemoveEntryCommand(CommandPtr pCommand)
   {
      mOnStart.remove(pCommand);
   }

   //////////////////////////////////////////////////////////////////////////
   void GameState::RemoveExitCommand(CommandPtr pCommand)
   {
      mOnFinish.remove(pCommand);
   }

   //////////////////////////////////////////////////////////////////////////
   void GameState::SetUpdate(const UpdateFunctor& pUpdate)
   {
      mOnUpdate = pUpdate;
   }

   //////////////////////////////////////////////////////////////////////////
   GameState::UpdateFunctor& GameState::GetUpdate()
   {
      return mOnUpdate;
   }

   //////////////////////////////////////////////////////////////////////////
   void GameState::ExecuteCommands(CommandList& pList)
   {
      // Execute all the commands.
      std::for_each(pList.begin(), pList.end(), funcExecuteCmds());   
   }

   //////////////////////////////////////////////////////////////////////////
   void GameState::DefaultUpdateFunctor(float dt)
   {
   }

}
