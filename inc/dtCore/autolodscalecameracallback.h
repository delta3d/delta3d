/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2011 MOVES Institute
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

#ifndef AUTOLODSCALECAMERACALLBACK_H__
#define AUTOLODSCALECAMERACALLBACK_H__

#include <dtCore/export.h>
#include <osg/Referenced>
#include <dtUtil/getsetmacros.h>
#include <dtCore/observerptr.h>

namespace dtCore
{

   class Camera;

   /**
    * @brief implements the auto level of detail (LOD) scale feature.
    *
    * The LOD scale is a multiplier that is applied to the distance from the view point to
    * the bounding sphere of an LOD node when it is traversed in the culling phase of rendering.
    * the.  The LOD node has several children that are assigned ranges of distance where they will be rendered,
    * and each child is expected to have a different level of detail.  At some distance, no child will be selected and
    * nothing will be rendered. The scale with either move those transitions closer or farther depending on if the value is
    * greater or lesser than 1.0 respectively.  This class will take into account the time it takes to render the scene
    * using the system frame time by default and decide whether to increase or decrease the scale to make the rendering
    * time approach the target.  Minimum and maximum scales values exist to so the rendering quality can be controlled as well.
    *
    * The Update and GetFrameTimeMS methods can be overridden to change the behavior if needed.
    */
   class DT_CORE_EXPORT AutoLODScaleCameraCallback : public osg::Referenced
   {
   public:
      /**
       * Main constructor.
       * @param camera The camera use a target. If you want to use a specific camera, you set this, if you want to
       *               apply it to all cameras, pass NULL.
       */
      AutoLODScaleCameraCallback(Camera* camera = NULL);
      virtual ~AutoLODScaleCameraCallback();

      /// The main update method. Override this to change the behavior.
      virtual void Update(Camera&);
      /// @return the frame time in milliseconds.  You may override this to use a different time value that you either lookup or calculate.
      virtual double GetFrameTimeMS() const;

      /**
       * The target frame time.  It will increase the LOD scale if the frame time is larger than this,
       * or decrease it if it's smaller. Default is 33.33 or 30Hz
       */
      DT_DECLARE_ACCESSOR(double, TargetFrameTimeMS)

      /// The amount of slop to allow when comparing the frame time to the target frame time.  Defaults to 0.3
      DT_DECLARE_ACCESSOR(double, TargetFrameTimeEpsilon)

      /**
       * The minimum LOD scale to set even if the framerate is really high.  You should probably set this to 1.0 (Default)
       * but if you want it to go lower than 1.0 so it will show better LOD if you have the performance, then you can.
       */
      DT_DECLARE_ACCESSOR(float, MinLODScale)

      /**
       * The maximum LOD scale to set even if the framerate is really low.  You should probably set this to something greater than 1.0.
       * The default is 2.0.
       */
      DT_DECLARE_ACCESSOR(float, MaxLODScale)

      /**
       * Each time it needs to change the number, this is the scalar factor of how much to change it.  0-1 work.  Something
       * around 0.001 probably makes the most sense so you don't get huge changes in one frame, which can lead to oscillations.
       */
      DT_DECLARE_ACCESSOR(float, ChangeFactor)

   private:
      dtCore::ObserverPtr<Camera> mCamera;
      // bool to see if it should just exit if the camera is/becomes null.
      bool mCameraWasNull;
   };
}
#endif // AUTOLODSCALECAMERACALLBACK_H__
