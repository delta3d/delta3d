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
* @author Curtiss Murphy
*/

#include "testaar.h"

#include <dtCore/object.h>
#include <dtCore/globals.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <dtCore/keyboard.h>
#include <dtUtil/exception.h>

#include <dtGame/binarylogstream.h>
#include <dtGame/logtag.h>
#include <dtGame/logkeyframe.h>
#include <dtGame/logstatus.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/loggermessages.h>
#include <dtGame/basemessages.h>
#include <dtGame/clientgamemanager.h>
#include <dtGame/logcontroller.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/taskcomponent.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventmanager.h>

#include <dtActors/taskactor.h>
#include <dtActors/taskactorgameevent.h>
#include <dtActors/taskactorrollup.h>
#include <dtActors/taskactorordered.h>

#include <ctime>

// TODO: replace this with a platform-independant wrapper
#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #ifndef snprintf
      #define snprintf _snprintf
   #endif // snprintf
#endif // WIN32

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(AppException);
AppException AppException::INIT_ERROR("INIT_ERROR");

//////////////////////////////////////////////////////////////////////////
AARApplication::AARApplication()
    : dtABC::Application("testLogger.xml"), mKeyIsPressed(false)
{
   mSimSpeedFactor = 1.0;
   mLastAutoRequestStatus = 0.0;
   srand(static_cast<unsigned>(time(NULL)));
}

//////////////////////////////////////////////////////////////////////////
AARApplication::~AARApplication()
{

}


