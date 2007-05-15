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
 * Bradley Anderegg 03/30/2007
 */

#include <dtAnim/animationhelper.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/cal3dloader.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3danimator.h>
#include <dtAnim/ical3ddriver.h>
#include <dtAnim/sequencemixer.h>
#include <dtAnim/skeletalconfiguration.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/animationcontroller.h>

#include <dtDAL/actorproperty.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxy.h>

#include <dtUtil/log.h>

#include <osg/Geode>
#include <osg/Texture2D>


namespace dtAnim
{

dtCore::RefPtr<Cal3DLoader> AnimationHelper::sModelLoader = new Cal3DLoader();


AnimationHelper::AnimationHelper()
: mGeode(0)
, mAnimator(0)
, mNodeBuilder(new AnimNodeBuilder())
, mSequenceMixer(new SequenceMixer())
, mSkeleton(new SkeletalConfiguration())
{
}


AnimationHelper::~AnimationHelper()
{
}

void AnimationHelper::Update(float dt)
{
   if(mAnimator.valid())
   {
      mSequenceMixer->Update(dt);
      mAnimator->Update(dt);
   }
}

void AnimationHelper::PlayAnimation(const std::string& pAnim)
{
   mSequenceMixer->PlayAnimation(pAnim);
}

void AnimationHelper::ClearAnimation(const std::string& pAnim, float fadeOutTime)
{
   mSequenceMixer->ClearAnimation(pAnim, fadeOutTime);
}


void AnimationHelper::LoadModel(const std::string& pFilename)
{
      dtCore::RefPtr<dtAnim::Cal3DModelWrapper> newModel = sModelLoader->Load(pFilename);

      if (newModel.valid())
      {
         mAnimator = new dtAnim::Cal3DAnimator(newModel.get());   
         mGeode = mNodeBuilder->CreateGeode(newModel.get());
      }
      else
      {
         LOG_ERROR("Unable to load skeletal resource: " + pFilename);
      }
}

void AnimationHelper::GetActorProperties(dtDAL::ActorProxy& pProxy, std::vector<dtCore::RefPtr<dtDAL::ActorProperty> >& pFillVector)
{
   pFillVector.push_back(new dtDAL::ResourceActorProperty(pProxy, dtDAL::DataType::SKELETAL_MESH,
      "Skeletal Mesh", "Skeletal Mesh", dtDAL::MakeFunctor(*this, &AnimationHelper::LoadModel),
      "The model resource that defines the skeletal mesh", "AnimationModel"));     
}

osg::Geode* AnimationHelper::GetGeode()
{
   return mGeode.get();
}

const osg::Geode* AnimationHelper::GetGeode() const
{
   return mGeode.get();
}

Cal3DAnimator* AnimationHelper::GetAnimator()
{
   return mAnimator.get();
}

const Cal3DModelWrapper* AnimationHelper::GetModelWrapper() const
{
   return mAnimator->GetWrapper();
}

Cal3DModelWrapper* AnimationHelper::GetModelWrapper()
{
   return mAnimator->GetWrapper();
}

const Cal3DAnimator* AnimationHelper::GetAnimator() const
{
   return mAnimator.get();
}

SequenceMixer* AnimationHelper::GetSequenceMixer()
{
   return mSequenceMixer.get();
}

const SequenceMixer* AnimationHelper::GetSequenceMixer() const
{
   return mSequenceMixer.get();
}

SkeletalConfiguration* AnimationHelper::GetSkeletalConfiguration()
{
   return mSkeleton.get();
}

const SkeletalConfiguration* AnimationHelper::GetSkeletalConfiguration() const
{
   return mSkeleton.get();
}


}//namespace dtAnim
