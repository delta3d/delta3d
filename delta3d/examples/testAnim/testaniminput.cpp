/* -*-c++-*-
 * testAnim - testaniminput (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007-2008, Alion Science and Technology Corporation
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
 * Bradley Anderegg
 */

#include "testaniminput.h"

#include <dtABC/application.h>
#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/animationgameactor.h>
#include <dtAnim/animactorregistry.h>
#include <dtAnim/animationcomponent.h>
#include <dtUtil/matrixutil.h>
#include <dtCore/transform.h>
#include <osg/Matrix>
#include <osg/Vec3>
#include <dtGame/logtag.h>

const dtCore::RefPtr<dtCore::SystemComponentType> TestAnimInput::TYPE(new dtCore::SystemComponentType("TestAnimInput","GMComponents",
      "Test dtAnim input component",
      dtGame::BaseInputComponent::DEFAULT_TYPE));

////////////////////////////////////////////////////////////////////
TestAnimInput::TestAnimInput()
   : dtGame::BaseInputComponent(*TYPE)
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
bool TestAnimInput::HandleKeyPressed(const dtCore::Keyboard* keyBoard, int key)
{
   bool handled = false;

   switch (key)
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
      case osgGA::GUIEventAdapter::KEY_Up:
         {
            if (!mIsWalking)
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
      case '\\':
      case osgGA::GUIEventAdapter::KEY_Insert:
         {
            dtABC::Application& app = GetGameManager()->GetApplication();
            app.SetNextStatisticsType();
            return true;
         }
      default:
         break;
   };

   if (!handled)
   {
      return GetGameManager()->GetApplication().KeyPressed(keyBoard, key);
   }

   return handled;
}

//////////////////////////////////////////////////////////////////////////
bool TestAnimInput::HandleKeyReleased(const dtCore::Keyboard* keyboard, int key)
{
   bool handled = false;

   switch (key)
   {
      case osgGA::GUIEventAdapter::KEY_Up:
         {
            mAnimationHelper->ClearAll(0.25f);
            mAnimationHelper->PlayAnimation("Idle");
            mIsWalking = false;
            break;
         }
      case osgGA::GUIEventAdapter::KEY_Right:
      case osgGA::GUIEventAdapter::KEY_Left:
         {
            mIsTurning = false;
            break;
         }
      case '/':
         {

            // Test code to dynamically load a model.  This can be used to test the 
            // benefits of using asynchronous vs synchronous loading.

            //dtGame::GameManager* gm = GetGameManager();

            //dtCore::RefPtr<dtAnim::AnimationGameActorProxy> animProxy;

            //// Create new marine
            //gm->CreateActor(*dtAnim::AnimActorRegistry::ANIMATION_ACTOR_TYPE, animProxy);

            //dtAnim::AnimationGameActor* dynamicActor;
            //dynamicActor = dynamic_cast<dtAnim::AnimationGameActor*>(animProxy->GetDrawable());
            //dynamicActor->SetModel("yourModelURL");

            //dtCore::Transform newTransform;
            //newTransform.SetTranslation(-1.0f, 50.0f, 4.4f);

            //dynamicActor->SetTransform(newTransform);

            //// Add marine to world
            //gm->AddActor(*animProxy, false, false); 

            //dtAnim::AnimationComponent* animComponent;
            //gm->GetComponentByName(dtAnim::AnimationComponent::DEFAULT_NAME, animComponent);

            //animComponent->RegisterActor(*animProxy, *dynamicActor->GetHelper());
         }
      default:
         break;
   };

   if (!handled)
   {
      return GetGameManager()->GetApplication().KeyPressed(keyboard, key);
   }

   return handled;
}

////////////////////////////////////////////////////////////////////////
void TestAnimInput::SetPlayerActor(dtGame::GameActorProxy& gap)
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
   if (!mPlayer.valid())
   {
      return;
   }

   dtCore::Transformable* pDrawable = mPlayer->GetDrawable<dtCore::Transformable>();

   dtCore::Transform trans;
   osg::Matrix mat;

   osg::Vec3 pos, up, forward;

   if (mIsWalking || mIsTurning)
   {
      pDrawable->GetTransform(trans);
      trans.Get(mat);

      up = dtUtil::MatrixUtil::GetRow3(mat, 2);
      forward = dtUtil::MatrixUtil::GetRow3(mat, 1);

      trans.GetTranslation(pos);
   }


   if (mIsTurning)
   {
      const float angle = dt * mTurnRate * mTurnDirection;

      trans.GetRotation(mat);
      trans.SetRotation(mat * osg::Matrix::rotate(angle, up));
   }


   if (mIsWalking)
   {
      float dist = -1.0f * (mSpeed * dt);
      pos += forward * dist;

      trans.SetTranslation(pos);
   }

   if (mIsWalking || mIsTurning)
   {
      pDrawable->SetTransform(trans, dtCore::Transformable::REL_CS);
   }
}

