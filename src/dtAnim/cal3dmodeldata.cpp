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
 * Bradley Anderegg 07/17/2007
 */
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/animatable.h>
#include <dtAnim/animationwrapper.h>
#include <dtUtil/log.h>

#include <osg/BufferObject>

#include <cal3d/coremodel.h>
#include <cal3d/hardwaremodel.h>

#include <algorithm>

 namespace dtAnim
 {
   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::Cal3DModelData(CalCoreModel* coreModel, const std::string& filename)
      : mFilename(filename)
      , mStride(-1)
      , mIndexArray(NULL)
      , mVertexArray(NULL)
      , mVertexBufferObject(NULL)
      , mElementBufferObject(NULL)
      , mCoreModel(coreModel)
      , mHardwareModel(NULL)
      , mAnimWrappers()
      , mAnimatables()
      , mShaderMaxBones(72)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::~Cal3DModelData()
   {
      mCoreModel = 0;
      mAnimWrappers.clear();
      mAnimatables.clear();

      DestroySourceArrays();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::Add(AnimationWrapper* wrapper)
   {
      mAnimWrappers.push_back(wrapper);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::Add(Animatable* anim)
   {
      mAnimatables.push_back(anim);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::Remove(AnimationWrapper* wrapper)
   {
      mAnimWrappers.erase(std::remove(mAnimWrappers.begin(), mAnimWrappers.end(), wrapper));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::Remove(Animatable* anim)
   {
      mAnimatables.erase(std::remove(mAnimatables.begin(), mAnimatables.end(), anim));
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelData::GetFilename() const
   {
      return mFilename;
   }

   ////////////////////////////////////////////////////////////////////////////////
   CalCoreModel* Cal3DModelData::GetCoreModel()
   {
      return mCoreModel;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const CalCoreModel* Cal3DModelData::GetCoreModel() const
   {
      return mCoreModel;
   }

   ////////////////////////////////////////////////////////////////////////////////
   AnimationWrapper* Cal3DModelData::GetAnimationWrapperByName(const std::string& name)
   {
      AnimationWrapper* anim = NULL;

      AnimationWrapper* curAnim = NULL;
      AnimationWrapperArray::iterator curIter = mAnimWrappers.begin();
      AnimationWrapperArray::iterator endIter = mAnimWrappers.end();
      for (;curIter != endIter; ++curIter)
      {
         curAnim = curIter->get();
         if(curAnim->GetName() == name)
         {
            anim = curAnim;
            break;
         }
      }

      return anim;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const AnimationWrapper* Cal3DModelData::GetAnimationWrapperByName(const std::string& name) const
   {
      const AnimationWrapper* anim = NULL;

      const AnimationWrapper* curAnim = NULL;
      AnimationWrapperArray::const_iterator curIter = mAnimWrappers.begin();
      AnimationWrapperArray::const_iterator endIter = mAnimWrappers.end();
      for (;curIter != endIter; ++curIter)
      {
         curAnim = curIter->get();
         if(curAnim->GetName() == name)
         {
            anim = curAnim;
            break;
         }
      }

      return anim;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::AnimationWrapperArray& Cal3DModelData::GetAnimationWrappers()
   {
      return mAnimWrappers;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Cal3DModelData::AnimationWrapperArray& Cal3DModelData::GetAnimationWrappers() const
   {
      return mAnimWrappers;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Animatable* Cal3DModelData::GetAnimatableByName(const std::string& name)
   {
      Animatable* anim = NULL;

      Animatable* curAnim = NULL;
      AnimatableArray::iterator curIter = mAnimatables.begin();
      AnimatableArray::iterator endIter = mAnimatables.end();
      for (;curIter != endIter; ++curIter)
      {
         curAnim = curIter->get();
         if(curAnim->GetName() == name)
         {
            anim = curAnim;
            break;
         }
      }

      return anim;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Animatable* Cal3DModelData::GetAnimatableByName(const std::string& name) const
   {
      const Animatable* anim = NULL;

      Animatable* curAnim = NULL;
      AnimatableArray::const_iterator curIter = mAnimatables.begin();
      AnimatableArray::const_iterator endIter = mAnimatables.end();
      for (;curIter != endIter; ++curIter)
      {
         curAnim = curIter->get();
         if(curAnim->GetName() == name)
         {
            anim = curAnim;
            break;
         }
      }

      return anim;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::AnimatableArray& Cal3DModelData::GetAnimatables()
   {
      return mAnimatables;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Cal3DModelData::AnimatableArray& Cal3DModelData::GetAnimatables() const
   {
      return mAnimatables;
   }

   /////////////////////////////////////////////////////////////////////////////
   CalHardwareModel* Cal3DModelData::GetOrCreateCalHardwareModel()
   {      
      if (!mHardwareModel)
      {
         // We need a core model to make the hardware model
         if (mCoreModel)
         {
            mHardwareModel = new CalHardwareModel(mCoreModel);
         }
         else
         {
            LOG_ERROR("Unable to create hardware model.");
         }
      }

      return mHardwareModel;
   }


   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::CreateSourceArrays(int numberOfVertices, int numberOfIndices, int stride)
   {
      if (!mVertexArray && !mIndexArray)
      {
         mIndexArray = new CalIndex[numberOfIndices];
         mVertexArray = new osg::FloatArray(stride * numberOfVertices);

         mStride = stride;
      }
      else
      {
         LOG_ERROR("Unable to create source arrays.")
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::DestroySourceArrays()
   {
      mVertexArray = NULL;
      delete[] mIndexArray;  mIndexArray = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::VertexBufferObject* Cal3DModelData::GetVertexBufferObject() 
   {
      return mVertexBufferObject.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetVertexBufferObject(osg::VertexBufferObject* vbo)
   {
      mVertexBufferObject = vbo;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::ElementBufferObject* Cal3DModelData::GetElementBufferObject() const
   {
      return mElementBufferObject.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetElementBufferObject(osg::ElementBufferObject* ebo)
   {
      mElementBufferObject = ebo;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelData::GetShaderGroupName() const
   {
      return mShaderGroupName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetShaderGroupName(const std::string& groupName)
   {
      mShaderGroupName = groupName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelData::GetShaderName() const
   {
      return mShaderName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetShaderName(const std::string& name)
   {
      mShaderName = name;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelData::GetPoseMeshFilename() const
   {
      return mPoseMeshFilename;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetPoseMeshFilename(const std::string& name)
   {
      mPoseMeshFilename = name;
   }
 
   ////////////////////////////////////////////////////////////////////////////////
   unsigned Cal3DModelData::GetShaderMaxBones() const
   {
      return mShaderMaxBones;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetShaderMaxBones(unsigned maxBones)
   {
      mShaderMaxBones = maxBones;
   }

   LODOptions::LODOptions():
      mStartDistance(10.0), mEndDistance(500.0), mMaxVisibleDistance(1000.0)
   {
      
   }

   void LODOptions::SetStartDistance(double newDistance)
   {
      mStartDistance = newDistance;
   }

   void LODOptions::SetEndDistance(double newDistance)
   {
      mEndDistance = newDistance;
   }

   void LODOptions::SetMaxVisibleDistance(double newDistance)
   {
      mMaxVisibleDistance = newDistance;
   }

} //namespace dtAnim
