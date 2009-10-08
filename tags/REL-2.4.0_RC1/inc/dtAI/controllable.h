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

#ifndef DELTA_AICONTROLLABLE
#define DELTA_AICONTROLLABLE

#include <vector>

#include <dtDAL/propertymacros.h>

#include <dtUtil/log.h>
#include <dtUtil/functor.h>

#include <dtAI/steeringpipeline.h>
#include <dtAI/steeringbehavior.h>

namespace dtAI
{

   template < class State_, class GoalState_, class Controls_, class PathType_ = std::vector<GoalState_> >
   class Controllable: public AIPipelineFunctorsBase<State_, GoalState_, Controls_, PathType_>
   {
   public:
      typedef AIPipelineFunctorsBase<State_, GoalState_, Controls_, PathType_> BaseClass;
      typedef Controllable<State_, GoalState_, Controls_, PathType_> ControllableType;

      typedef typename BaseClass::StateType AIState;
      typedef typename BaseClass::GoalStateType AIGoal;
      typedef typename BaseClass::ControlType AIControlState;
      typedef typename BaseClass::PathType AIPath;

      typedef Targeter<AIState, AIGoal> TargeterType;
      typedef Decomposer<AIState, AIGoal> DecomposerType;
      typedef Constraint<AIState, AIGoal, AIPath> ConstraintType;

      typedef dtAI::SteeringBehavior<AIGoal, AIState, AIControlState> SteeringBehaviorType;

      typedef std::vector<TargeterType*> TargeterArray;
      typedef std::vector<DecomposerType*> DecomposerArray;
      typedef std::vector<ConstraintType*> ConstraintArray;

      typedef dtUtil::Functor<void, TYPELIST_3(const AIPath&, const AIState&, AIControlState&)> OutputControlFunctor;
      typedef dtUtil::Functor<bool, TYPELIST_3(const AIState&, AIGoal&, AIPath&)> GetPathFunctor;
      typedef dtUtil::Functor<void, TYPELIST_2(float, const AIControlState&)> UpdateStateFunctor;

   public:
      Controllable()
      {
         mFindPathFunc = GetPathFunctor(this, &ControllableType::DefaultFindPath);
         mOutputControlFunc = OutputControlFunctor(this, &ControllableType::DefaultOutputControl);
         mUpdateStateFunc = UpdateStateFunctor(this, &ControllableType::DefaultUpdateState);
      }

      virtual ~Controllable(){}

      TargeterArray& GetTargeters()
      {
         return mTargeters;
      }

      DecomposerArray& GetDecomposers()
      {
         return mDecomposers;
      }

      ConstraintArray& GetConstraints()
      {
         return mConstraints;
      }

      /**
       * @return if a valid path has been found
       */
      virtual bool FindPath(const AIState& fromState, AIGoal& goal, AIPath& resultingPath) const
      {
         // by default we call the functor
         return mFindPathFunc(fromState, goal, resultingPath);
      }

      bool DefaultFindPath(const AIState& fromState, AIGoal& goal, AIPath& resultingPath) const
      {
         resultingPath.push_back(goal);
         return true;
      }

      virtual void OutputControl(const AIPath& pathToFollow, const AIState& current_state, AIControlState& result) const
      {
         // by default we call the functor
         mOutputControlFunc(pathToFollow, current_state, result);
      }

      void DefaultOutputControl(const AIPath& pathToFollow, const AIState& current_state, AIControlState& result) const
      {
      }

      virtual void UpdateState(float dt, const AIControlState& steerData)
      {
         // by default we call the functor
         mUpdateStateFunc(dt, steerData);
      }

      void DefaultUpdateState(float dt, const AIControlState& steerData)
      {
      }

      virtual void RegisterProperties(dtDAL::PropertyContainer& pc, const std::string& group)
      {
         mDefaultControls.RegisterProperties(pc, "Default Values");
         mDefaultState.RegisterProperties(pc, "Default Values");

         mStateConstraints.RegisterProperties(pc, "Constraints");
         mControlConstraints.RegisterProperties(pc, "Constraints");
      };

      AIControlState mCurrentControls;
      AIState mCurrentState;
      AIState NextPredictedState;
      AIGoal mGoalState;

      OutputControlFunctor mOutputControlFunc;
      GetPathFunctor mFindPathFunc;
      UpdateStateFunctor mUpdateStateFunc;

      TargeterArray mTargeters;
      DecomposerArray mDecomposers;
      ConstraintArray mConstraints;

      AIControlState mDefaultControls;
      AIState mDefaultState;
      AIState mStateConstraints;
      AIControlState mControlConstraints;

      AIPath mPathToFollow;
      AIPath PredictedPath;
   };

} // namespace dtAI

#endif //DELTA_AICONTROLLABLE
