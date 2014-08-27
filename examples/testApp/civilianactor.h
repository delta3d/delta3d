/*
 * CivilianActor.h
 *
 *  Created on: Aug 27, 2014
 *      Author: david
 */

#ifndef CIVILIANACTOR_H_
#define CIVILIANACTOR_H_

#include <dtAnim/animationgameactor.h>

namespace dtExample
{

   class CivilianActor: public dtAnim::AnimationGameActor
   {
   public:

      typedef dtAnim::AnimationGameActor BaseClass;

      CivilianActor();

      /**
       * Overridden to add more components.
       */
      virtual void BuildActorComponents();

   protected:
      virtual ~CivilianActor();
   };

} /* namespace dtExample */

#endif /* CIVILIANACTOR_H_ */
