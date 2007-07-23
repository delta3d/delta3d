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

#include <dtCore/refptr.h>

#include <osg/Referenced>

#include <vector>

class CalCoreModel;

namespace dtAnim
{
   class AnimationWrapper;
   class Animatable;

   class Cal3DModelData: public osg::Referenced
   {
      public:
         //we will hold all the animation wrappers for each CalCoreModel
         typedef std::vector< dtCore::RefPtr<AnimationWrapper> > AnimationWrapperArray;
         //we will hold a vector of animatables for each CalCoreModel
         typedef std::vector<dtCore::RefPtr<Animatable> > AnimatableArray;

      public:
         Cal3DModelData(CalCoreModel* coreModel, const std::string& filename);

         void Add(AnimationWrapper*);
         void Add(Animatable*);

         void Remove(AnimationWrapper*);
         void Remove(Animatable*);

         const std::string& GetFilename() const;

         CalCoreModel* GetCoreModel();
         const CalCoreModel* GetCoreModel() const;

         AnimationWrapperArray& GetAnimationWrappers();
         const AnimationWrapperArray& GetAnimationWrappers() const;

         AnimatableArray& GetAnimatables();
         const AnimatableArray& GetAnimatables() const;

         unsigned GetVertexVBO() const;
         void SetVertexVBO(unsigned);

         unsigned GetIndexVBO() const;
         void SetIndexVBO(unsigned);

      protected:
         virtual ~Cal3DModelData();

         Cal3DModelData(const Cal3DModelData&); //not implemented
         Cal3DModelData& operator=(const Cal3DModelData&); //not implemented

      private:        
         
         std::string mFilename;
         CalCoreModel* mCoreModel;
         AnimationWrapperArray mAnimWrappers;
         AnimatableArray mAnimatables;
         unsigned mVertexVBO, mIndexVBO;

   };

}

#endif /*DELTA_CAL3D_MODEL_DATA*/
