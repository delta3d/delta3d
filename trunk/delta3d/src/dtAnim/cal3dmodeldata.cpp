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
      , mIndexArray(NULL)
      , mVertexArray(NULL)
      , mStride(-1)
      , mCoreModel(coreModel)
      , mHardwareModel(NULL)
      , mAnimWrappers()
      , mAnimatables()
      , mVertexBufferObject(0)
      , mElementBufferObject(0)
      , mShaderMaxBones(72)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::~Cal3DModelData()
   {
      mCoreModel = 0;
      mAnimWrappers.clear();
      mAnimatables.clear();
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
         mVertexArray = new float[stride * numberOfVertices];  

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
      delete[] mVertexArray; mVertexArray = NULL;
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
