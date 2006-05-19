/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Matthew W. Campbell
 */

#include "testplayer.h"
#include <osgDB/ReadFile>
#include <dtCore/isector.h>
#include <dtGame/messagetype.h>
#include <dtGame/gamemanager.h>
#include <dtGame/basemessages.h>
#include <dtGame/actorupdatemessage.h>
#include <dtDAL/enginepropertytypes.h>

#include <osg/io_utils>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////
TestPlayer::TestPlayer(dtGame::GameActorProxy& proxy): dtGame::GameActor(proxy),
                                                       mIsector(new dtCore::Isector())
{
   mVelocity = 0.0f;
   mTurnRate = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////
TestPlayer::~TestPlayer()
{
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayer::TickLocal(const dtGame::Message &tickMessage)
{
   const dtGame::TickMessage &tick =
      static_cast<const dtGame::TickMessage&>(tickMessage);

   HandleTick(tick.GetDeltaSimTime());
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayer::TickRemote(const dtGame::Message &tickMessage)
{
   const dtGame::TickMessage &tick =
      static_cast<const dtGame::TickMessage&>(tickMessage);
   HandleTick(tick.GetDeltaSimTime());
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayer::HandleTick(double deltaSimTime, bool forceGroundClamp)
{
   if (deltaSimTime == 0.0f)
      return;

   if (forceGroundClamp || mVelocity > 0.0001f || mVelocity < -0.0001f ||
       mTurnRate > 0.0001f || mTurnRate < -0.0001f)
   {
      dtCore::Transform tx;
      osg::Matrix mat;
      osg::Quat q;
      osg::Vec3 viewDir;

      GetTransform(&tx);
      tx.GetRotation(mat);
      mat.get(q);

      viewDir = q*osg::Vec3(0,-1,0);

      //Translate the player along its current view direction based on its
      //current velocity.
      osg::Vec3 pos;
      tx.GetTranslation(pos);
      pos = pos + (viewDir*(mVelocity*deltaSimTime));

      // attempt to ground clamp the actor so that he doesn't go through
      // mountains.
      osg::Vec3 intersection;
      mIsector->Reset();
      mIsector->SetScene(GetSceneParent());
      mIsector->SetStartPosition(osg::Vec3(pos.x(),pos.y(),-10000));
      mIsector->SetDirection(osg::Vec3(0,0,1));
      if (mIsector->Update())
      {
         //std::cout << "Got an intersection!" << std::endl;
         osgUtil::IntersectVisitor &iv = mIsector->GetIntersectVisitor();
         osg::Vec3 p = iv.getHitList(mIsector->GetLineSegment())[0].getWorldIntersectPoint();
         pos.z() = p.z() + 2.55f;
      }

      osg::Vec3 xyz = GetGameActorProxy().GetRotation();
      xyz[2] += 360.0f*mTurnRate*deltaSimTime;
      if (xyz[2] > 360.0f)
         xyz[2] -= 360.0f;

      tx.SetTranslation(pos);
      SetTransform(&tx);
      GetGameActorProxy().SetRotation(xyz);
   }

}

//////////////////////////////////////////////////////////////////////////////
void TestPlayer::SetModel(const std::string &fileName)
{
   if (mModelFile != fileName)
   {
      dtCore::RefPtr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options();

      options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);
      osg::Node *model = osgDB::readNodeFile(fileName,options.get());
      if (model != NULL)
      {
         if (GetMatrixNode()->getNumChildren() != 0)
            GetMatrixNode()->removeChild(0,GetMatrixNode()->getNumChildren());
         GetMatrixNode()->addChild(model);
         mModelFile = fileName;
      }
      else
      {
         LOG_ERROR("Unable to load model file: " + fileName);
      }

      //the game manager is not set when this property is first set at map load time.
      if (!IsRemote() && GetGameActorProxy().GetGameManager() != NULL)
      {
         dtCore::RefPtr<dtGame::Message> updateMsg = GetGameActorProxy().GetGameManager()->
            GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
         dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *> (updateMsg.get());
         GetGameActorProxy().PopulateActorUpdate(*message);

         GetGameActorProxy().GetGameManager()->ProcessMessage(*updateMsg);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayer::SetVelocity(float velocity)
{
   if (mVelocity != velocity)
   {
      mVelocity = velocity;
      // if local, then we need to do an actor update - let the world know.
      //the game manager is not set when this property is first set at map load time.
      if (!IsRemote() && GetGameActorProxy().GetGameManager() != NULL)
      {
         dtCore::RefPtr<dtGame::Message> updateMsg = GetGameActorProxy().GetGameManager()->
            GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
         dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *> (updateMsg.get());
         GetGameActorProxy().PopulateActorUpdate(*message);

         GetGameActorProxy().GetGameManager()->ProcessMessage(*updateMsg);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
float TestPlayer::GetVelocity() const
{
   return mVelocity;
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayer::SetTurnRate(float rate)
{
   if (mTurnRate != rate)
   {
      mTurnRate = rate;

      //the game manager is not set when this property is first set at map load time.
      if (!IsRemote() && GetGameActorProxy().GetGameManager() != NULL)
      {
         dtCore::RefPtr<dtGame::Message> updateMsg = GetGameActorProxy().GetGameManager()->
            GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
         dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *> (updateMsg.get());
         GetGameActorProxy().PopulateActorUpdate(*message);

         GetGameActorProxy().GetGameManager()->ProcessMessage(*updateMsg);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
float TestPlayer::GetTurnRate() const
{
   return mTurnRate;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
TestPlayerProxy::TestPlayerProxy()
{
   SetClassName("TestPlayer");
   mEnteredBefore = false;
}

//////////////////////////////////////////////////////////////////////////////
TestPlayerProxy::~TestPlayerProxy()
{
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayerProxy::BuildPropertyMap()
{
   dtGame::GameActorProxy::BuildPropertyMap();

   TestPlayer &player = static_cast<TestPlayer &>(GetGameActor());
   AddProperty(new dtDAL::StringActorProperty("mesh","mesh",
      dtDAL::MakeFunctor(player,&TestPlayer::SetModel),
      dtDAL::MakeFunctorRet(player,&TestPlayer::GetModel)));

   AddProperty(new dtDAL::FloatActorProperty("velocity","velocity",
      dtDAL::MakeFunctor(player,&TestPlayer::SetVelocity),
      dtDAL::MakeFunctorRet(player,&TestPlayer::GetVelocity)));

   AddProperty(new dtDAL::FloatActorProperty("turnrate","turnrate",
      dtDAL::MakeFunctor(player,&TestPlayer::SetTurnRate),
      dtDAL::MakeFunctorRet(player,&TestPlayer::GetTurnRate)));
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayerProxy::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayerProxy::OnEnteredWorld()
{
   if (!mEnteredBefore)
   {
      mEnteredBefore = true;

      TestPlayer &actor = static_cast<TestPlayer&>(GetGameActor());

      //std::cout << "Here in on entered world.  Attempting to ground clamp." << std::endl;
      actor.HandleTick(1.0,true);

      //enable receiving tick messages.
      if (IsRemote())
      {
         GetGameManager()->RegisterGlobalMessageListener(dtGame::MessageType::TICK_REMOTE,
            *this,"Tick Remote");
      }
      else
      {
         GetGameManager()->RegisterGlobalMessageListener(dtGame::MessageType::TICK_LOCAL,
            *this,"Tick Local");
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayerProxy::CreateActor()
{
   mActor = new TestPlayer(*this);
}
