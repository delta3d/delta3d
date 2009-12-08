/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 03/28/2007
 */

#ifndef __DELTA_CAL3DANIMATOR_H__
#define __DELTA_CAL3DANIMATOR_H__

#include <dtAnim/export.h>
#include <dtAnim/ical3ddriver.h>
#include <dtAnim/cal3dmodelwrapper.h>

#include <osg/Referenced>
#include <dtCore/refptr.h>

namespace dtAnim
{

   class DT_ANIM_EXPORT Cal3DAnimator: public osg::Referenced
   {
   public:
      Cal3DAnimator(Cal3DModelWrapper* pWrapper);

      void Update(double dt);

      Cal3DModelWrapper* GetWrapper();
      const Cal3DModelWrapper* GetWrapper() const;

      ///Give the Animator a new Cal3DModelWrapper to operate on.
      void SetWrapper(Cal3DModelWrapper* wrapper);

      // these are listed in the order in which they are called on update
      void SetPreDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetPreDriver() const {return mPreDriver.get(); }

      void SetAnimationDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetAnimationDriver() const {return mAnimDriver.get(); }

      void SetSkeletonDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetSkeletonDriver() const {return mSkelDriver.get(); }

      void SetMorphTargetDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetMorphTargetDriver() const {return mMorphDriver.get(); }

      void SetPhysiqueDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetPhysiqueDriver() const {return mPhysiqueDriver.get(); }

      void SetSpringDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetSpringDriver() const {return mSpringDriver.get(); }

      void SetPostDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetPostDriver() const {return mPostDriver.get(); }

   protected:
      virtual ~Cal3DAnimator();

   private:
      dtCore::RefPtr<Cal3DModelWrapper> mWrapper;

      dtCore::RefPtr<ICal3DDriver> mPreDriver;
      dtCore::RefPtr<ICal3DDriver> mPostDriver;
      dtCore::RefPtr<ICal3DDriver> mAnimDriver;
      dtCore::RefPtr<ICal3DDriver> mSkelDriver;
      dtCore::RefPtr<ICal3DDriver> mMorphDriver;
      dtCore::RefPtr<ICal3DDriver> mSpringDriver;
      dtCore::RefPtr<ICal3DDriver> mPhysiqueDriver;
   };

} // namespace dtAnim

#endif // __DELTA_CAL3DANIMATOR_H__
