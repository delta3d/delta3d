/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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
 * Bradley Anderegg 03/20/2006
 */

#include <dtAI/fsm.h>
#include <dtUtil/log.h>

namespace dtAI
{
   //////////////////////////////////////////////////////////////////////////
   FSM::FSM()
      : mCurrentState(new NPCState(&NPCStateTypes::NPC_STATE_DEFAULT))
      , mFactory(0)
      , mProcessSelfTransitions(true)
   {
      SetupDefaultFactory();
   }

   //////////////////////////////////////////////////////////////////////////
   FSM::FSM(FactoryType* pFactory)
      : mCurrentState(new NPCState(&NPCStateTypes::NPC_STATE_DEFAULT))
      , mFactory(pFactory)
      , mProcessSelfTransitions(true)
   {

   }

   //////////////////////////////////////////////////////////////////////////
   FSM::~FSM()
   {
      FreeMem();
   }


   //////////////////////////////////////////////////////////////////////////
   void FSM::FreeMem()
   {
      mCurrentState = 0;
      mStates.clear();
      mTransitions.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   NPCState* FSM::AddState(const NPCState::Type* state)
   {
      NPCState* newState = NULL;

      if (mFactory.valid())
      {
         //for default creation we have to add the type to the object factory
         if (!mFactory->IsTypeSupported(state->GetName()))
         {
            mFactory->RegisterType<NPCState>(state->GetName());
         }

         newState = mFactory->CreateObject(state->GetName());
         newState->SetType(state);
         mStates.insert(newState);
      }
      else
      {
         LOG_ERROR("ObjectFactory has not been initialized, returning NULL pointer.");
      }

      return newState;
   }

   //////////////////////////////////////////////////////////////////////////
   //Release note: lazy state addition removed
   void FSM::AddTransition(const NPCEvent* eventType, const NPCState::Type* from, const NPCState::Type* to)
   {
      //lazy state addition
      //AddState(from);
      //AddState(to);

      // checking the set of States
      NPCState* realFrom = GetState(from);
      NPCState* realTo = GetState(to);

      if (realFrom != NULL && realTo != NULL)
      {
         // checking the transition map's keys
         TransitionMap::key_type key(eventType, realFrom);
         mTransitions.insert(TransitionMap::value_type(key , realTo));
      }
      else
      {
         LOG_ERROR("Unable to add transition- Event:"
            + eventType->GetName() + ", from state: " + from->GetName() +
            ", to state: " + to->GetName());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   NPCState* FSM::GetCurrentState()
   {
      return mCurrentState.get();
   }

   //////////////////////////////////////////////////////////////////////////
   NPCState* FSM::GetState(const NPCState::Type* pStateType)
   {
      StateSet::iterator iter      = mStates.begin();
      StateSet::iterator endOfList = mStates.end();

      while (iter != endOfList)
      {
         if ((*iter)->GetName() == pStateType->GetName())
         {
            return const_cast<NPCState*>((*iter).get());
         }
         ++iter;
      }

      LOG_ERROR("State Type: " + pStateType->GetName() + " not found.");
      return 0;
   }

   //////////////////////////////////////////////////////////////////////////
   /** Forces the given State to now be the 'current' State.*/
   void FSM::MakeCurrent(const NPCState::Type* pStateType)
   {
      OnStateChange(GetState(pStateType));
   }

   //////////////////////////////////////////////////////////////////////////
   void FSM::OnStateChange(NPCState* pState)
   {
      if (pState == NULL)
      {
         LOG_ERROR("FSM::OnStateChange, Invalid State: NULL");
         return;
      }

      //if it's a new state or we're handling transitions to the same state
      if (mCurrentState != pState || mProcessSelfTransitions == true)
      {
         mCurrentState->OnExit();
         mCurrentState = pState;
         mCurrentState->OnEntry();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void FSM::Update(double dt)
   {
      if (mCurrentState.valid() && mCurrentState->GetUpdate().valid())
      {
         mCurrentState->GetUpdate()(dt);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool FSM::HandleEvent(const NPCEvent* pEvent)
   {
      TransitionMap::key_type key(pEvent, mCurrentState.get());
      TransitionMap::iterator iter = mTransitions.find(key);

      if (iter != mTransitions.end())
      {
         NPCState* to = (*iter).second.get();
         OnStateChange(to);
         return true;
      }

      LOG_ERROR("Unable to handle event: " + pEvent->GetName() + " from state: " + mCurrentState->GetName());
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void FSM::SetupDefaultFactory()
   {
      mFactory = new FactoryType();
   }

   //////////////////////////////////////////////////////////////////////////
   void FSM::SetProcessSelfTransitions(bool handle)
   {
      mProcessSelfTransitions = handle;
   }

   //////////////////////////////////////////////////////////////////////////
   bool FSM::GetProcessSelfTransitions() const
   {
      return mProcessSelfTransitions;
   }


} // namespace dtAI
