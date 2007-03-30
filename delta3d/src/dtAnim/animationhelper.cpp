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

#include <dtDAL/actorproperty.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxy.h>

#include <dtUtil/log.h>

#include <osg/Geode>
#include <osg/Texture2D>


namespace dtAnim
{

AnimationHelper::AnimationHelper(AnimNodeBuilder* pBuilder)
: mGeode(0)
, mAnimator(0)
, mNodeBuilder(pBuilder)
{
}


AnimationHelper::~AnimationHelper()
{
}

osg::Geode* AnimationHelper::GetGeode()
{
   return mGeode.get();
}

Cal3DAnimator* AnimationHelper::GetAnimator()
{
   return mAnimator.get();
}

void AnimationHelper::LoadModel(const std::string& pFilename)
{
      dtAnim::Cal3DLoader pLoader;
      dtCore::RefPtr<dtAnim::Cal3DModelWrapper> newModel = pLoader.Load(pFilename);

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


}//namespace dtAnim
