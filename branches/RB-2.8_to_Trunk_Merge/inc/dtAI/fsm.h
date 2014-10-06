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

#ifndef __DELTA_FSM_H__
#define __DELTA_FSM_H__

#include <dtAI/export.h>
#include <dtAI/npcstate.h>
#include <dtAI/npcevent.h>

#include <dtCore/refptr.h>

#include <dtUtil/objectfactory.h>
#include <set>
#include <map>
#include <functional>
#include <string>

namespace dtAI
{
   /**
    * A class which represents a Finite State Machine
    */
   class DT_AI_EXPORT FSM
   {
   public:
      // had to place this outside of the template so gcc won't gripe.
      /** Compares 2 referenced pointer objects by pointer value and name.*/
      template<typename T>
      struct RefPtrWithNameCompare : std::binary_function<T,T,bool>
      {
         /**
          * RefPtrWithNameCompare will make sure the State being added is
          * unique to the set based on its name AND based on the fact
          * that the State has a unique place in memory.
          * This makes sure that no one tried to submit a State that
          * had the same name as another State, or someone tried to
          * resubmit a State already in the set by changing its name.
          */
         bool operator()(const T& lhs,const T& rhs) const
         {
            return lhs.get() != rhs.get() && lhs->GetName() < rhs->GetName();
         }
      };

      // had to place this outside of the template so gcc won't gripe.
      /** Compares a pair, but assumes the 2nd type is a referenced pointer.*/
      template<typename T>
      struct PairRefPtrWithNameCompare : public std::binary_function<T,T,bool>
      {
         /**
          * Re-implement the default comparison algorithm for std::pair<T1,T2>::operator<,
          * but add smart StatePtr comparison with the RefPtrWithNameCompare predicate.
          * \sa http://www.sgi.com/tech/stl/pair.html
          */
         bool operator()(const T& x, const T& y) const
         {
            // try to use the first element
            bool first_less(x.first < y.first);
            if (first_less)
            {
               return true;
            }

            bool first_greater(y.first < x.first);
            if (first_greater)
            {
               return false;
            }

            // else, key off the second element, and use the RefPtrWithNameCompare comparison
            RefPtrWithNameCompare<typename T::second_type> compare_them;
            return compare_them(x.second,y.second);
         }
      };

      typedef dtUtil::ObjectFactory<std::string, NPCState> FactoryType;
      typedef dtCore::RefPtr<NPCState> StatePtr;
      typedef std::set< StatePtr, RefPtrWithNameCompare<StatePtr> > StateSet;
      typedef std::pair< const NPCEvent*, StatePtr > EventStatePtrPair;
      typedef std::map< EventStatePtrPair, StatePtr, PairRefPtrWithNameCompare<EventStatePtrPair> > TransitionMap;

   public:
      FSM(); // sets up default factory
      FSM(FactoryType* pFactory); //created with specific NPCState factory
      virtual ~FSM();

      virtual void Update(double dt);

      NPCState* AddState(const NPCState::Type* state);

      void AddTransition( const NPCEvent* eventType, const NPCState::Type* from, const NPCState::Type* to);

      NPCState* GetCurrentState();
      NPCState* GetState(const NPCState::Type* pStateType);

      /** Forces the given State to now be the 'current' State.*/
      void MakeCurrent(const NPCState::Type* state);

      /**
       * pass all events through this function
       * @return whether or not the event caused a transition
       */

      bool HandleEvent(const NPCEvent* pEvent);

      /** 
        *  Call entry and exit functions when a self transition takes place, 
        *  e.g. when a transition from state A to state A takes place.
        *  Defaults to true.
        *  @param handle True will call OnExit()/OnEntry() when transitioning to the 
        *  same state, false will not.
        */
      void SetProcessSelfTransitions(bool handle);

      /** 
        * Should self transitions be processed?
        * @return true if transitions OnExit()/OnEntry() is called when transitioning
        * to the same state.  False otherwise.
        */
      bool GetProcessSelfTransitions() const;

   private:
      void FreeMem();
      void OnStateChange(NPCState* pState);

      // NPCState's are not abstract so by default we don't need a derivation
      void SetupDefaultFactory();

   protected:
      dtCore::RefPtr<NPCState>  mCurrentState;

   private:

      dtCore::RefPtr<FactoryType > mFactory;

      StateSet                mStates;
      TransitionMap           mTransitions;
      bool                    mProcessSelfTransitions;
   };
} // namespace dtAI

#endif // __DELTA_FSM_H__
