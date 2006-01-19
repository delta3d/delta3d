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

#include "testlogger.h"

#include <dtCore/object.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtGame/clientgamemanager.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/binarylogstream.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/logcontroller.h>
#include <dtGame/logkeyframe.h>
#include <dtGame/logtag.h>
#include <dtUtil/exception.h>

#include <ctime>

//////////////////////////////////////////////////////////////////////////
LoggerApplication::LoggerApplication() : 
   dtABC::Application("testloggerconfig.xml"),
   mClientGM(0),
   mLogController(0),
   mPlayerIsValid(false),
   mPlayer(0),
   mMotionModel(0),
   mCameraMode(CAM_FOLLOW_PLAYER),
   mKeyIsPressed(false),
   mSimSpeedFactor(1.0)
{
   srand(static_cast<unsigned>(time(NULL)));
}

//////////////////////////////////////////////////////////////////////////
LoggerApplication::~LoggerApplication()
{
   
}


//////////////////////////////////////////////////////////////////////////
void LoggerApplication::Config()
{
   mClientGM = new dtGame::ClientGameManager(*GetScene());
   dtGame::BinaryLogStream *logStream = new dtGame::BinaryLogStream(mClientGM->GetMessageFactory());
   dtGame::ServerLoggerComponent *logger = new dtGame::ServerLoggerComponent(*logStream);
      
   dtGame::DefaultMessageProcessor *mp = new dtGame::DefaultMessageProcessor();  

   mLogController = new dtGame::LogController();
   mLogController->SignalReceivedStatus().connect_slot(this, &LoggerApplication::OnReceivedStatus);
   mLogController->SignalReceivedRejection().connect_slot(this, &LoggerApplication::OnReceivedRejection);
   
   mClientGM->AddComponent(*logger);
   mClientGM->AddComponent(*mLogController.get());  
   mClientGM->AddComponent(*mp);

   //Load the library with the test game actors...
   #if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
      mClientGM->LoadActorRegistry("testGameActorLibraryd");
   #else
      mClientGM->LoadActorRegistry("testGameActorLibrary");
   #endif  
   
   SetupScene();
}

//////////////////////////////////////////////////////////////////////////
void LoggerApplication::SetupScene()
{
   mPlayer = CreateNewMovingActor("models/physics_happy_sphere.ive", 0.0, 0.0, false);
   mPlayerIsValid = true;

   dtCore::Object *terrain = new dtCore::Object();
   terrain->LoadFile("models/dirt.ive");
   GetScene()->AddDrawable(terrain);   
   SetCameraMode(CAM_FREE_FLY);  
}

//////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<dtGame::GameActorProxy>
LoggerApplication::CreateNewMovingActor(const std::string &meshName, float velocity, float turnRate, bool bSetLocation)
{
   float xScale = 0.0f, yScale = 0.0f, zScale = 0.0f;
   float xRot = 0.0f, yRot = 0.0f, zRot = 0.0f;
   dtCore::RefPtr<dtGame::GameActorProxy> object;
   dtCore::Transform position;

   dtCore::RefPtr<dtDAL::ActorType> playerType = mClientGM->FindActorType("ExampleActors", "TestPlayer");      
   object = dynamic_cast<dtGame::GameActorProxy *>(mClientGM->CreateActor(*playerType).get());   

   // starting location = mplayer position, unless he's invalid in which case, use the camera
   if (bSetLocation) 
   {
      if (mPlayerIsValid)
      {
         // do this if 
         object->SetTranslation(mPlayer->GetTranslation());
      }
      else 
      { // can't use player, so use the camera position instead.
         GetCamera()->GetTransform(&position);
         object->GetGameActor().SetTransform(&position);
      }

      // rescale our object to make it neat.
      zScale = Random(.80, 1.20);
      xScale = Random(.80, 1.20) * zScale;
      yScale = Random(.80, 1.20) * zScale;
      object->SetScale(osg::Vec3(xScale, yScale, zScale));

      // set initial random rotation (X = pitch, Y = roll, Z = yaw) for non rotating objects
      // don't change rotating objects cause the movement will follow the rotation, which may 
      // look wierd.
      if (turnRate == 0.0)
      {
         xRot = Random(-5, 5);
         yRot = Random(-5, 5);
         zRot = Random(0, 360);
         object->SetRotation(osg::Vec3(xRot, yRot, zRot));
      }
   }

   mClientGM->AddActor(*object,false,false);

   // set mesh, velocity, and turn rate
   dtDAL::StringActorProperty *prop = static_cast<dtDAL::StringActorProperty *>(object->GetProperty("mesh"));
   prop->SetValue(meshName);
   dtDAL::FloatActorProperty *velocityProp = static_cast<dtDAL::FloatActorProperty *>(object->GetProperty("velocity"));
   velocityProp->SetValue(velocity);
   dtDAL::FloatActorProperty *turnRateProp = static_cast<dtDAL::FloatActorProperty *>(object->GetProperty("turnrate"));
   turnRateProp->SetValue(turnRate);

   return object;
}

