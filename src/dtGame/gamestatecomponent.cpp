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
#include <dtGame/gamestatecomponent.h>
#include <dtABC/application.h>
#include <dtCore/project.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>
#include <dtGame/transitionxmlhandler.h>
#include <dtUtil/fileutils.h>


namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   // COMPONENT CODE
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   GameStateComponent::GameStateComponent(dtCore::SystemComponentType& type)
      : BaseClass(type)
      , mCurrentState(NULL)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   GameStateComponent::~GameStateComponent()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void GameStateComponent::Update(float dt)
   {
      if(mCurrentState.valid())
      {
         mCurrentState->GetUpdate()(dt);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GameStateComponent::ProcessMessage(const dtGame::Message &message)
   {
      const dtGame::MessageType& messageType = message.GetMessageType();

      if( messageType == dtGame::MessageType::TICK_LOCAL)
      {
         Update(float(static_cast<const dtGame::TickMessage&>(message).GetDeltaSimTime()));
      }
      else if( messageType == dtGame::MessageType::REQUEST_GAME_STATE_TRANSITION)
      {
         DoStateTransition( &(static_cast<const GameStateTransitionRequestMessage&>(message).GetTransition()) );
      }
      else if( messageType == dtGame::MessageType::INFO_GAME_STATE_CHANGED)
      {
         ProcessStateChanged( static_cast<const GameStateChangedMessage&>(message) );
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GameStateComponent::ProcessStateChanged( const GameStateChangedMessage& stateChange )
   {
      if( stateChange.GetNewState() == GameState::Type::STATE_SHUTDOWN )
      {
         GetGameManager()->GetApplication().Quit();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GameStateComponent::SendGameStateChangedMessage(const GameState::Type& oldState, const GameState::Type& newState)
   {
      dtCore::RefPtr<GameStateChangedMessage> gscm;
      GetGameManager()->GetMessageFactory().CreateMessage(MessageType::INFO_GAME_STATE_CHANGED, gscm);
      gscm->SetOldState(oldState);
      gscm->SetNewState(newState);
      LOG_INFO("Changing game state to: " + newState.GetName());
      GetGameManager()->SendMessage(*gscm);
   }

   //////////////////////////////////////////////////////////////////////////
   void GameStateComponent::SetInitialState(const StateType* pState)
   {
      mCurrentState = GetState(pState);
   }

   //////////////////////////////////////////////////////////////////////////
   const StateType* GameStateComponent::GetCurrentState() const
   {
      if(mCurrentState.valid())
      {
         return mCurrentState->GetType();
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   GameState* GameStateComponent::GetState(const StateType* pState)
   {
      GameStateSet::iterator iter = mStates.find(pState);
      if(iter != mStates.end())
      {
         return (*iter).second.get();
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void GameStateComponent::MakeCurrent(const StateType* pState)
   {
      GameState* state = GetState(pState);
      if(state == NULL)
      {
         LOG_ERROR("No GameState object found for StateType '" + pState->GetName() + ".'");
      }
      else
      {
         OnStateChange(state);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool GameStateComponent::DoStateTransition(const std::string& pEvent)
   {
      EventType* et = EventType::GetValueForName(pEvent);
      if(et != NULL)
      {
         return DoStateTransition(et);
      }
      else
      {
         LOG_ERROR("Undefined event type for '" + pEvent + "'.");
         return false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool GameStateComponent::DoStateTransition(const EventType* pEvent)
   {
      if(pEvent == NULL)
      {
         LOG_ERROR("NULL is invalid parameter to function!");
         return false;
      }

      TransitionMap::key_type key(pEvent, mCurrentState.get());
      TransitionMap::iterator iter = mTransitions.find( key );

      if(iter != mTransitions.end())
      {
         GameState* to = (*iter).second.get();
         OnStateChange(to);
         return true;
      }

      LOG_ERROR( "Unable to handle event: " + pEvent->GetName() + " from state: "
         + (mCurrentState.valid() ? mCurrentState->GetName() : "NULL") );
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void GameStateComponent::OnStateChange(GameState* pState)
   {
      if(pState == NULL)
      {
         LOG_ERROR("NULL is invalid parameter to function!");
      }
      else
      {
         if(mCurrentState.valid())
         {
            mCurrentState->OnExit();
         }

         const StateType* from = mCurrentState->GetType();

         mCurrentState = pState;
         mCurrentState->OnEntry();

         const StateType* to = pState->GetType();
         SendGameStateChangedMessage(*from, *to);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   GameState* GameStateComponent::AddState(const StateType* pStateType)
   {
      GameState* newState = NULL;

      if(pStateType == NULL)
      {
         LOG_ERROR("NULL is an invalid argument!");
      }
      else
      {
         newState = GetState(pStateType);
         if(newState == NULL)
         {
            newState = new GameState(pStateType);
            mStates.insert(std::make_pair(pStateType, newState));
         }
      }

      return newState;
   }

   //////////////////////////////////////////////////////////////////////////
   void GameStateComponent::AddTransition(const EventType* eventType, const StateType* fromState, const StateType* toState)
   {
      //lazy state addition
      GameState* realFrom = AddState(fromState);
      GameState* realTo = AddState(toState);

      if(realFrom != NULL && realTo != NULL)
      {
         // checking the transition map's keys
         TransitionMap::key_type key(eventType, realFrom);
         mTransitions.insert(TransitionMap::value_type(key, realTo));
      }
      else
      {
         LOG_ERROR("Unable to add transition- Event:"
            + eventType->GetName() + ", from state: " + fromState->GetName() +
            ", to state: " + toState->GetName());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool GameStateComponent::RemoveTransition(const EventType* eventType, const StateType* from, const StateType* to)
   {
      if( !eventType || !from || !to )
      {
         return false;
      }

      GameState* fromState = GetState(from);
      GameState* toState = GetState(to);

      if(fromState != NULL && toState != NULL)
      {
         // Returns true if any elements were removed from the EventMap
         TransitionMap::key_type key( eventType, fromState);

         // if key is in map...
         TransitionMap::iterator iter( mTransitions.find(key) );
         if( iter != mTransitions.end() )
         {
            //and if key maps to "to"
            if( iter->second.get() == toState )
               return mTransitions.erase(key) > 0;
         }
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GameStateComponent::IsInState(const GameStateType& state) const
   {
      const GameStateType* currentState = GetCurrentState();
      return currentState != NULL && state == *currentState;
   }

   //////////////////////////////////////////////////////////////////////////
   bool GameStateComponent::LoadTransitions(const std::string& filePath)
   {
      if(dtUtil::FileUtils::GetInstance().FileExists(filePath))
      {
         TransitionXMLHandler handler(this);
         dtUtil::XercesParser parser;
         return parser.Parse(filePath, handler);
      }
      else
      {
         LOG_ERROR("Unable to find file '" + filePath + "'.")
         return false;
      }
   }

}

