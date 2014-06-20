/* -*-c++-*-
 * dtPhysics
 * Copyright 2010, Alion Science and Technology
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
 * David Guthrie
 */

#ifndef ACTION_H_
#define ACTION_H_

#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>

class palAction;

namespace dtPhysics
{
   class ActionImpl;

   /**
    * Actions are custom objects that can be added to the physics system and they will be called during each physics step.
    * The default behavior is to call these once each time palPhysics::Update is called, but some engines support calling this every
    * substep.  In either case, the correct time step will be passed in.
    *
    * @note Unless you are certain the engine you are using will only call this once per call to Update(), you should
    *       not call AddForce because the force could continue to be applied for multiple time steps, giving undesired
    *       results.  Instead, you should call ApplyImpulse giving the force you want multiplied by the timestep.
    */
   class DT_PHYSICS_EXPORT Action : public osg::Referenced
   {
   public:
      Action();
      virtual ~Action();
      virtual void operator()(Real timeStep) = 0;

      palAction* GetPalAction();

   private:
      ActionImpl* mImpl;
   };
}

#endif /* ACTION_H_ */
