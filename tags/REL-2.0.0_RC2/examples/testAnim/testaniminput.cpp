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
 * Bradley Anderegg
 */
#include "testaniminput.h"

#include <dtUtil/macros.h>
#include <dtABC/application.h>
#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtAnim/animationhelper.h>
#include <dtUtil/matrixutil.h>
#include <dtCore/transform.h>
#include <osg/Matrix>
#include <osg/Vec3>
#include <dtGame/logtag.h>

////////////////////////////////////////////////////////////////////
TestAnimInput::TestAnimInput(const std::string &name) 
: dtGame::BaseInputComponent(name)
, mSpeed(2.0f)
, mTurnRate(1.01f)
, mTurnDirection(1.0f)
, mKeyIsPressed(false)
, mIsWalking(false)
, mIsTurning(false)
{
  
}

////////////////////////////////////////////////////////////////////////
TestAnimInput::~TestAnimInput()
{

}

//////////////////////////////////////////////////////////////////////////
void TestAnimInput::SetSpeed(float speed)
{
   mSpeed = speed;
}

//////////////////////////////////////////////////////////////////////////
void TestAnimInput::SetTurnRate(float turn)
{
   mTurnRate = turn;
}

//////////////////////////////////////////////////////////////////////////
bool TestAnimInput::HandleKeyPressed(const dtCore::Keyboard *keyBoard, int key)
{
   bool handled = true;
   
   switch(key)
   {

      case osgGA::GUIEventAdapter::KEY_Escape:
         {            
            GetGameManager()->GetApplication().Quit();
            return true;
         }

      case osgGA::GUIEventAdapter::KEY_Right:
         {
            mIsTurning = true;
            mTurnDirection = -1.0f;
            return false;
         }
      case osgGA::GUIEventAdapter::KEY_Left:
         {
            mIsTurning = true;
            mTurnDirection = 1.0f;
            return false;
         }
      case '\\':
      case osgGA::GUIEventAdapter::KEY_Insert:
         {
//            GetGameManager()->GetApplication().GetCamera()->SetNextStatisticsType();
            break;
         }
      case osgGA::GUIEventAdapter::KEY_Up:
         {            
            if(!mIsWalking)
            {
               mAnimationHelper->ClearAll(0.25f);
               mAnimationHelper->PlayAnimation("RunWalk");
               mIsWalking = true;
            }
            return false;
         }
      case osgGA::GUIEventAdapter::KEY_Page_Up:
         {            
            dtCore::Camera* camera = GetGameManager()->GetApplication().GetCamera();
            float oldLODScale = camera->GetLODScale();
            camera->SetLODScale(oldLODScale * 1.1);
            return false;
         }
      case osgGA::GUIEventAdapter::KEY_Page_Down:
         {            
            dtCore::Camera* camera = GetGameManager()->GetApplication().GetCamera();
            float oldLODScale = camera->GetLODScale();
            camera->SetLODScale(oldLODScale / 1.1);
            return false;
         }
      default:
         break;
   };

   if(!handled)
      return GetGameManager()->GetApplication().KeyPressed(keyBoard, key);

   return handled;
}

//////////////////////////////////////////////////////////////////////////
bool TestAnimInput::HandleKeyReleased(const dtCore::Keyboard* keyboard, int key)
{
   bool handled = true;
   
   switch(key)
   {
      case osgGA::GUIEventAdapter::KEY_Up:
         {                        
            mAnimationHelper->ClearAll(0.25f);
            mAnimationHelper->PlayAnimation("Idle");
            mIsWalking = false;
            return false;
         }
      case osgGA::GUIEventAdapter::KEY_Right:
      case osgGA::GUIEventAdapter::KEY_Left:
         {
            mIsTurning = false;
            return false;
         }
      default:
         break;
   };

   if(!handled)
      return GetGameManager()->GetApplication().KeyPressed(keyboard, key);

   return handled;
}

////////////////////////////////////////////////////////////////////////
void TestAnimInput::SetPlayerActor(dtGame::GameActorProxy &gap)
{
   mPlayer = &gap;
}

////////////////////////////////////////////////////////////////////////
void TestAnimInput::SetAnimationHelper(dtAnim::AnimationHelper& pHelper)
{
   mAnimationHelper = &pHelper;
   mAnimationHelper->ClearAnimation("Idle", 0.25f);
}

////////////////////////////////////////////////////////////////////////
void TestAnimInput::ProcessMessage(const dtGame::Message& message)
{

   if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
   {
      const dtGame::TickMessage& mess = static_cast<const dtGame::TickMessage&>(message);
      TickLocal(mess.GetDeltaSimTime());
   }
}

////////////////////////////////////////////////////////////////////////
void TestAnimInput::TickLocal(float dt)
{
   
   dtGame::GameActor* pActor = &mPlayer->GetGameActor();   

   dtCore::Transform trans;
   osg::Matrix mat;

   osg::Vec3 pos, up, forward;

   if(mIsWalking || mIsTurning)
   {
      pActor->GetTransform(trans);
      trans.Get(mat);
      
      up = dtUtil::MatrixUtil::GetRow3(mat, 2);
      forward = dtUtil::MatrixUtil::GetRow3(mat, 1);

      pos = trans.GetTranslation();
   }

      
   if(mIsTurning)
   {
      float angle = dt * mTurnRate * mTurnDirection;
      osg::Quat quat(angle, up);
      forward = quat * forward;

      dtUtil::MatrixUtil::SetRow(mat, forward, 1);
      mat.orthoNormalize(mat);
      trans.Set(mat);
   }

   if(mIsWalking)
   { 
      float dist = -1.0f * (mSpeed * dt);
      pos += forward * dist;  

      trans.SetTranslation(pos);      
   }

   if(mIsWalking || mIsTurning)
   {
      pActor->SetTransform(trans, dtCore::Transformable::REL_CS);
   }
}

