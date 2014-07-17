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
#include <dtGame/gamestatemessages.h>



namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   // GAME STATE CHANGED MESSAGE
   //////////////////////////////////////////////////////////////////////////
   const std::string GameStateChangedMessage::PARAMETER_STATE_OLD("OldState");
   const std::string GameStateChangedMessage::PARAMETER_STATE_NEW("NewState");

   //////////////////////////////////////////////////////////////////////////
   GameStateChangedMessage::GameStateChangedMessage()
      : mOldParam(*new dtGame::EnumMessageParameter(PARAMETER_STATE_OLD, GameState::Type::STATE_UNKNOWN.GetName()))
      , mNewParam(*new dtGame::EnumMessageParameter(PARAMETER_STATE_NEW, GameState::Type::STATE_UNKNOWN.GetName()))
   {
      AddParameter(&mOldParam);
      AddParameter(&mNewParam);
   }

   //////////////////////////////////////////////////////////////////////////
   GameStateChangedMessage::~GameStateChangedMessage()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   GameState::Type& GameStateChangedMessage::GetOldState() const
   {
      return *GameState::Type::GetValueForName(mOldParam.GetValue());
   }

   //////////////////////////////////////////////////////////////////////////
   void GameStateChangedMessage::SetOldState(const GameState::Type &oldState)
   {
      mOldParam.SetValue(oldState.GetName());
   }

   //////////////////////////////////////////////////////////////////////////
   GameState::Type& GameStateChangedMessage::GetNewState() const 
   {
      return *GameState::Type::GetValueForName(mNewParam.GetValue());
   }

   //////////////////////////////////////////////////////////////////////////
   void GameStateChangedMessage::SetNewState(const GameState::Type &newState)
   {
      mNewParam.SetValue(newState.GetName());
   }



   //////////////////////////////////////////////////////////////////////////
   // GAME STATE TRANSITION REQUEST MESSAGE
   //////////////////////////////////////////////////////////////////////////
   const std::string GameStateTransitionRequestMessage::PARAMETER_TRANSITION("Transition");
   
   //////////////////////////////////////////////////////////////////////////
   GameStateTransitionRequestMessage::GameStateTransitionRequestMessage()
      : mTransition(*new dtGame::EnumMessageParameter(PARAMETER_TRANSITION, dtGame::EventType::TRANSITION_OCCURRED.GetName()))
   {
      AddParameter(&mTransition);
   }

   //////////////////////////////////////////////////////////////////////////
   GameStateTransitionRequestMessage::~GameStateTransitionRequestMessage()
   {}

   //////////////////////////////////////////////////////////////////////////
   dtGame::EventType& GameStateTransitionRequestMessage::GetTransition() const 
   {
      return *dtGame::EventType::GetValueForName(mTransition.GetValue());
   }

   //////////////////////////////////////////////////////////////////////////
   void GameStateTransitionRequestMessage::SetTransition(const dtGame::EventType &transition)
   {
      mTransition.SetValue(transition.GetName());
   }

}
