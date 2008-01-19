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
* Taken from the source code for CTFGame off of the AI Game Programming Wisdom 4 chapter
* 	"Particle Filter Methods for More Realistic Hiding and Seeking" (Christian Darken, Brad Anderegg)
*
* Copyright (C) 2007, Bradley Anderegg
*/

#ifndef DELTA_STEERINGBEHAVIOR
#define DELTA_STEERINGBEHAVIOR

#include <dtUtil/typetraits.h>

namespace dtAI
{
   template<typename KinematicGoal_, typename Kinematic_, typename SteeringOutput_>
   class SteeringBehavior: public osg::Referenced
   {
      public:
         typedef KinematicGoal_ KinematicGoalType;
         typedef Kinematic_ KinematicType;
         typedef SteeringOutput_ SteeringOutputType;

         typedef typename dtUtil::TypeTraits<KinematicGoalType>::const_param_type ConstKinematicGoalParam;
         typedef typename dtUtil::TypeTraits<KinematicType>::const_param_type ConstKinematicParam;
         typedef typename dtUtil::TypeTraits<SteeringOutputType>::reference SteeringOutByRefParam;

      public:
         SteeringBehavior(){}

      public:
         /**
         * @param dt, the delta frame time
         * @param current_goal, the kinematic goal which the behavior can operate on
         * @param current_state, the kinematic state is physical state which we apply our behavior to in hopes of making it match the goal
         * @param result, the result of the behavior is copied into a SteeringOutput which can later be integrated by the physics model
         */
         virtual void Think(float dt, ConstKinematicGoalParam current_goal, ConstKinematicParam current_state, SteeringOutByRefParam result) = 0;
         virtual bool GoalAchieved(ConstKinematicGoalParam current_goal, ConstKinematicParam current_state) = 0;


      protected:
         /*virtual*/ ~SteeringBehavior(){}
   };
}

#endif //DELTA_STEERINGBEHAVIOR
