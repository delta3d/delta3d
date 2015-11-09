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

#ifndef DELTA_GAME_STATE_COMPONENT_H
#define DELTA_GAME_STATE_COMPONENT_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtGame/export.h>
#include <dtGame/gamestate.h>
#include <dtGame/gamestatemessages.h>
#include <dtGame/gmcomponent.h>



namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   // COMPONENT CODE
   //////////////////////////////////////////////////////////////////////////
   class DT_GAME_EXPORT GameStateComponent : public dtGame::GMComponent
   {
      public:
         typedef dtGame::GMComponent BaseClass;

         static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;
         static const std::string DEFAULT_NAME;

         typedef dtCore::RefPtr<GameState> GameStatePtr;
         typedef std::map<const StateType*, GameStatePtr> GameStateSet;
         typedef std::pair<const EventType*, GameStatePtr> EventStatePtrPair;
         typedef std::map<EventStatePtrPair, GameStatePtr> TransitionMap;

         GameStateComponent(dtCore::SystemComponentType& type = *TYPE );

         /**
          * Handles incoming messages
          */
         virtual void ProcessMessage(const dtGame::Message& message);

         /**
          * Handles logic dependent on certain states.
          */
         virtual void ProcessStateChanged( const GameStateChangedMessage& stateChange );

         bool LoadTransitions(const std::string& filePath);

         void SetInitialState(const StateType* stateType);

         const StateType* GetCurrentState() const;
         GameState* GetState(const StateType* stateType);

         /** Forces the given State to now be the 'current' State.*/
         void MakeCurrent(const StateType* stateType);

         /**
          * Pass all events through this function
          * @return whether or not the event caused a transition
          */
         bool DoStateTransition(const EventType* eventType);
         bool DoStateTransition(const std::string& eventName);

         GameState* AddState(const StateType* stateType);
         void AddTransition(const EventType* transitionEvent, const StateType* fromState, const StateType* toState);
         bool RemoveTransition(const EventType* eventType, const StateType* from, const StateType* to);

         bool IsInState(const GameStateType& state) const;

      protected:
         virtual ~GameStateComponent();

         void Update(float);
         void OnStateChange(GameState*);
         void SendGameStateChangedMessage(const GameState::Type& oldState, const GameState::Type& newState);

      private:
         dtCore::RefPtr<GameState> mCurrentState;
         GameStateSet mStates;
         TransitionMap mTransitions;
    };

}

#endif
