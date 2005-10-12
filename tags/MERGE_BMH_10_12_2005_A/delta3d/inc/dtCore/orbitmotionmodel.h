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

// orbitmotionmodel.h: Declaration of the OrbitMotionModel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_ORBIT_MOTION_MODEL
#define DELTA_ORBIT_MOTION_MODEL

#include "dtCore/keyboard.h"
#include "dtCore/mouse.h"
#include "dtCore/logicalinputdevice.h"
#include "dtCore/motionmodel.h"
#include "dtCore/inputdevice.h"

namespace dtCore
{
   /**
    * A motion model that causes its target to orbit around a point
    * (initially its local origin).
    */
   class DT_CORE_EXPORT OrbitMotionModel : public MotionModel,
                                      public AxisListener
   {
      DECLARE_MANAGEMENT_LAYER(OrbitMotionModel)


      public:

         /**
          * Constructor.
          *
          * @param keyboard the keyboard instance, or NULL to
          * avoid creating default input mappings
          * @param mouse the mouse instance, or NULL to avoid
          * creating default input mappings
          */
         OrbitMotionModel(Keyboard* keyboard = NULL,
                          Mouse* mouse = NULL);

         /**
          * Destructor.
          */
         virtual ~OrbitMotionModel();
         
         /**
          * Sets the input axes to a set of default mappings for mouse
          * and keyboard.
          *
          * @param keyboard the keyboard instance
          * @param mouse the mouse instance
          */
         void SetDefaultMappings(Keyboard* keyboard, Mouse* mouse);
         
         /**
          * Sets the axis that affects the azimuth of the orbit.
          *
          * @param azimuthAxis the new azimuth axis
          */
         void SetAzimuthAxis(Axis* azimuthAxis);
         
         /**
          * Returns the axis that affects the azimuth of the orbit.
          *
          * @return the current azimuth axis
          */
         Axis* GetAzimuthAxis();
         
         /**
          * Sets the axis that affects the elevation of the orbit.
          *
          * @param elevationAxis the new elevation axis
          */
         void SetElevationAxis(Axis* elevationAxis);
         
         /**
          * Returns the axis that affects the elevation of the orbit.
          *
          * @return the current elevation axis
          */
         Axis* GetElevationAxis();
         
         /**
          * Sets the axis that affects the distance of the orbit.
          *
          * @param distanceAxis the new distance axis
          */
         void SetDistanceAxis(Axis* distanceAxis);
         
         /**
          * Returns the axis that affects the distance of the orbit.
          *
          * @return the current distance axis
          */
         Axis* GetDistanceAxis();
         
         /**
          * Sets the axis that affects the left/right translation of the orbit.
          *
          * @param leftRightTranslationAxis the new left/right translation axis
          */
         void SetLeftRightTranslationAxis(Axis* leftRightTranslationAxis);
         
         /**
          * Returns the axis that affects the left/right translation of the orbit.
          *
          * @return the current left/right translation axis
          */
         Axis* GetLeftRightTranslationAxis();
         
         /**
          * Sets the axis that affects the up/down translation of the orbit.
          *
          * @param upDownTranslationAxis the new up/down translation axis
          */
         void SetUpDownTranslationAxis(Axis* upDownTranslationAxis);
         
         /**
          * Returns the axis that affects the up/down translation of the orbit.
          *
          * @return the current up/down translation axis
          */
         Axis* GetUpDownTranslationAxis();
         
         /**
          * Sets the angular rate (the ratio between axis units and angular
          * movement in degrees).
          *
          * @param angularRate the new angular rate
          */
         void SetAngularRate(float angularRate);
         
         /**
          * Returns the angular rate.
          *
          * @return the current angular rate
          */
         float GetAngularRate();
         
         /**
          * Sets the linear rate (the ratio between axis units and linear
          * movement in meters).
          *
          * @param linearRate the new linear rate
          */
         void SetLinearRate(float linearRate);
         
         /**
          * Returns the linear rate.
          *
          * @return the current linear rate
          */
         float GetLinearRate();
         
         /**
          * Sets the distance from the focal point.
          *
          * @param distance the new distance
          */
         void SetDistance(float distance);
         
         /**
          * Returns the distance from the focal point.
          *
          * @return the current distance
          */
         float GetDistance();
         
         /**
          * Called when an axis' state has changed.
          *
          * @param axis the changed axis
          * @param oldState the old state of the axis
          * @param newState the new state of the axis
          * @param delta a delta value indicating stateless motion
          */
         virtual void AxisStateChanged(Axis* axis,
                                       double oldState, 
                                       double newState, 
                                       double delta);
         
         
      private:
      
         /**
          * The default input device.
          */
         RefPtr<LogicalInputDevice> mDefaultInputDevice;
         
         /**
          * The left button up/down mapping.
          */
         ButtonAxisToAxis* mLeftButtonUpDownMapping;
         
         /**
          * The left button right/left mapping.
          */
         ButtonAxisToAxis* mLeftButtonLeftRightMapping;
         
         /**
          * The right button up/down mapping.
          */
         ButtonAxisToAxis* mRightButtonUpDownMapping;
         
         /**
          * The right button left/right mapping.
          */
         ButtonAxisToAxis* mRightButtonLeftRightMapping;
         
         /**
          * The middle button up/down mapping.
          */
         ButtonAxisToAxis* mMiddleButtonUpDownMapping;
         
         /**
          * The default azimuth axis.
          */
         LogicalAxis* mDefaultAzimuthAxis;
         
         /**
          * The default azimuth axis.
          */
         LogicalAxis* mDefaultElevationAxis;
         
         /**
          * The default azimuth axis.
          */
         LogicalAxis* mDefaultDistanceAxis;
         
         /**
          * The default azimuth axis.
          */
         LogicalAxis* mDefaultLeftRightTranslationAxis;
         
         /**
          * The default azimuth axis.
          */
         LogicalAxis* mDefaultUpDownTranslationAxis;
         
         /**
          * The axis that affects the azimuth of the orbit.
          */
         Axis* mAzimuthAxis;
         
         /**
          * The axis that affects the elevation of the orbit.
          */
         Axis* mElevationAxis;
         
         /**
          * The axis that affects the distance of the orbit.
          */
         Axis* mDistanceAxis;
         
         /**
          * The axis that affects the left/right translation of the orbit.
          */
         Axis* mLeftRightTranslationAxis;
         
         /**
          * The axis that affects the up/down translation of the orbit.
          */
         Axis* mUpDownTranslationAxis;
         
         /**
          * The angular rate (ratio between axis units and angular movement).
          */ 
         float mAngularRate;
         
         /**
          * The linear rate (ratio between axis units and linear movement).
          */
         float mLinearRate;
         
         /**
          * The current distance from the focal point.
          */
         float mDistance;
   };
};

#endif // DELTA_ORBIT_MOTION_MODEL
