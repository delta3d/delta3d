/*
 * CivilianActor.cpp
 *
 *  Created on: Aug 27, 2014
 *      Author: david
 */

#include "civilianactor.h"
#include "civilianaiactorcomponent.h"
#include "dtAnim/animationtransitionplanner.h"

namespace dtExample
{

   CivilianActor::CivilianActor()
   {
   }

   CivilianActor::~CivilianActor()
   {
   }

   void CivilianActor::BuildActorComponents()
   {
      BaseClass::BuildActorComponents();
      AddComponent(*new dtAnim::AnimationTransitionPlanner);
      AddComponent(*new CivilianAIActorComponent);
   }


} /* namespace dtExample */
