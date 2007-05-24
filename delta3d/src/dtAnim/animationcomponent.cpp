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
 * Bradley Anderegg 05/11/2007
 */
#include <dtAnim/animationcomponent.h>
#include <dtAnim/cal3danimator.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/sequencemixer.h>
#include <dtAnim/skeletalconfiguration.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/animdriver.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/animationcontroller.h>

#include <dtCore/batchisector.h>

#include <dtDAL/actortype.h>

#include <dtGame/gameactor.h>
#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>

#include <dtUtil/log.h>

namespace dtAnim
{

const std::string AnimationComponent::DEFAULT_NAME("Animation Component");

/////////////////////////////////////////////////////////////////////////////////
AnimationComponent::AnimationComponent(const std::string& name)
: BaseClass(name)
, mRegisteredActors()
, mTerrainActor(0)
, mIsector(0)
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationComponent::~AnimationComponent()
{
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::ProcessMessage(const dtGame::Message &message)
{
   if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
   {
      const dtGame::TickMessage& mess = static_cast<const dtGame::TickMessage&>(message);
      TickLocal(mess.GetDeltaSimTime());
   }
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::TickLocal(float dt)
{
   AnimCompIter end = mRegisteredActors.end();

   for(AnimCompIter iter = mRegisteredActors.begin(); iter != end; ++iter)
   {
      (*iter).second->Update(dt);
   }

   GroundClamp();
}

/////////////////////////////////////////////////////////////////////////////////
const dtAnim::AnimationHelper* AnimationComponent::GetHelperForProxy(dtGame::GameActorProxy &proxy) const
{
   const AnimCompMap::const_iterator iter = mRegisteredActors.find(proxy.GetId());
   if(iter == mRegisteredActors.end())
   {
      LOG_ERROR("Unable to find dtAnim::AnimationHelper for GameActorProxy.");
      return 0;
   }
   else
   {
      return (*iter).second.get();
   }

}

/////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::RegisterActor(dtGame::GameActorProxy& proxy, dtAnim::AnimationHelper& helper)
{
   AnimCompIter iter = mRegisteredActors.find(proxy.GetId());
   if(iter != mRegisteredActors.end())
   {
      LOG_ERROR("GameActor already registered with Animation Component.");
   }
   else
   {
      mRegisteredActors.insert(AnimCompMapping(proxy.GetId(), &helper));
   }
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::UnregisterActor(dtGame::GameActorProxy& proxy)
{
   AnimCompIter iter = mRegisteredActors.find(proxy.GetId());
   if(iter == mRegisteredActors.end())
   {
      LOG_ERROR("Unable to find dtAnim::AnimationHelper for GameActorProxy.");
   }
   else
   {
      mRegisteredActors.erase(iter);
   }
}

/////////////////////////////////////////////////////////////////////////////////
bool AnimationComponent::IsRegisteredActor(dtGame::GameActorProxy& proxy)
{
   AnimCompIter iter = mRegisteredActors.find(proxy.GetId());
   if(iter == mRegisteredActors.end())
   {
      return false;
   }
   else
   {
      return true;
   }
}

/////////////////////////////////////////////////////////////////////////////////
dtCore::Transformable* AnimationComponent::GetTerrainActor()
{
   return mTerrainActor.get();
}

/////////////////////////////////////////////////////////////////////////////////
const dtCore::Transformable* AnimationComponent::GetTerrainActor() const
{
   return mTerrainActor.get();
};

/////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::SetTerrainActor(dtCore::Transformable* newTerrain)
{
   mTerrainActor = newTerrain;

   if(!mIsector.valid())
   {
      mIsector = new dtCore::BatchIsector();
   }   

   mIsector->SetQueryRoot(mTerrainActor.get());
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::GroundClamp()
{
   if(mTerrainActor.valid() && mIsector.valid())
   {
      dtCore::Transform trans;
      osg::Matrix mat;
      osg::Vec3 pos, hitPt;
      
      dtGame::GameManager* gm = GetGameManager();
        
      AnimCompIter end = mRegisteredActors.end();

      for(AnimCompIter iter = mRegisteredActors.begin(); iter != end; ++iter)
      {
         AnimationHelper* helper = (*iter).second.get();

         if(helper->GetGroundClamp())
         {
            dtGame::GameActorProxy* pProxy = gm->FindGameActorById((*iter).first);   
            if(pProxy)
            {
               dtGame::GameActor* pActor = &pProxy->GetGameActor();

               pActor->GetTransform(trans);            
               pos = trans.GetTranslation();      

               mIsector->EnableAndGetISector(0).SetSectorAsLineSegment(osg::Vec3(pos[0], pos[1], pos[2] + 10.0f), osg::Vec3(pos[0], pos[1], pos[2] - 10.0f));
               if(mIsector->Update(pos, true))
               {
                  mIsector->GetSingleISector(0).GetHitPoint(hitPt);
                  pos[2] = hitPt[2];
                  trans.SetTranslation(pos);
                  pActor->SetTransform(trans);//, dtCore::Transformable::REL_CS);
               }
            }
         }
      }

   }
}

}//namespace dtGame