//////////////////////////////////////////////////////////////////////////
void LoggerApplication::PreFrame(const double deltaFrameTime)
{
   if (mPlayerIsValid)
   {
      dtDAL::FloatActorProperty *playerVelocity = static_cast<dtDAL::FloatActorProperty *>(mPlayer->GetProperty("velocity"));
      dtDAL::FloatActorProperty *playerTurnRate = static_cast<dtDAL::FloatActorProperty *>(mPlayer->GetProperty("turnrate"));
              
      if (GetKeyboard()->GetKeyState(Producer::Key_S))
         playerVelocity->SetValue(10.0f);
      else if (GetKeyboard()->GetKeyState(Producer::Key_W))
         playerVelocity->SetValue(-10.0f);
      else
         playerVelocity->SetValue(0.0f);
         
      if (GetKeyboard()->GetKeyState(Producer::Key_D))
         playerTurnRate->SetValue(-0.25f);
      else if (GetKeyboard()->GetKeyState(Producer::Key_A))
         playerTurnRate->SetValue(0.25f);
      else
         playerTurnRate->SetValue(0.0f);
   }
}

//////////////////////////////////////////////////////////////////////////
void LoggerApplication::SetCameraMode(CameraMode mode)
{
   mCameraMode = mode;
   if (mode == CAM_FREE_FLY)
   {
      mPlayer->GetGameActor().RemoveChild(GetCamera());
      
      if (!mMotionModel.valid())
      {
         mMotionModel = new dtCore::OrbitMotionModel(GetKeyboard(),GetMouse());
         mMotionModel->SetTarget(GetCamera());
      }
      
      mMotionModel->SetEnabled(true);      
   }
   else if (mode == CAM_FOLLOW_PLAYER)
   {
      mMotionModel->SetEnabled(false);
      mPlayer->GetGameActor().AddChild(GetCamera());
      dtCore::Transform tx(0,10,0,0,0,0);
      GetCamera()->SetTransform(&tx,dtCore::Transformable::REL_CS);
   }
}

