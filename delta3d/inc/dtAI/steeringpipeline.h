/* -*-c++-*-
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
 *
 * Copyright (C) 2007, Bradley Anderegg
 */

#ifndef DELTA_STEERINGPIPELINE
#define DELTA_STEERINGPIPELINE

#include <vector>
#include <dtUtil/log.h>
#include <dtUtil/functor.h>
#include <dtDAL/propertymacros.h>
#include <dtAI/steeringbehavior.h>

namespace dtAI
{

   template <class State_, class GoalState_, class Controls_, class PathType_ = std::vector<GoalState_> >
   class AIPipelineFunctorsBase
   {
   public:
      // these typedefs are used by the steering pipeline to resolve type info
      // on the 3 template parameters of a Controllable
      typedef State_ StateType;
      typedef GoalState_ GoalStateType;
      typedef PathType_ PathType;
      typedef Controls_ ControlType;
   };

   template <class State_, class GoalState_>
   class Targeter
   {
   public:
      typedef State_ StateType;
      typedef GoalState_ GoalStateType;

      typedef dtUtil::Functor<bool, TYPELIST_2(const StateType&, GoalStateType&)> TargetFunctor;

      // returns true to continue
      virtual bool GetGoal(const StateType& current_state, GoalStateType& result) = 0;
   };

   template <class State_, class GoalState_>
   class Decomposer
   {
   public:
      typedef State_ StateType;
      typedef GoalState_ GoalStateType;
      typedef dtUtil::Functor<bool, TYPELIST_2(const StateType&, GoalStateType&)> DecomposeFunctor;

      virtual void Decompose(const StateType& current_state, GoalStateType& result) const = 0;
   };

   template < class State_, class GoalState_, class PathType_ = std::vector<GoalState_> >
   class Constraint
   {
   public:
      typedef State_ StateType;
      typedef GoalState_ GoalStateType;
      typedef PathType_ PathType;

      typedef dtUtil::Functor<bool, TYPELIST_1(const PathType&)> CheckConstraintFunctor;
      typedef dtUtil::Functor<bool, TYPELIST_3(const PathType&, const StateType&, GoalStateType&)> FixConstraintFunctor;

      virtual bool WillViolate(const PathType& pathToFollow) const = 0;
      virtual void Suggest(const PathType& pathToFollow, const StateType& current_state, GoalStateType& result) const = 0;
   };

   template <class State_, class GoalState_, class Controls_, class PathType_ = std::vector<GoalState_> >
   struct ObtainGoal : public AIPipelineFunctorsBase<State_, GoalState_, Controls_, PathType_>
   {
      typedef AIPipelineFunctorsBase<State_, GoalState_, Controls_, PathType_> BaseClass;

      ObtainGoal(const typename BaseClass::StateType& k) : mState(k) {}

      bool operator()(Targeter<State_, GoalState_>& target)
      {
         return target.GetGoal(mState, mGoal);
      }

      const typename BaseClass::StateType& mState;
      typename BaseClass::GoalStateType mGoal;
   };

   template <class State_, class GoalState_, class Controls_, class PathType_ = std::vector<GoalState_> >
   struct DecomposeGoal : public AIPipelineFunctorsBase<State_, GoalState_, Controls_, PathType_>
   {
      typedef AIPipelineFunctorsBase<State_, GoalState_, Controls_, PathType_> BaseClass;

      DecomposeGoal(const typename BaseClass::StateType& k, typename BaseClass::GoalStateType& g)
         : mState(k)
         , mGoal(g)
      {}

      void operator()(const Decomposer<State_, GoalState_>& target)
      {
         return target.Decompose(mState, mGoal);
      }

      const typename BaseClass::StateType& mState;
      typename BaseClass::GoalStateType& mGoal;
   };

   template<class IterType, class Functor>
   void ForEachIf(IterType from, IterType to, Functor& func)
   {
      bool result = true;
      for (; from != to && result; ++from) result = func(**from);
   }

   template<class IterType, class Functor>
   void ForEach(IterType from, IterType to, Functor func)
   {
      for (; from != to; ++from) func(**from);
   }

   template <class ControllableType_>
   class SteeringPipeline : public AIPipelineFunctorsBase<typename ControllableType_::StateType,
                                                          typename ControllableType_::GoalStateType,
                                                          typename ControllableType_::ControlType,
                                                          typename ControllableType_::PathType >
   {
   public:
      typedef AIPipelineFunctorsBase<typename ControllableType_::StateType,
         typename ControllableType_::GoalStateType,
         typename ControllableType_::ControlType,
         typename ControllableType_::PathType> BaseClass;

      typedef ControllableType_ ControllableType;

      typedef typename ControllableType::StateType AIState;
      typedef typename ControllableType::GoalStateType AIGoal;
      typedef typename ControllableType::ControlType AIControlState;
      typedef typename ControllableType::PathType AIPath;

      typedef typename ControllableType::TargeterType TargeterType;
      typedef typename ControllableType::DecomposerType DecomposerType;
      typedef typename ControllableType::ConstraintType ConstraintType;

      typedef SteeringPipeline<ControllableType> value_type;

   public:
      SteeringPipeline()
         : mMaxUpdateAttempts(10)
      {
      }

      ~SteeringPipeline()
      {
      }

      bool Step(float dt, ControllableType& entityToStep)
      {
         ObtainGoal<AIState, AIGoal, AIControlState, AIPath> og(entityToStep.mCurrentState);
         ForEachIf(entityToStep.GetTargeters().begin(), entityToStep.GetTargeters().end(), og);

         AIGoal curr_goal = og.mGoal;

         DecomposeGoal<AIState, AIGoal, AIControlState, AIPath> dg(entityToStep.mCurrentState, curr_goal);
         ForEach(entityToStep.GetDecomposers().begin(), entityToStep.GetDecomposers().end(), dg);

         AIControlState& controlsOut = entityToStep.mCurrentControls;
         bool hasOutput = FindGoal(curr_goal, entityToStep, controlsOut, mMaxUpdateAttempts);

         //if no constraints are satisfied then we fall back to a default Behavior
         if(hasOutput)
         {
            //update our state based on our steering output
            entityToStep.UpdateState(dt, controlsOut);
         }
         else
         {
            //mDefaultBehavior->Think(dt, curr_goal, mKinematicState, output);
            LOG_ERROR("Steering Pipeline unable to satisfy constraints.");
         }

         return hasOutput;
      }

   private:
      bool FindGoal(AIGoal& g, ControllableType& entityToStep, AIControlState& output, int maxAttempts)
      {
         AIPath p;
         const AIState& state = entityToStep.mCurrentState;
         if(maxAttempts <= 0 || !entityToStep.FindPath(state, g, p))
         {
            //we cannot complete our goal
            return false;
         }

         //todo- use ForEachIF() as above
         typename ControllableType::ConstraintArray::iterator iter = entityToStep.GetConstraints().begin();
         typename ControllableType::ConstraintArray::iterator iterEnd = entityToStep.GetConstraints().end();

         for(;iter != iterEnd; ++iter)
         {
            Constraint<AIState, AIGoal, AIPath>& cont = **iter;
            if(cont.WillViolate(p))
            {
               cont.Suggest(p, state, g);
               FindGoal(g, entityToStep, output, --maxAttempts);
            }
         }

         entityToStep.OutputControl(p, state, output);
         return true;
      }

      unsigned mMaxUpdateAttempts;
   };

} // namespace dtAI

#endif //DELTA_STEERINGPIPELINE