//////////////////////////////////////////////////////////////////////////
void AARApplication::Config()
{
   mClientGM = new dtGame::ClientGameManager(*GetScene());

   //To enable logging, we must have three parts.  A server logger component which, in a
   //networked environment would reside on the server, a log controller which is a client side
   //component responcible for maintaining tracking information and control the server logger,
   //and third, a stream by which to send the log messagse to.  In this case, we use the built
   //in custom log file format in dtGame.
   dtGame::BinaryLogStream *logStream = new dtGame::BinaryLogStream(mClientGM->GetMessageFactory());
   mServerLogger = new dtGame::ServerLoggerComponent(*logStream);

   mLogController = new dtGame::LogController();
   mLogController->SignalReceivedStatus().connect_slot(this, &AARApplication::OnReceivedStatus);
   mLogController->SignalReceivedRejection().connect_slot(this, &AARApplication::OnReceivedRejection);
   mLogController->SignalReceivedTags().connect_slot(this, &AARApplication::OnReceivedTags);
   mLogController->SignalReceivedKeyframes().connect_slot(this, &AARApplication::OnReceivedKeyframes);

   mClientGM->AddComponent(*mServerLogger.get(), dtGame::GameManager::ComponentPriority::NORMAL);
   mClientGM->AddComponent(*mLogController.get(), dtGame::GameManager::ComponentPriority::NORMAL);

   //Add the default message processor.  Note, we specifiy its priority to be HIGHEST to ensure
   //that it gets all messages before other game manager components.  This should almost always
   //be the case for the default message processor or custom subclasses of it.
   dtGame::DefaultMessageProcessor *mp = new dtGame::DefaultMessageProcessor();
   mClientGM->AddComponent(*mp, dtGame::GameManager::ComponentPriority::HIGHEST);

   //Ask the game manager to log statistics every three seconds..
   mClientGM->SetStatisticsInterval(5);

   //Create the task component so we have a centralized method of tracking tasks without having
   //to do a generalized search using the Game Manager.
   mTaskComponent = new dtGame::TaskComponent();
   mClientGM->AddComponent(*mTaskComponent,dtGame::GameManager::ComponentPriority::NORMAL);

   //Load the library with the test game actors...
   mClientGM->LoadActorRegistry("testGameActorLibrary");

   dtABC::Application::Config();
   GetWindow()->SetWindowTitle("testAAR");
   SetupScene();
   dtCore::Transform tx(0.0f,-25.0f,10.0f,0.0f,-15.0f,0.0f);
   GetCamera()->SetTransform(&tx);
   Reset();
   SetupGUI();
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::SetupScene()
{
   mMotionModel = new dtCore::FlyMotionModel(GetKeyboard(),GetMouse(), false);
   mMotionModel->SetTarget(GetCamera());
   dtCore::Transform tx(0.0f,-25.0f,10.0f,0.0f,-15.0f,0.0f);
   GetCamera()->SetTransform(&tx);

   // setup terrain
   dtCore::Object *terrain = new dtCore::Object();
   terrain->LoadFile("models/terrain_simple.ive");
   GetScene()->AddDrawable(terrain);

   // Create and register the events we need so we can send messages and update our tasks
   mEventStartRecord = new dtDAL::GameEvent("Event - Start Record");
   mEventStartRecord->SetUniqueId(dtCore::UniqueId("Event - Start Record"));

   mEventBoxPlaced = new dtDAL::GameEvent("Event - Box Placed");
   mEventBoxPlaced->SetUniqueId(dtCore::UniqueId("Event - Box Placed"));

   mEventPlayerLeft = new dtDAL::GameEvent("Event - Player Left");
   mEventPlayerLeft->SetUniqueId(dtCore::UniqueId("Event - Player Left"));

   mEventPlayerRight = new dtDAL::GameEvent("Event - player Right");
   mEventPlayerRight->SetUniqueId(dtCore::UniqueId("Event - player Right"));

   mEventPlayerForward = new dtDAL::GameEvent("Event - Player Forward");
   mEventPlayerForward->SetUniqueId(dtCore::UniqueId("Event - Player Forward"));

   mEventPlayerBackward = new dtDAL::GameEvent("Event - Player Backward");
   mEventPlayerBackward->SetUniqueId(dtCore::UniqueId("Event - Player Backward"));

   dtDAL::GameEventManager::GetInstance().AddEvent(*mEventStartRecord);
   dtDAL::GameEventManager::GetInstance().AddEvent(*mEventBoxPlaced);
   dtDAL::GameEventManager::GetInstance().AddEvent(*mEventPlayerLeft);
   dtDAL::GameEventManager::GetInstance().AddEvent(*mEventPlayerRight);
   dtDAL::GameEventManager::GetInstance().AddEvent(*mEventPlayerForward);
   dtDAL::GameEventManager::GetInstance().AddEvent(*mEventPlayerBackward);
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::SetupGUI()
{
   hudGUI = new TestAARHUD(GetWindow(), *mClientGM, *mLogController, *mTaskComponent, *mServerLogger);
   mClientGM->AddComponent(*hudGUI, dtGame::GameManager::ComponentPriority::NORMAL);
   GetScene()->AddDrawable(hudGUI->GetGUIDrawable().get());
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::SetupTasks()
{
   //Get our actor types...
   dtCore::RefPtr<dtDAL::ActorType> taskType = mClientGM->FindActorType("dtcore.Tasks","Task Actor");
   if (taskType == NULL)
      EXCEPT(AppException::INIT_ERROR,"Could not find task actor type.");
   dtCore::RefPtr<dtDAL::ActorType> eventTaskType = mClientGM->FindActorType("dtcore.Tasks","GameEvent Task Actor");
   if (eventTaskType == NULL)
      EXCEPT(AppException::INIT_ERROR,"Could not find Game Event Task Actor.");
   dtCore::RefPtr<dtDAL::ActorType> orderedTaskType = mClientGM->FindActorType("dtcore.Tasks","Ordered Task Actor");
   if (orderedTaskType == NULL)
      EXCEPT(AppException::INIT_ERROR,"Could not find Ordered Task Actor.");
   dtCore::RefPtr<dtDAL::ActorType> rollupTaskType = mClientGM->FindActorType("dtcore.Tasks","Rollup Task Actor");
   if (rollupTaskType == NULL)
      EXCEPT(AppException::INIT_ERROR,"Could not find Rollup Task Actor.");

   // task - root - event - start record  where are all the comments for this code
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskStartRecordProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(mClientGM->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskStart = dynamic_cast<dtActors::TaskActorGameEvent &>
      (taskStartRecordProxy->GetGameActor());
   taskStart.SetGameEvent(mEventStartRecord.get());
   taskStart.SetMinOccurances(1);
   taskStart.SetName("Start a Record");
   taskStartRecordProxy->SetId(dtCore::UniqueId("Start a Record"));
   mClientGM->AddActor(*taskStartRecordProxy,false,false);

   // task - root - event - create 10 boxes
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskDrop10BoxesProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(mClientGM->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskDrop10Boxes = dynamic_cast<dtActors::TaskActorGameEvent &>
      (taskDrop10BoxesProxy->GetGameActor());
   taskDrop10Boxes.SetGameEvent(mEventBoxPlaced.get());
   taskDrop10Boxes.SetMinOccurances(10);
   taskDrop10Boxes.SetName("Drop 10 boxes");
   taskDrop10BoxesProxy->SetId(dtCore::UniqueId("Drop 10 boxes"));
   mClientGM->AddActor(*taskDrop10BoxesProxy,false,false);

   // * task - root - base - move camera, any direction
   mTaskMoveCameraProxy = dynamic_cast<dtActors::TaskActorProxy *>(mClientGM->CreateActor(*taskType).get());
   mTaskMoveCamera = dynamic_cast<dtActors::TaskActor *> (&(mTaskMoveCameraProxy->GetGameActor()));
   mTaskMoveCameraProxy->SetId(dtCore::UniqueId("TaskMoveCamera"));
   mTaskMoveCameraProxy->SetName("Move Camera");
   mTaskMoveCameraProxy->SetId(dtCore::UniqueId("Move Camera"));
   mClientGM->AddActor(*mTaskMoveCameraProxy,false,false);

   // SETUP THE NESTED TASKS

   // task - root - ordered (blocking) - 'Place Objects'
   dtCore::RefPtr<dtActors::TaskActorOrderedProxy> taskPlaceObjectsProxy =
      dynamic_cast<dtActors::TaskActorOrderedProxy*>(mClientGM->CreateActor(*orderedTaskType).get());
   dtActors::TaskActorOrdered &taskPlaceObjects = dynamic_cast<dtActors::TaskActorOrdered &>
      (taskPlaceObjectsProxy->GetGameActor());
   taskPlaceObjects.SetFailureType(dtActors::TaskActorOrdered::FailureType::BLOCK);
   taskPlaceObjects.SetName("Place Objects (Ordered)");
   taskPlaceObjectsProxy->SetId(dtCore::UniqueId("Place Objects (Ordered)"));
   mClientGM->AddActor(*taskPlaceObjectsProxy,false,false);

   //   task - child - rollup - (.75 to pass) - 'Move Player'
   dtCore::RefPtr<dtActors::TaskActorRollupProxy> taskMovePlayerProxy =
      dynamic_cast<dtActors::TaskActorRollupProxy*>(mClientGM->CreateActor(*rollupTaskType).get());
   dtActors::TaskActorRollup &taskMovePlayer = dynamic_cast<dtActors::TaskActorRollup &>
      (taskMovePlayerProxy->GetGameActor());
   taskMovePlayer.SetName("Move the Player (Rollup)");
   taskMovePlayerProxy->SetId(dtCore::UniqueId("Move the Player (Rollup)"));
   taskMovePlayer.SetPassingScore(0.75f); // only need 3 to succeed
   taskPlaceObjectsProxy->AddSubTaskProxy(*taskMovePlayerProxy);
   mClientGM->AddActor(*taskMovePlayerProxy,false,false);

   //      task - child - event - move left (.25)
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskPlayerLeftProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(mClientGM->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskPlayerLeft = dynamic_cast<dtActors::TaskActorGameEvent &>
      (taskPlayerLeftProxy->GetGameActor());
   taskPlayerLeft.SetGameEvent(mEventPlayerLeft.get());
   taskPlayerLeft.SetMinOccurances(1);
   taskPlayerLeft.SetWeight(0.25f);
   taskPlayerLeft.SetName("Turn Player Left");
   taskPlayerLeftProxy->SetId(dtCore::UniqueId("Turn Player Left"));
   taskMovePlayerProxy->AddSubTaskProxy(*taskPlayerLeftProxy);
   mClientGM->AddActor(*taskPlayerLeftProxy,false,false);

   //      task - child - event - move right (.25)
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskPlayerRightProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(mClientGM->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskPlayerRight = dynamic_cast<dtActors::TaskActorGameEvent &>
      (taskPlayerRightProxy->GetGameActor());
   taskPlayerRight.SetGameEvent(mEventPlayerRight.get());
   taskPlayerRight.SetMinOccurances(1);
   taskPlayerRight.SetWeight(0.25f);
   taskPlayerRight.SetName("Turn Player Right");
   taskPlayerRightProxy->SetId(dtCore::UniqueId("Turn Player Right"));
   taskMovePlayerProxy->AddSubTaskProxy(*taskPlayerRightProxy);
   mClientGM->AddActor(*taskPlayerRightProxy,false,false);

   //      task - child - event - move forward (.25)
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskPlayerForwardProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(mClientGM->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskPlayerForward = dynamic_cast<dtActors::TaskActorGameEvent &>
      (taskPlayerForwardProxy->GetGameActor());
   taskPlayerForward.SetGameEvent(mEventPlayerForward.get());
   taskPlayerForward.SetMinOccurances(1);
   taskPlayerForward.SetWeight(0.25f);
   taskPlayerForward.SetName("Move Player Forward");
   taskPlayerForwardProxy->SetId(dtCore::UniqueId("Move Player Forward"));
   taskMovePlayerProxy->AddSubTaskProxy(*taskPlayerForwardProxy);
   mClientGM->AddActor(*taskPlayerForwardProxy,false,false);

   //      task - child - event - move back (.25)
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskPlayerBackProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(mClientGM->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskPlayerBack = dynamic_cast<dtActors::TaskActorGameEvent &>
      (taskPlayerBackProxy->GetGameActor());
   taskPlayerBack.SetGameEvent(mEventPlayerBackward.get());
   taskPlayerBack.SetMinOccurances(1);
   taskPlayerBack.SetWeight(0.25f);
   taskPlayerBack.SetName("Move Player Back");
   taskPlayerBackProxy->SetId(dtCore::UniqueId("Move Player Back"));
   taskMovePlayerProxy->AddSubTaskProxy(*taskPlayerBackProxy);
   mClientGM->AddActor(*taskPlayerBackProxy,false,false);

   //   task - child - event - place 5 boxes
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskDrop5BoxesProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(mClientGM->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskDrop5Boxes = dynamic_cast<dtActors::TaskActorGameEvent &>
      (taskDrop5BoxesProxy->GetGameActor());
   taskDrop5Boxes.SetGameEvent(mEventBoxPlaced.get());
   taskDrop5Boxes.SetMinOccurances(5);
   taskDrop5Boxes.SetName("Drop 5 boxes");
   taskDrop5BoxesProxy->SetId(dtCore::UniqueId("Drop 5 boxes"));
   taskPlaceObjectsProxy->AddSubTaskProxy(*taskDrop5BoxesProxy);
   mClientGM->AddActor(*taskDrop5BoxesProxy,false,false);
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::Reset()
{
   mClientGM->DeleteAllActors();
   mTaskComponent->ClearTaskList();
   dtCore::System::Instance()->Step();

   dtCore::RefPtr<dtDAL::ActorType> playerType = mClientGM->FindActorType("ExampleActors", "TestPlayer");
   mPlayer = dynamic_cast<dtGame::GameActorProxy *>(mClientGM->CreateActor(*playerType).get());
   mPlayer->SetTranslation( osg::Vec3( 0.0f, 0.0f, 5.0f ) );

   dtDAL::StringActorProperty *prop = static_cast<dtDAL::StringActorProperty *>(mPlayer->GetProperty("mesh"));
   prop->SetValue("models/physics_happy_sphere.ive");

   mClientGM->AddActor(*mPlayer,false,false);

   SetupTasks();
}

//////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<dtGame::GameActorProxy> AARApplication::CreateNewMovingActor(
   const std::string &meshName, float velocity, float turnRate, bool bSetLocation)
{
   if (mLogController->GetLastKnownStatus().GetStateEnum() ==
       dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
      return NULL;

   float xScale = 0.0f, yScale = 0.0f, zScale = 0.0f;
   float xRot = 0.0f, yRot = 0.0f, zRot = 0.0f;
   dtCore::RefPtr<dtGame::GameActorProxy> object;
   dtCore::Transform position;

   dtCore::RefPtr<dtDAL::ActorType> playerType = mClientGM->FindActorType("ExampleActors", "TestPlayer");
   object = dynamic_cast<dtGame::GameActorProxy *>(mClientGM->CreateActor(*playerType).get());

   if (bSetLocation)
   {
      object->SetTranslation(mPlayer->GetTranslation());

      // rescale our object to make it neat.
      zScale = Random(0.70f, 1.3f);
      xScale = Random(0.70f, 1.3f) * zScale;
      yScale = Random(0.70f, 1.3f) * zScale;
      object->SetScale(osg::Vec3(xScale, yScale, zScale));

      // set initial random rotation (X = pitch, Y = roll, Z = yaw) for non rotating objects
      // don't change rotating objects cause the movement will follow the rotation, which may
      // look wierd.
      if (turnRate == 0.0f)
      {
         xRot = Random(-5.0f, 5.0f);
         yRot = Random(-5.0f, 5.0f);
         zRot = Random(0.0f, 360.0f);
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
void AARApplication::PreFrame(const double deltaFrameTime)
{
   // roughly every 3 real seconds, we force status update so the HUD updates and doesn't look broken.
   if (mLastAutoRequestStatus > mClientGM->GetSimulationTime())
      mLastAutoRequestStatus = mClientGM->GetSimulationTime();
   else if ((mLastAutoRequestStatus + 3.0*mClientGM->GetTimeScale()) < mClientGM->GetSimulationTime())
   {
      RequestAllControllerUpdates();
      mLastAutoRequestStatus = mClientGM->GetSimulationTime();
   }

   if (mLogController->GetLastKnownStatus().GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK
       || mPlayer == NULL)
      return;

   dtDAL::FloatActorProperty *playerVelocity = static_cast<dtDAL::FloatActorProperty *>(mPlayer->GetProperty("velocity"));
   dtDAL::FloatActorProperty *playerTurnRate = static_cast<dtDAL::FloatActorProperty *>(mPlayer->GetProperty("turnrate"));
   if (GetKeyboard()->GetKeyState(Producer::Key_I))
      playerVelocity->SetValue(10.0f);
   else if (GetKeyboard()->GetKeyState(Producer::Key_K))
      playerVelocity->SetValue(-10.0f);
   else
      playerVelocity->SetValue(0.0f);

   if (GetKeyboard()->GetKeyState(Producer::Key_L))
      playerTurnRate->SetValue(-0.25f);
   else if (GetKeyboard()->GetKeyState(Producer::Key_J))
      playerTurnRate->SetValue(0.25f);
   else
      playerTurnRate->SetValue(0.0f);
}

//////////////////////////////////////////////////////////////////////////
bool AARApplication::KeyPressed(const dtCore::Keyboard *keyBoard,
   Producer::KeyboardKey key, Producer::KeyCharacter character)
{
   dtABC::Application::KeyPressed(keyBoard,key,character);

   std::ostringstream ss;
   bool handled = true;

   switch (key)
   {
      case Producer::Key_space:
         RequestAllControllerUpdates();
         break;

      case Producer::Key_W:
      case Producer::Key_A:
      case Producer::Key_S:
      case Producer::Key_D:
         if (mLogController->GetLastKnownStatus().GetStateEnum() !=
             dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
         {
            if (!mTaskMoveCamera->IsComplete())
            {
               mTaskMoveCamera->SetScore(1.0);
               mTaskMoveCamera->SetComplete(true);

               dtActors::TaskActorProxy &proxy =
                     static_cast<dtActors::TaskActorProxy&>(mTaskMoveCamera->GetGameActorProxy());
               proxy.NotifyActorUpdate();
            }
         }
         break;

      case Producer::Key_I:
         FireEvent(mEventPlayerForward.get());
         break;

      case Producer::Key_J:
         FireEvent(mEventPlayerLeft.get());
         break;

      case Producer::Key_K:
         FireEvent(mEventPlayerBackward.get());
         break;

      case Producer::Key_L:
         FireEvent(mEventPlayerRight.get());
         break;

      case Producer::Key_1:
         if (mLogController->GetLastKnownStatus().GetStateEnum() ==
               dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
         {
            Reset();
         }

         mLogController->RequestChangeStateToIdle();
         mClientGM->SetPaused(false);
         break;

      case Producer::Key_2:
         mLogController->RequestChangeStateToRecord();
         FireEvent(mEventStartRecord.get());
         break;

      case Producer::Key_3:
         //Going from idle mode to playback mode so we need to remove the player..
         if (mLogController->GetLastKnownStatus().GetStateEnum() ==
            dtGame::LogStateEnumeration::LOGGER_STATE_IDLE)
         {
            mClientGM->DeleteAllActors();
            dtCore::System::Instance()->Step();
            mPlayer = NULL;
         }

         mLogController->RequestChangeStateToPlayback();
         mClientGM->SetPaused(false);
         break;

      case Producer::Key_minus:
      case Producer::Key_KP_Subtract:
         mSimSpeedFactor = mSimSpeedFactor * 0.9f;
         if (mSimSpeedFactor < 0.10f)
            mSimSpeedFactor = 0.10f;
         else
         {
            ss << "Decreasing Game Manager Speed to [" << mSimSpeedFactor << "]X Realtime.";
            std::cout << ss.str() << std::endl;
         }

         mClientGM->ChangeTimeSettings(mClientGM->GetSimulationTime(),
            mSimSpeedFactor, mClientGM->GetSimulationClockTime());
         break;

      case Producer::Key_KP_Add:
      case Producer::Key_equal:
      case Producer::KeyChar_plus:
         mSimSpeedFactor = mSimSpeedFactor * 1.20f;
         if (mSimSpeedFactor > 10.0f)
            mSimSpeedFactor = 10.0f;
         else
         {
            ss << "Increasing Game Manager Speed to [" << mSimSpeedFactor << "]X Realtime.";
            std::cout << ss.str() << std::endl;
         }

         mClientGM->ChangeTimeSettings(mClientGM->GetSimulationTime(),
            mSimSpeedFactor, mClientGM->GetSimulationClockTime());
         handled = true;
         break;

      case Producer::Key_0:
         mSimSpeedFactor = 1.0f;
         ss << "Resetting Game Manager Speed to [" << mSimSpeedFactor << "] == Realtime.";
         std::cout << ss.str() << std::endl;
         mClientGM->ChangeTimeSettings(mClientGM->GetSimulationTime(),
            mSimSpeedFactor, mClientGM->GetSimulationClockTime());
         break;

      case Producer::Key_P:
         mClientGM->SetPaused(!mClientGM->IsPaused());
         break;

      case Producer::Key_B:
         if (mLogController->GetLastKnownStatus().GetStateEnum() !=
             dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
            PlaceActor();
         break;

      case Producer::Key_T:
         InsertTag();
         break;

      case Producer::Key_F:
         InsertKeyFrame();
         break;

      case Producer::Key_F1:
         if (HUDState::HELP == hudGUI->GetHUDState())
            hudGUI->CycleToNextHUDState(); // already in help, so toggle it off
         else
            hudGUI->SetHUDState(HUDState::HELP);

         break;

      case Producer::Key_F2:
         hudGUI->CycleToNextHUDState();
         break;

      case Producer::Key_Return:
         GetCamera()->SetNextStatisticsType();
         break;

      case Producer::Key_M:
         PrintTasks();
         break;

      case Producer::Key_comma:
         GotoPreviousKeyframe();
         break;

      case Producer::Key_period:
         GotoNextKeyframe();
         break;

      default:
         handled = false;
         break;
   };

   return handled;
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::GotoPreviousKeyframe()
{
   const std::vector<dtGame::LogKeyframe> &frames = mLogController->GetLastKnownKeyframeList();
   const dtGame::LogKeyframe *prevFrame = NULL;

   for (int i = 0; i < (int) frames.size(); i ++)
   {
      // Get the oldest frame less than the current sim time.  Allow a 3 second grace period
      // so that we don't have the same problem you do on CD players
      if ((frames[i].GetSimTimeStamp() + 2.25) < mClientGM->GetSimulationTime() &&
         (prevFrame == NULL || frames[i].GetSimTimeStamp() > prevFrame->GetSimTimeStamp()))
      {
         prevFrame = &(frames[i]);
      }
   }

   // found one, so jump to keyframe
   if (prevFrame != NULL)
   {
      std::ostringstream ss;
      ss << "## Attempting to Jump to Previous Keyframe: [" << prevFrame->GetName() << "] ##";
      std::cout << ss.str() << std::endl;

      mLogController->RequestJumpToKeyframe(*prevFrame);
   }
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::GotoNextKeyframe()
{
   const std::vector<dtGame::LogKeyframe> &frames = mLogController->GetLastKnownKeyframeList();
   const dtGame::LogKeyframe *nextFrame = NULL;

   for (int i = 0; i < (int) frames.size(); i ++)
   {
      // Get the first frame older than the current sim time.
      if (frames[i].GetSimTimeStamp() > mClientGM->GetSimulationTime() &&
         (nextFrame == NULL || frames[i].GetSimTimeStamp() < nextFrame->GetSimTimeStamp()))
      {
         nextFrame = &(frames[i]);
      }
   }

   // found one, so jump to keyframe
   if (nextFrame != NULL)
   {
      std::ostringstream ss;
      ss << "## Attempting to Jump to Next Keyframe: [" << nextFrame->GetName() << "] ##";
      std::cout << ss.str() << std::endl;

      mLogController->RequestJumpToKeyframe(*nextFrame);
   }
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::FireEvent(dtDAL::GameEvent *event)
{
   if (event != NULL && (mLogController->GetLastKnownStatus().GetStateEnum()
       != dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK))
   {
      dtCore::RefPtr<dtGame::GameEventMessage> eventMsg;
      mClientGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT, eventMsg);

      eventMsg->SetGameEvent(*event);
      mClientGM->SendMessage(*eventMsg);
   }
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::InsertTag()
{
   char clin[100];            // buffer to print
   dtGame::LogTag newTag;

   newTag.SetCaptureKeyframe(false);
   newTag.SetDescription("Nifty Tag Description");
   snprintf(clin, 100, "My Tag At [%.2f]", mClientGM->GetSimulationTime());
   newTag.SetName(clin);

   mLogController->RequestInsertTag(newTag);
   mLogController->RequestServerGetTags();

}

//////////////////////////////////////////////////////////////////////////
void AARApplication::InsertKeyFrame()
{
   char clin[100];            // buffer to print
   dtGame::LogKeyframe newKeyframe;

   newKeyframe.SetDescription("Nifty Keyframe Description");
   snprintf(clin, 100, "Frame At [%.2f]", mClientGM->GetSimulationTime());
   newKeyframe.SetName(clin);

   mLogController->RequestCaptureKeyframe(newKeyframe);
   mLogController->RequestServerGetKeyframes();
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::RequestAllControllerUpdates()
{
   mLogController->RequestServerGetStatus();
   mLogController->RequestServerGetKeyframes();
   mLogController->RequestServerGetTags();
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::PlaceActor()
{
   float turn,velocity;
   float chance,chance2;
   dtCore::RefPtr<dtGame::GameActorProxy> obj;

   turn = Random(-0.60f, 0.60f);
   if (turn < 0.1f && turn > -0.1f)
      turn = 0.1f;

   velocity = Random(-12.0f, 12.0f);
   if (velocity < 0.5f && velocity > -0.5f)
      velocity = 0.0f;

   chance = Random(0.0, 1.0f);

   // make only some of them move cause it causes problems computing
   // the intersection with the ground. (Performance bug..)
   chance2 = Random(0.0f, 1.0f);
   if (chance2 <= 0.75f)
      velocity = 0.0f;

   if (chance <= 0.5f)
      obj = CreateNewMovingActor("models/physics_crate.ive",velocity,turn,true);
   else
      obj = CreateNewMovingActor("models/physics_barrel.ive",velocity,turn,true);


   // fire a box created event
   dtCore::RefPtr<dtGame::GameEventMessage> eventMsg = static_cast<dtGame::GameEventMessage*>
         (mClientGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());
   eventMsg->SetGameEvent(*mEventBoxPlaced);
   mClientGM->SendMessage(*eventMsg);
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::OnReceivedStatus(const dtGame::LogStatus &newStatus)
{
   static bool isFirstPlayback = true;

   // so we don't update again if user just requested it
   mLastAutoRequestStatus = mClientGM->GetSimulationTime();

   if (newStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE)
      isFirstPlayback = true;

   //The following is a great big hack due to the fact that we do not yet
   //support the task hierarchy as actor properties.  Since the task hierarchy
   //is not wrapped within actor properties, it does not get recreated
   //properly when changing to playback mode.  However, the since the task
   //actors themselves got properly recreated, we just need to manually set
   //the hierarchy when changing from IDLE state to PLAYBACK state.
   if (newStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK
         && isFirstPlayback)
   {
      isFirstPlayback = false;

      dtActors::TaskActorProxy *placeObjects =
            dynamic_cast<dtActors::TaskActorProxy*>(mTaskComponent->GetTaskByName("Place Objects (Ordered)").get());

      dtActors::TaskActorProxy *movePlayerRollup =
            dynamic_cast<dtActors::TaskActorProxy*>(mTaskComponent->GetTaskByName("Move the Player (Rollup)").get());

      dtActors::TaskActorProxy *movePlayerLeft =
            dynamic_cast<dtActors::TaskActorProxy*>(mTaskComponent->GetTaskByName("Turn Player Left").get());

      dtActors::TaskActorProxy *movePlayerRight =
            dynamic_cast<dtActors::TaskActorProxy*>(mTaskComponent->GetTaskByName("Turn Player Right").get());

      dtActors::TaskActorProxy *movePlayerForward =
            dynamic_cast<dtActors::TaskActorProxy*>(mTaskComponent->GetTaskByName("Move Player Forward").get());

      dtActors::TaskActorProxy *movePlayerBack =
            dynamic_cast<dtActors::TaskActorProxy*>(mTaskComponent->GetTaskByName("Move Player Back").get());

      dtActors::TaskActorProxy *drop5Boxes =
            dynamic_cast<dtActors::TaskActorProxy*>(mTaskComponent->GetTaskByName("Drop 5 boxes").get());

      //Recreate the hierarchy...
      movePlayerRollup->AddSubTaskProxy(*movePlayerLeft);
      movePlayerRollup->AddSubTaskProxy(*movePlayerRight);
      movePlayerRollup->AddSubTaskProxy(*movePlayerForward);
      movePlayerRollup->AddSubTaskProxy(*movePlayerBack);

      placeObjects->AddSubTaskProxy(*movePlayerRollup);
      placeObjects->AddSubTaskProxy(*drop5Boxes);

      mTaskComponent->CheckTaskHierarchy();
   }
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::OnReceivedRejection(const dtGame::Message &newMessage)
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

//////////////////////////////////////////////////////////////////////////
void AARApplication::OnReceivedTags(const std::vector<dtGame::LogTag> &newTagList)
{
   // left this cruft in for debugging
   //std::ostringstream ss;
   //ss << "## RECEIVED TAG LIST ##: [" << newTagList.size() << "] tags";
   //std::cout << ss.str() << std::endl;
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::OnReceivedKeyframes(const std::vector<dtGame::LogKeyframe> &newKeyframesList)
{
   // left this cruft in for debugging...
   //std::ostringstream ss;
   //ss << "## RECEIVED Keyframes LIST ##: [" << newKeyframesList.size() << "] keyframes";
   //std::cout << ss.str() << std::endl;
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::PrintTasks()
{
   std::ostringstream printer;
   std::vector<dtCore::RefPtr<dtGame::GameActorProxy> > tasks;
   std::vector<dtCore::RefPtr<dtGame::GameActorProxy> >::iterator itor;

   printer << "Number of Top Level Tasks: " << mTaskComponent->GetNumTopLevelTasks() <<
      " Total Number of Tasks: " << mTaskComponent->GetNumTasks();

   mTaskComponent->GetAllTasks(tasks);
   printer << std::endl << "Task List:" << std::endl;
   for (itor=tasks.begin(); itor!=tasks.end(); ++itor)
   {
      printer << "\tTask Name: " << (*itor)->GetName() <<
         " Complete: " << (*itor)->GetProperty("Complete")->GetStringValue() << std::endl;
   }

   std::cout << printer.str() << std::endl;
}
