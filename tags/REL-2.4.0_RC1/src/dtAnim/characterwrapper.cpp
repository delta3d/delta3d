/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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
 * Bradley Anderegg 12/21/2007
 */

#include <dtAnim/characterwrapper.h>
#include <osg/Math>
#include <dtCore/isector.h>
#include <dtCore/transform.h>
#include <dtUtil/matrixutil.h>
#include <dtAnim/sequencemixer.h>
#include <osg/MatrixTransform>
#include <dtAnim/cal3danimator.h>
#include <dtAnim/attachmentcontroller.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/ical3ddriver.h>
#include <dtAnim/animationhelper.h>
#include <dtUtil/hotspotdefinition.h>


namespace dtAnim
{
   ////////////////////////////////////////////////////////////////////////////////
   CharacterWrapper::CharacterWrapper(const std::string& filename)
      : BaseClass()
      , mSpeed(0.0f)
      , mRotationSpeed(0.0f)
      , mHeightAboveGround(0.0f)
      , mLocalOffset(0)
      , mAnimHelper(0)
   {
      Init(filename);
   }

   ////////////////////////////////////////////////////////////////////////////////
   CharacterWrapper::~CharacterWrapper()
   {
   }


   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::Update(float dt)
   {
      osg::Vec3 pos;
      dtCore::Transform trans;
      BaseClass::GetTransform(trans);
      trans.GetTranslation(pos);

      //apply rotation
      osg::Vec3 hpr;
      trans.GetRotation(hpr); //this is a little less efficient but increase readability dramatically
      hpr[0] += mRotationSpeed * dt;
      trans.SetRotation(hpr);

      //apply translation
      osg::Matrix rot;
      trans.GetRotation(rot);
      osg::Vec3 forwardVec = dtUtil::MatrixUtil::GetRow3(rot, 1);
      pos += forwardVec * (mSpeed * dt);

      //ground clamp
      if (mIsector.valid())
      {
         //we disable our nodemask so we don't intersect ourself
         unsigned nodeMask = BaseClass::GetOSGNode()->getNodeMask();
         BaseClass::GetOSGNode()->setNodeMask(0x0);

         mIsector->SetStartPosition(pos + osg::Vec3(0.0f, 0.0f, mHeightAboveGround + 1.0f)); //start the intersect 1 meter above the target
         mIsector->SetDirection(osg::Vec3(0.0f, 0.0f, -1.0f)); //straight down
         if (mIsector->Update())
         {
            pos[2] = mIsector->GetHitList()[0].getWorldIntersectPoint()[2] + mHeightAboveGround;
         }

         //set our node back to its original state
         BaseClass::GetOSGNode()->setNodeMask(nodeMask);
      }

      //update our actual transform
      trans.SetTranslation(pos);
      BaseClass::SetTransform(trans);

      //update the animation helper
      mAnimHelper->Update(dt);
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CharacterWrapper::GetHeightAboveGround() const
   {
      return mHeightAboveGround;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::SetHeightAboveGround(float heightAboveGround)
   {
      mHeightAboveGround = heightAboveGround;
   }


   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::SetGroundClamp(dtCore::Transformable* nodeToClampTo, float heightAboveGround)
   {
      if (!mIsector.valid())
      {
         mIsector = new dtCore::Isector();
      }

      mHeightAboveGround = heightAboveGround;
      mIsector->SetGeometry(nodeToClampTo);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::SetGroundClamp(dtCore::Scene* sceneRoot, float heightAboveGround)
   {
      if (!mIsector.valid())
      {
         mIsector = new dtCore::Isector(sceneRoot);
      }

      mHeightAboveGround = heightAboveGround;
      mIsector->SetScene(sceneRoot);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::Init(const std::string& filename)
   {
      mAnimHelper = new dtAnim::AnimationHelper();
      mAnimHelper->LoadModel(filename);

      mLocalOffset = new osg::MatrixTransform();
      mLocalOffset->addChild(mAnimHelper->GetNode());
      BaseClass::GetMatrixNode()->addChild(mLocalOffset.get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CharacterWrapper::GetSpeed() const
   {
      return mSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::SetSpeed(float metersPerSecond)
   {
      mSpeed = metersPerSecond;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CharacterWrapper::GetRotationSpeed() const
   {
      return mRotationSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::SetRotationSpeed(float radiansPerSecond)
   {
      mRotationSpeed = radiansPerSecond;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::RotateToHeading(float headingInRadians, float delta)
   {
      float rotation = GetHeading();
      float dR = headingInRadians - rotation;

      if (dR > 180.0f)
      {
         dR -= 360.0f;
      }
      else if (dR < -180.0f)
      {
         dR += 360.0f;
      }

      if (dR > 0.0f)
      {
         rotation += (dR > delta*90.0f ? delta*90.0f : dR);
      }
      else if (dR < 0.0f)
      {
         rotation += (dR < -delta*90.0f ? -delta*90.0f : dR);
      }

      SetHeading(rotation);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::RotateToPoint(const osg::Vec3& targetPos, float delta)
   {
      osg::Vec3 ownPos;
      dtCore::Transform trans;
      BaseClass::GetTransform(trans);
      trans.GetTranslation(ownPos);

      osg::Vec3 pVector = targetPos - ownPos;

      pVector[2] = 0.0f;

      float dir = osg::RadiansToDegrees(atan2(pVector[0], -pVector[1]));
      float rotation = GetHeading();
      float dR = dir - rotation;

      if (dR > 180.0f)
      {
         dR -= 360.0f;
      }
      else if (dR < -180.0f)
      {
         dR += 360.0f;
      }

      if (dR > 0.0f)
      {
         rotation += (dR > delta*90.0f ? delta*90.0f : dR);
      }
      else if (dR < 0.0f)
      {
         rotation += (dR < -delta*90.0f ? -delta*90.0f : dR);
      }

      SetHeading(rotation);
   }


   ////////////////////////////////////////////////////////////////////////////////
   float CharacterWrapper::GetHeading() const
   {
      osg::Vec3 rot;
      dtCore::Transform trans;
      BaseClass::GetTransform(trans);
      trans.GetRotation(rot);
      return rot[0];
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::SetHeading(float degrees)
   {
      osg::Vec3 rot;
      dtCore::Transform trans;
      BaseClass::GetTransform(trans);
      trans.GetRotation(rot);
      rot[0] = degrees;
      trans.SetRotation(rot);
      BaseClass::SetTransform(trans);
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::Transform CharacterWrapper::GetLocalOffset() const
   {
      dtCore::Transform trans;
      trans.Set(mLocalOffset->getMatrix());
      return trans;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::SetLocalOffset(const dtCore::Transform& localOffset)
   {
      osg::Matrix mat;
      localOffset.Get(mat);
      mLocalOffset->setMatrix(mat);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::PlayAnimation(const std::string& pAnim)
   {
      mAnimHelper->PlayAnimation(pAnim);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::ClearAnimation(const std::string& pAnim, float fadeOutTime)
   {
      mAnimHelper->ClearAnimation(pAnim, fadeOutTime);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CharacterWrapper::IsAnimationPlaying(const std::string& name) const
   {
      return mAnimHelper->GetSequenceMixer().IsAnimationPlaying(name);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterWrapper::ClearAllAnimations(float fadeOutTime)
   {
      mAnimHelper->ClearAll(fadeOutTime);
   }

   ////////////////////////////////////////////////////////////////////////////////
   AnimationHelper& CharacterWrapper::GetAnimationHelper()
   {
      return *mAnimHelper;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const AnimationHelper& CharacterWrapper::GetAnimationHelper() const
   {
      return *mAnimHelper;
   }
} // namespace dtAI
