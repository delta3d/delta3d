/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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

#ifndef DELTA_MotionModelRTS_h__
#define DELTA_MotionModelRTS_h__

#include <dtCore/export.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/refptr.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/logicalinputdevice.h>


namespace dtCore
{
   class Keyboard;
   class Mouse;
   class MessageData;
   class Axis;

   /** A typical RTS camera controller.  Moving the mouse to the window
     * extents will pan the camera up, down, left, right as will keyboard
     * w, s, a, d.  The mouse wheel will zoom in and out.  Holding the <alt>
     * key and moving the mouse will rotate around the focal point.
     */
   class DT_CORE_EXPORT RTSMotionModel : public dtCore::OrbitMotionModel
   {
   public:
      RTSMotionModel(dtCore::Keyboard* keyboard,
                     dtCore::Mouse* mouse,
                     bool useSimTimeForSpeed = false,
                     bool useMouseScrolling = true);

      /**
       * Called when an axis' state has changed.
       *
       * @param axis the changed axis
       * @param oldState the old state of the axis
       * @param newState the new state of the axis
       * @param delta a delta value indicating stateless motion
       * @return If the
       */
      virtual bool HandleAxisStateChanged(const dtCore::Axis* axis,
                                    double oldState,
                                    double newState,
                                    double delta);

      /**
       * Message handler callback.
       *
       * @param data the message data
       */
      virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

      void SetTerrain(dtCore::DeltaDrawable* terrain) { mTerrainDrawable = terrain; }

      void EnableTranslation(bool enabled) { mShouldTranslate = enabled; }

      float GetMinGroundHeight() const   { return mMinGroundHeight; }
      void SetMinGroundHeight(float val) { mMinGroundHeight = val;  }

   protected:

      virtual ~RTSMotionModel();

   private:

      /**
       * A list of ref pointers to misc axis mappings created in this class.
       */
      std::vector<dtCore::RefPtr<AxisMapping> > mMiscAxisMappingList;

      dtCore::RefPtr<dtCore::DeltaDrawable> mTerrainDrawable;
      bool mUseSimTimeForSpeed;
      bool mShouldTranslate;
      float mMinGroundHeight;
   };
} // namespace dtCore

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_MotionModelRTS_h__
