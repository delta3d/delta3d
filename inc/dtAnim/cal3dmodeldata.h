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
#ifndef DELTA_CAL3D_MODEL_DATA
#define DELTA_CAL3D_MODEL_DATA

#include <dtAnim/export.h>
#include <dtCore/refptr.h>

#include <osg/Array>
#include <osg/Referenced>
#include <osg/observer_ptr>

#include <cal3d/global.h>

#include <vector>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class VertexBufferObject;
   class ElementBufferObject;
}
/// @endcond

class CalCoreModel;
class CalHardwareModel;

namespace dtAnim
{
   class AnimationWrapper;
   class Animatable;

   /**
    * A simple data class that stores the configuration options for level of detail.
    */
   class DT_ANIM_EXPORT LODOptions
   {
   public:
      LODOptions();

      ///@return the distance at which to start decreasing the level of detail.
      double GetStartDistance() const { return mStartDistance; }
      void SetStartDistance(double newDistance);

      ///@return the distance at which the level of detail should be the minimum.
      double GetEndDistance() const { return mEndDistance; }
      void SetEndDistance(double newDistance);

      ///@return the maximum distance that the model will be drawn at all.
      double GetMaxVisibleDistance() const { return mMaxVisibleDistance; }
      void SetMaxVisibleDistance(double newDistance);

   private:
      double mStartDistance, mEndDistance, mMaxVisibleDistance;
   };

   class DT_ANIM_EXPORT Cal3DModelData : public osg::Referenced
   {
   public:
      // we will hold all the animation wrappers for each CalCoreModel
      typedef std::vector<dtCore::RefPtr<dtAnim::AnimationWrapper> > AnimationWrapperArray;
      // we will hold a vector of animatables for each CalCoreModel
      typedef std::vector<dtCore::RefPtr<dtAnim::Animatable> > AnimatableArray;

   public:
      Cal3DModelData(CalCoreModel* coreModel, const std::string& filename);

      void Add(AnimationWrapper*);
      void Add(Animatable*);

      void Remove(AnimationWrapper*);
      void Remove(Animatable*);

      const std::string& GetFilename() const;

      CalCoreModel* GetCoreModel();
      const CalCoreModel* GetCoreModel() const;

      AnimationWrapper* GetAnimationWrapperByName(const std::string& name);
      const AnimationWrapper* GetAnimationWrapperByName(const std::string& name) const;

      AnimationWrapperArray& GetAnimationWrappers();
      const AnimationWrapperArray& GetAnimationWrappers() const;

      Animatable* GetAnimatableByName(const std::string& name);
      const Animatable* GetAnimatableByName(const std::string& name) const;

      AnimatableArray& GetAnimatables();
      const AnimatableArray& GetAnimatables() const;

      CalHardwareModel* GetOrCreateCalHardwareModel();

      /**
       * @return the vertex array that stores the raw vertex data
       */
      osg::FloatArray* GetSourceVertexArray() { return mVertexArray; }

      /**
       * @return the vertex array that stores the raw index data
       */
      CalIndex* GetSourceIndexArray() { return mIndexArray; }

      /**
       * @return the number of elements between each successive source vertex row
       */
      int GetStride() { return mStride; }

      /// Allocate memory to be used for the source arrays
      void CreateSourceArrays(int numberOfVertices, int numberOfIndices, int stride);

      /// Deletes the memory held by the vertex and index source arrays
      void DestroySourceArrays();

      /**
       * @return the vbo being used with this character core model, or 0 for none.
       */
      osg::VertexBufferObject* GetVertexBufferObject();

      /// Sets the Vertex Buffer Object being used with this character core model
      void SetVertexBufferObject(osg::VertexBufferObject* vbo);

      /**
       * @return the ebo being used with this character core model, or 0 for none
       */
      osg::ElementBufferObject* GetElementBufferObject() const;

      /// Sets the Element Buffer Object being used with this character core model
      void SetElementBufferObject(osg::ElementBufferObject* ebo);

      /**
       * @see dtCore::ShaderManager
       * @return the shader group used to lookup the shader for this character model.
       */
      const std::string& GetShaderGroupName() const;

      /// Sets the shader group name
      void SetShaderGroupName(const std::string& groupName);

      /**
       * @see dtCore::ShaderManager
       * @see #GetShaderGroupName
       * @return the name of the shader within the shader group to use.
       */
      const std::string& GetShaderName() const;

      /// Sets the shader group name
      void SetShaderName(const std::string& name);

      const std::string& GetPoseMeshFilename() const;

      void SetPoseMeshFilename(const std::string& name);

      /**
       * @return the maximum number of bones the skinning shader supports.
       */
      unsigned GetShaderMaxBones() const;

      /// Sets the maximum number of bones the shader supports
      void SetShaderMaxBones(unsigned maxBones);

      LODOptions& GetLODOptions() { return mLODOptions; }
      const LODOptions& GetLODOptions() const { return mLODOptions; }

   protected:
      virtual ~Cal3DModelData();

      Cal3DModelData(const Cal3DModelData&); //not implemented
      Cal3DModelData& operator=(const Cal3DModelData&); //not implemented

   private:
      std::string mFilename;
      std::string mShaderName, mShaderGroupName;
      std::string mPoseMeshFilename;

      // Buffer data
      int mStride;
      CalIndex* mIndexArray;
      osg::ref_ptr<osg::FloatArray> mVertexArray;
      osg::observer_ptr<osg::VertexBufferObject> mVertexBufferObject;
      osg::observer_ptr<osg::ElementBufferObject> mElementBufferObject;

      CalCoreModel* mCoreModel;
      CalHardwareModel* mHardwareModel;
      AnimationWrapperArray mAnimWrappers;
      AnimatableArray mAnimatables;

      unsigned mShaderMaxBones;

      LODOptions mLODOptions;
   };

} // namespace dtAnim

#endif /*DELTA_CAL3D_MODEL_DATA*/
