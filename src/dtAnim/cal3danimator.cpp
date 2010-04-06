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

#include <dtAnim/cal3danimator.h>

#include <dtAnim/animdriver.h>
#include <dtAnim/skeletondriver.h>
#include <dtAnim/morphdriver.h>
#include <dtAnim/physiquedriver.h>
#include <dtAnim/springdriver.h>

namespace dtAnim
{

Cal3DAnimator::Cal3DAnimator(Cal3DModelWrapper* pWrapper)
   : mWrapper(pWrapper)
   , mPreDriver(0)
   , mPostDriver(0)
   , mAnimDriver(new AnimDriver(pWrapper))
   , mSkelDriver(new SkeletonDriver(pWrapper))
   , mMorphDriver(new MorphDriver(pWrapper))
   , mSpringDriver(new SpringDriver(pWrapper))
   , mPhysiqueDriver(new PhysiqueDriver(pWrapper))
{
}


Cal3DAnimator::~Cal3DAnimator()
{
}


void Cal3DAnimator::Update(double dt)
{
   // it is ok for a driver to be null

   if (mPreDriver.valid())      { mPreDriver->Update(dt);      }

   if (mAnimDriver.valid())     { mAnimDriver->Update(dt);     }

   if (mSkelDriver.valid())     { mSkelDriver->Update(dt);     }

   if (mMorphDriver.valid())    { mMorphDriver->Update(dt);    }

   if (mPhysiqueDriver.valid()) { mPhysiqueDriver->Update(dt); }

   if (mSpringDriver.valid())   { mSpringDriver->Update(dt);   }

   if (mPostDriver.valid())     { mPostDriver->Update(dt);     }

}

Cal3DModelWrapper* Cal3DAnimator::GetWrapper()
{
   return mWrapper.get();
}


const Cal3DModelWrapper* Cal3DAnimator::GetWrapper() const
{
   return mWrapper.get();
}


void Cal3DAnimator::SetPreDriver(ICal3DDriver* pDriver)
{
   mPreDriver = pDriver;
}

void Cal3DAnimator::SetAnimationDriver(ICal3DDriver* pDriver)
{
   mAnimDriver = pDriver;
}

void Cal3DAnimator::SetSkeletonDriver(ICal3DDriver* pDriver)
{
   mSkelDriver = pDriver;
}

void Cal3DAnimator::SetMorphTargetDriver(ICal3DDriver* pDriver)
{
   mMorphDriver = pDriver;
}

void Cal3DAnimator::SetPhysiqueDriver(ICal3DDriver* pDriver)
{
   mPhysiqueDriver = pDriver;
}

void Cal3DAnimator::SetPostDriver(ICal3DDriver* pDriver)
{
   mPostDriver = pDriver;
}

/** Will pass the wrapper unto any ICal3DDrivers that may be present.
 * @param wrapper : the new Cal3DModelWrapper this instance should use.
 */
void Cal3DAnimator::SetWrapper( Cal3DModelWrapper *wrapper )
{
   mWrapper = wrapper;

   if (mPreDriver.valid())      { mPreDriver->SetWrapper(wrapper);      }
   if (mPostDriver.valid())     { mPostDriver->SetWrapper(wrapper);     }
   if (mAnimDriver.valid())     { mAnimDriver->SetWrapper(wrapper);     }
   if (mSkelDriver.valid())     { mSkelDriver->SetWrapper(wrapper);     }
   if (mMorphDriver.valid())    { mMorphDriver->SetWrapper(wrapper);    }
   if (mSpringDriver.valid())   { mSpringDriver->SetWrapper(wrapper);   }
   if (mPhysiqueDriver.valid()) { mPhysiqueDriver->SetWrapper(wrapper); }
}

} // namespace dtAnim