//////////////////////////////////////////////////////////////////////////
void LoggerApplication::KeyPressed(dtCore::Keyboard *keyBoard, 
   Producer::KeyboardKey key, Producer::KeyCharacter character)
{
   dtABC::Application::KeyPressed(keyBoard,key,character); 
   std::ostringstream ss;
   dtCore::RefPtr<dtGame::GameActorProxy> obj;
   float turn,velocity;
   float chance,chance2;
   
   switch (key)
   {
      case Producer::Key_space:
         mLogController->RequestServerGetStatus();
         break;
      
      case Producer::Key_C:
         if (mCameraMode == CAM_FOLLOW_PLAYER)
            SetCameraMode(CAM_FREE_FLY);
         else
            SetCameraMode(CAM_FOLLOW_PLAYER);
         break;
           
      case Producer::Key_1:
         if (!mPlayerIsValid)
         {
            mClientGM->AddActor(*mPlayer,false,false);
            mPlayerIsValid = true;
         }
         
         mLogController->RequestChangeStateToIdle();
         mClientGM->SetPaused(false);
         break;
         
      case Producer::Key_2:
         mLogController->RequestChangeStateToRecord();
         break;
         
      case Producer::Key_3:
         if (mPlayerIsValid)
         {
            GetScene()->RemoveDrawable(&mPlayer->GetGameActor());
            mClientGM->DeleteActor(*mPlayer);
            mPlayerIsValid = false;
         }
         
         mLogController->RequestChangeStateToPlayback();
         mClientGM->SetPaused(false);
         break;

      case Producer::Key_bracketleft:
         mSimSpeedFactor = mSimSpeedFactor * 0.9;
         if (mSimSpeedFactor < 0.10)
            mSimSpeedFactor = 0.10;
         else 
         {
            ss << "Decreasing Game Manager Speed to [" << mSimSpeedFactor << "]X Realtime.";
            std::cout << ss.str() << std::endl;
         }

         mClientGM->ChangeTimeSettings(mClientGM->GetSimulationTime(), 
            mSimSpeedFactor, mClientGM->GetSimulationClockTime());
         break;

      case Producer::Key_bracketright:
         mSimSpeedFactor = mSimSpeedFactor * 1.20;
         if (mSimSpeedFactor > 10.0)
            mSimSpeedFactor = 10.0;
         else 
         {
            ss << "Increasing Game Manager Speed to [" << mSimSpeedFactor << "]X Realtime.";
            std::cout << ss.str() << std::endl;
         }

         mClientGM->ChangeTimeSettings(mClientGM->GetSimulationTime(), 
            mSimSpeedFactor, mClientGM->GetSimulationClockTime());
         break;

      case Producer::Key_0:
         mSimSpeedFactor = 1.0;
         ss << "Resetting Game Manager Speed to [" << mSimSpeedFactor << "] == Realtime.";
         std::cout << ss.str() << std::endl;
         mClientGM->ChangeTimeSettings(mClientGM->GetSimulationTime(), 
            mSimSpeedFactor, mClientGM->GetSimulationClockTime());
         break;

      case Producer::Key_P:
         mClientGM->SetPaused(!mClientGM->IsPaused()); 
         break;

      case Producer::Key_B:
         turn = Random(-0.60, 0.60);
         if (turn < 0.1f && turn > -0.1f) 
            turn = 0.1f;
         
         velocity = Random(-12.0, 12.00);
         if (velocity < 0.5f && velocity > -0.5f) 
            velocity = 0.0f;
         
         chance = Random(0.0, 1.0);
   
         // make only some of them move cause it causes problems computing 
         // the intersection with the ground. (Performance bug..)
         chance2 = Random(0.0, 1.0);
         if (chance2 <= 0.75)
            velocity = 0.0f;
         
         if (chance <= 0.5)
            obj = CreateNewMovingActor("models/physics_crate.ive",velocity,turn,true);
         else
            obj = CreateNewMovingActor("models/physics_barrel.ive",velocity,turn,true);
         break;

      case Producer::Key_Return:
         GetCamera()->SetNextStatisticsType();
         break;

      default:
         break;         
   };  
}

//////////////////////////////////////////////////////////////////////////
void LoggerApplication::OnReceivedStatus(const dtGame::LogStatus &newStatus)
{
   std::cout << "##STATUS MESSAGE##" << newStatus << std::endl;
}

//////////////////////////////////////////////////////////////////////////
void LoggerApplication::OnReceivedRejection(const dtGame::Message &newMessage)
{
   const dtGame::ServerMessageRejected &rejMsg = static_cast<const dtGame::ServerMessageRejected &>(newMessage);

   std::ostringstream ss;
   ss << "## REJECTION RECEIVED ##: Reason[" << rejMsg.GetCause() << "]...";
   std::cout << ss.str() << std::endl;

   const dtGame::Message *causeMsg = rejMsg.GetCausingMessage();
   if (causeMsg != NULL)
   {
      ss.str("");
      std::string paramsString;
      causeMsg->ToString(paramsString);
      ss << "     CAUSE: Type[" << causeMsg->GetMessageType().GetName() << "], Params[" 
         << paramsString << "]"; 
      std::cout << ss.str() << std::endl;
   }
}
