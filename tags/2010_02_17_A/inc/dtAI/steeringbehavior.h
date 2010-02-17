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
 *  Original version taken from the source code for CTFGame off of the AI Game Programming Wisdom 4 chapter
 *    "Particle Filter Methods for More Realistic Hiding and Seeking" (Christian Darken, Brad Anderegg)
 *
 * Copyright (C) 2007, Bradley Anderegg
 */

#ifndef DELTA_STEERINGBEHAVIOR
#define DELTA_STEERINGBEHAVIOR

#include <vector>
#include <algorithm>
#include <dtUtil/typetraits.h>
#include <dtUtil/templateutility.h>

namespace dtAI
{
   template<typename KinematicGoal_, typename Kinematic_, typename SteeringOutput_,
      typename SensorType = dtUtil::DoNothing0<int>, typename ErrorResultType = int,
      typename ErrorHandlerType = dtUtil::DoNothing<int, ErrorResultType> >
   class SteeringBehavior: public osg::Referenced
   {
   public:
      typedef KinematicGoal_ KinematicGoalType;
      typedef Kinematic_ KinematicType;
      typedef SteeringOutput_ SteeringOutputType;

      typedef std::vector<std::pair<SensorType, ErrorHandlerType> > SensorErrorArray;

      typedef typename dtUtil::TypeTraits<KinematicGoalType>::const_param_type ConstKinematicGoalParam;
      typedef typename dtUtil::TypeTraits<KinematicType>::const_param_type ConstKinematicParam;
      typedef typename dtUtil::TypeTraits<SteeringOutputType>::reference SteeringOutByRefParam;

      typedef typename dtUtil::TypeTraits<SensorType>::param_type SensorParamType;
      typedef typename dtUtil::TypeTraits<ErrorHandlerType>::param_type ErrorHandlerParamType;

   public:
      SteeringBehavior(){}

      /**
       * @param dt, the delta frame time
       * @param current_goal, the kinematic goal which the behavior can operate on
       * @param current_state, the kinematic state is physical state which we apply our behavior to in hopes of making it match the goal
       * @param result, the result of the behavior is copied into a SteeringOutput which can later be integrated by the physics model
       */
      virtual void Think(float dt, ConstKinematicGoalParam current_goal, ConstKinematicParam current_state, SteeringOutByRefParam result) = 0;

      void AddErrorHandler(SensorParamType s, ErrorHandlerParamType eh)
      {
         mSensorErrors.push_back(std::make_pair(SensorType(s), ErrorHandlerType(eh)));
      }

      void RemoveErrorHandler(SensorParamType s, ErrorHandlerParamType eh)
      {
         mSensorErrors.erase(std::remove(mSensorErrors.begin(), mSensorErrors.end(), std::make_pair(SensorType(s), ErrorHandlerType(eh))));
      }

      void InvokeErrorHandling()
      {
         std::for_each(mSensorErrors.begin(), mSensorErrors.end(), dtUtil::EvaluateInvoke<SensorParamType, ErrorHandlerParamType, ErrorResultType>());
      }

   protected:
      /*virtual*/ ~SteeringBehavior(){}

      SensorErrorArray mSensorErrors;
   };

} // namespace dtAI

#endif // DELTA_STEERINGBEHAVIOR
