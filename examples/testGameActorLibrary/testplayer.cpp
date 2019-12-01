/* -*-c++-*-
* testGameActorLibrary - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* Matthew W. Campbell
*/

#include "testplayer.h"

#include <dtCore/floatactorproperty.h>
#include <dtCore/stringactorproperty.h>

#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>
#include <dtCore/transform.h>

#include <osg/MatrixTransform>

#include <osgDB/ReadFile>

//////////////////////////////////////////////////////////////////////////////
TestPlayer::TestPlayer(dtGame::GameActorProxy& parent): dtGame::GameActor(parent),
                                                       mIsector(new dtCore::Isector())
{
   mVelocity = 0.0f;
   mTurnRate = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////
TestPlayer::~TestPlayer()
{
   mIsector = NULL;
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayer::OnTickLocal(const dtGame::TickMessage& tickMessage)
{
   const dtGame::TickMessage &tick =
      static_cast<const dtGame::TickMessage&>(tickMessage);

   HandleTick(tick.GetDeltaSimTime());
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayer::OnTickRemote(const dtGame::TickMessage& tickMessage)
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

      GetTransform(tx);
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
         pos.z() = p.z() + 1.0f; //2.55f;
      }

      osg::Vec3 xyz = GetGameActorProxy().GetRotation();
      xyz[2] += 360.0f*mTurnRate*deltaSimTime;
      if (xyz[2] > 360.0f)
         xyz[2] -= 360.0f;

      tx.SetTranslation(pos);
      SetTransform(tx);
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
      if (!IsRemote() && GetGameActorProxy().IsInGM())
      {
         dtCore::RefPtr<dtGame::Message> updateMsg = GetGameActorProxy().GetGameManager()->
            GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
         dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *> (updateMsg.get());
         GetGameActorProxy().PopulateActorUpdate(*message);

         GetGameActorProxy().GetGameManager()->SendMessage(*updateMsg);
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
      if (!IsRemote() && GetGameActorProxy().IsInGM())
      {
         dtCore::RefPtr<dtGame::Message> updateMsg = GetGameActorProxy().GetGameManager()->
            GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
         dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *> (updateMsg.get());
         GetGameActorProxy().PopulateActorUpdate(*message);

         GetGameActorProxy().GetGameManager()->SendMessage(*updateMsg);
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
      if (!IsRemote() && GetGameActorProxy().IsInGM())
      {
         dtCore::RefPtr<dtGame::Message> updateMsg = GetGameActorProxy().GetGameManager()->
            GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
         dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *> (updateMsg.get());
         GetGameActorProxy().PopulateActorUpdate(*message);

         GetGameActorProxy().GetGameManager()->SendMessage(*updateMsg);
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

   TestPlayer* player = NULL;
   GetDrawable(player);

   AddProperty(new dtCore::StringActorProperty("mesh","mesh",
      dtCore::StringActorProperty::SetFuncType(player,&TestPlayer::SetModel),
      dtCore::StringActorProperty::GetFuncType(player,&TestPlayer::GetModel)));

   AddProperty(new dtCore::FloatActorProperty("velocity","velocity",
      dtCore::FloatActorProperty::SetFuncType(player,&TestPlayer::SetVelocity),
      dtCore::FloatActorProperty::GetFuncType(player,&TestPlayer::GetVelocity)));

   AddProperty(new dtCore::FloatActorProperty("turnrate","turnrate",
      dtCore::FloatActorProperty::SetFuncType(player,&TestPlayer::SetTurnRate),
      dtCore::FloatActorProperty::GetFuncType(player,&TestPlayer::GetTurnRate)));
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

      TestPlayer* actor = GetDrawable<TestPlayer>();

      //std::cout << "Here in on entered world.  Attempting to ground clamp." << std::endl;
      actor->HandleTick(1.0,true);

      //enable receiving tick messages.
      if (IsRemote())
      {
         GetGameManager()->RegisterForMessages(dtGame::MessageType::TICK_REMOTE,
            *this,"Tick Remote");
      }
      else
      {
         GetGameManager()->RegisterForMessages(dtGame::MessageType::TICK_LOCAL,
            *this,"Tick Local");
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
void TestPlayerProxy::CreateDrawable()
{
   SetDrawable(*new TestPlayer(*this));
}
