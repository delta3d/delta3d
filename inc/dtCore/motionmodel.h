/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
*/

#ifndef DELTA_MOTIONMODEL
#define DELTA_MOTIONMODEL

// motionmodel.h: Declaration of the MotionModel class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/base.h>
#include <dtCore/refptr.h>
#include <dtCore/transformable.h>

namespace dtCore
{

   /**
    * The base class for all motion models: objects that update the
    * transforms of Transformable objects according to input gathered
    * from physical or logical input devices.
    */
   class DT_CORE_EXPORT MotionModel : public Base
   {
      DECLARE_MANAGEMENT_LAYER(MotionModel)

      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         MotionModel(const std::string& name = "MotionModel");

      protected:

         /**
          * Destructor.
          */
         virtual ~MotionModel();

      public:

         /**
          * Sets the target of this motion model.
          *
          * @param target the new target
          */
         virtual void SetTarget(Transformable* target);

         /**
          * Returns the target of this motion model.
          *
          * @return the current target
          */
         Transformable* GetTarget();
         const Transformable* GetTarget() const;

         /**
          * Enables or disables this motion model.
          *
          * @param enabled true to enable this motion model, false
          * to disable it
          */
         virtual void SetEnabled(bool enabled);

         /**
          * Checks whether or not this motion model is enabled.
          *
          * @return true if this motion model is enabled, false
          * if it is disabled
          */
         bool IsEnabled() const;

      private:

         /**
          * The motion model target.
          */
         RefPtr<Transformable> mTarget;

         /**
          * Whether or not the motion model is enabled.
          */
         bool mEnabled;
   };
}


#endif // DELTA_MOTIONMODEL
