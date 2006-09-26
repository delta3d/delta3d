/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * William E. Johnson II
 */
#include "testaarmessageprocessor.h"
#include "testaarhud.h"
#include "testaarmessagetypes.h"
#include "testaarexceptionenum.h"
#include "testaarinput.h"
#include "testaargameevent.h"

#include <dtABC/application.h>
#include <dtCore/globals.h>
#include <dtCore/keyboard.h>
#include <dtCore/system.h>
#include <dtGame/logcontroller.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/logkeyframe.h>
#include <dtGame/logstatus.h>
#include <dtGame/logtag.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/basemessages.h>
#include <dtDAL/gameeventmanager.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/actortype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtActors/taskactor.h>
#include <dtActors/taskactorgameevent.h>
#include <dtActors/taskactorordered.h>
#include <dtActors/taskactorrollup.h>
#include <dtLMS/lmscomponent.h>
#include <dtUtil/mathdefines.h>

TestAARMessageProcessor::TestAARMessageProcessor(dtLMS::LmsComponent &lmsComp, 
                                                       dtGame::LogController &logCtrl, 
                                                       dtGame::ServerLoggerComponent &srvrCtrl) 
 : mLogController(&logCtrl),
   mLmsComponent(&lmsComp), 
   mLastAutoRequestStatus(0.0),
   mServerLogger(&srvrCtrl),
   mPlayer(NULL)
{

}

TestAARMessageProcessor::~TestAARMessageProcessor()
{

}

void TestAARMessageProcessor::ProcessMessage(const dtGame::Message &msg)
{
   const dtGame::MessageType &type = msg.GetMessageType();

   if(type == dtGame::MessageType::TICK_LOCAL)
   {
      const dtGame::TickMessage &tick = static_cast<const dtGame::TickMessage&>(msg);
      PreFrame(tick.GetDeltaSimTime());
   }
   else if(type == dtGame::MessageType::INFO_ACTOR_DELETED)
   {
      if(mPlayer != NULL && msg.GetAboutActorId() == mPlayer->GetId())
         mPlayer = NULL;
   }
   else if(type == TestAARMessageType::PLACE_ACTOR)
   {
      PlaceActor();
   }
   else if(type == TestAARMessageType::RESET)
   {
      Reset();
   }
   else if(type == TestAARMessageType::REQUEST_ALL_CONTROLLER_UPDATES)
   {
      RequestAllControllerUpdates();
   }
   else if(type == TestAARMessageType::PRINT_TASKS)
   {
      PrintTasks();
   }
   else if(type == TestAARMessageType::UPDATE_TASK_CAMERA)
   {
      UpdateTaskCamera();
   }
   else if(type == dtGame::MessageType::INFO_ACTOR_UPDATED)
   {
      if(mPlayer != NULL && msg.GetAboutActorId() == mPlayer->GetId())
         UpdatePlayerActor(static_cast<const dtGame::ActorUpdateMessage&>(msg));
   }
   
   dtGame::DefaultMessageProcessor::ProcessMessage(msg);
}

//////////////////////////////////////////////////////////////////////////
void TestAARMessageProcessor::OnAddedToGM()
{
   mLogController->SignalReceivedStatus().connect_slot(this, &TestAARMessageProcessor::OnReceivedStatus);
   mLogController->SignalReceivedRejection().connect_slot(this, &TestAARMessageProcessor::OnReceivedRejection);
   mLogController->SignalReceivedTags().connect_slot(this, &TestAARMessageProcessor::OnReceivedTags);
   mLogController->SignalReceivedKeyframes().connect_slot(this, &TestAARMessageProcessor::OnReceivedKeyframes);
}

//////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<dtGame::GameActorProxy> 
TestAARMessageProcessor::CreateNewMovingActor(const std::string &meshName, 
                                                 float velocity, 
                                                 float turnRate, 
                                                 bool bSetLocation)
{
   if(mLogController->GetLastKnownStatus().GetStateEnum() ==
      dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
      return NULL;

   float xScale = 0.0f, yScale = 0.0f, zScale = 0.0f;
   float xRot = 0.0f, yRot = 0.0f, zRot = 0.0f;
   dtCore::RefPtr<dtGame::GameActorProxy> object;
   dtCore::Transform position;

   dtCore::RefPtr<dtDAL::ActorType> playerType = GetGameManager()->FindActorType("ExampleActors", "TestPlayer");
   object = dynamic_cast<dtGame::GameActorProxy *>(GetGameManager()->CreateActor(*playerType).get());

   if(bSetLocation)
   {
      object->SetTranslation(mPlayer->GetTranslation());

      // rescale our object to make it neat.
      zScale = dtUtil::RandFloat(0.70f, 1.3f);
      xScale = dtUtil::RandFloat(0.70f, 1.3f) * zScale;
      yScale = dtUtil::RandFloat(0.70f, 1.3f) * zScale;
      object->SetScale(osg::Vec3(xScale, yScale, zScale));

      // set initial random rotation (X = pitch, Y = roll, Z = yaw) for non rotating objects
      // don't change rotating objects cause the movement will follow the rotation, which may
      // look wierd.
      if(turnRate == 0.0f)
      {
         xRot = dtUtil::RandFloat(-5.0f, 5.0f);
         yRot = dtUtil::RandFloat(-5.0f, 5.0f);
         zRot = dtUtil::RandFloat(0.0f, 360.0f);
         object->SetRotation(osg::Vec3(xRot, yRot, zRot));
      }
   }

   GetGameManager()->AddActor(*object,false,false);

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
void TestAARMessageProcessor::PlaceActor()
{
   float turn,velocity;
   float chance,chance2;
   dtCore::RefPtr<dtGame::GameActorProxy> obj;

   turn = dtUtil::RandFloat(-0.60f, 0.60f);
   if (turn < 0.1f && turn > -0.1f)
      turn = 0.1f;

   velocity = dtUtil::RandFloat(-12.0f, 12.0f);
   if (velocity < 0.5f && velocity > -0.5f)
      velocity = 0.0f;

   chance = dtUtil::RandFloat(0.0, 1.0f);

   // make only some of them move cause it causes problems computing
   // the intersection with the ground. (Performance bug..)
   chance2 = dtUtil::RandFloat(0.0f, 1.0f);
   if (chance2 <= 0.75f)
      velocity = 0.0f;

   std::string path;
   if (chance <= 0.5f)
   {
      path = dtCore::FindFileInPathList("models/physics_crate.ive");
      if(!path.empty())
      {
         obj = CreateNewMovingActor(path,velocity,turn,true);
      }
      else
      {
         LOG_ERROR("Failed to find the physics_crate model file.");
      }
   }
   else
   {
      path = dtCore::FindFileInPathList("models/physics_barrel.ive");
      if(!path.empty())
      {
         obj = CreateNewMovingActor(path,velocity,turn,true);
      }
      else
      {
         LOG_ERROR("Failed to find the physics_barrel model file.");
      }
   }

   // fire a box created event
   dtCore::RefPtr<dtGame::GameEventMessage> eventMsg = static_cast<dtGame::GameEventMessage*>
      (GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());
   eventMsg->SetGameEvent(*TestAARGameEvent::EVENT_BOX_PLACED);
   GetGameManager()->SendMessage(*eventMsg);
}

//////////////////////////////////////////////////////////////////////////
void TestAARMessageProcessor::OnReceivedStatus(const dtGame::LogStatus &newStatus)
{
   static bool isFirstPlayback = true;

   // so we don't update again if user just requested it
   mLastAutoRequestStatus = GetGameManager()->GetSimulationTime();

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
         static_cast<dtActors::TaskActorProxy*>(mLmsComponent->GetTaskByName("Place Objects (Ordered)"));

      dtActors::TaskActorProxy *movePlayerRollup =
         static_cast<dtActors::TaskActorProxy*>(mLmsComponent->GetTaskByName("Move the Player (Rollup)"));

      dtActors::TaskActorProxy *movePlayerLeft =
         static_cast<dtActors::TaskActorProxy*>(mLmsComponent->GetTaskByName("Turn Player Left"));

      dtActors::TaskActorProxy *movePlayerRight =
         static_cast<dtActors::TaskActorProxy*>(mLmsComponent->GetTaskByName("Turn Player Right"));

      dtActors::TaskActorProxy *movePlayerForward =
         static_cast<dtActors::TaskActorProxy*>(mLmsComponent->GetTaskByName("Move Player Forward"));

      dtActors::TaskActorProxy *movePlayerBack =
         static_cast<dtActors::TaskActorProxy*>(mLmsComponent->GetTaskByName("Move Player Back"));

      dtActors::TaskActorProxy *drop5Boxes =
         static_cast<dtActors::TaskActorProxy*>(mLmsComponent->GetTaskByName("Drop 5 boxes"));

      //Recreate the hierarchy...
      movePlayerRollup->AddSubTaskProxy(*movePlayerLeft);
      movePlayerRollup->AddSubTaskProxy(*movePlayerRight);
      movePlayerRollup->AddSubTaskProxy(*movePlayerForward);
      movePlayerRollup->AddSubTaskProxy(*movePlayerBack);

      placeObjects->AddSubTaskProxy(*movePlayerRollup);
      placeObjects->AddSubTaskProxy(*drop5Boxes);

      //mTaskComponent->CheckTaskHierarchy();
      mLmsComponent->CheckTaskHierarchy();
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAARMessageProcessor::OnReceivedRejection(const dtGame::Message &newMessage)
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
void TestAARMessageProcessor::RequestAllControllerUpdates()
{
   mLogController->RequestServerGetStatus();
   mLogController->RequestServerGetKeyframes();
   mLogController->RequestServerGetTags();
}

//////////////////////////////////////////////////////////////////////////
void TestAARMessageProcessor::PreFrame(const double deltaFrameTime)
{
   // roughly every 3 real seconds, we force status update so the HUD updates and doesn't look broken.
   if (mLastAutoRequestStatus > GetGameManager()->GetSimulationTime())
      mLastAutoRequestStatus = GetGameManager()->GetSimulationTime();
   else if ((mLastAutoRequestStatus + 3.0*GetGameManager()->GetTimeScale()) < GetGameManager()->GetSimulationTime())
   {
      RequestAllControllerUpdates();
      mLastAutoRequestStatus = GetGameManager()->GetSimulationTime();
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAARMessageProcessor::SetupTasks()
{
   //Get our actor types...
   dtCore::RefPtr<dtDAL::ActorType> taskType = GetGameManager()->FindActorType("dtcore.Tasks","Task Actor");
   if (taskType == NULL)
      EXCEPT(AppException::INIT_ERROR,"Could not find task actor type.");
   dtCore::RefPtr<dtDAL::ActorType> eventTaskType = GetGameManager()->FindActorType("dtcore.Tasks","GameEvent Task Actor");
   if (eventTaskType == NULL)
      EXCEPT(AppException::INIT_ERROR,"Could not find Game Event Task Actor.");
   dtCore::RefPtr<dtDAL::ActorType> orderedTaskType = GetGameManager()->FindActorType("dtcore.Tasks","Ordered Task Actor");
   if (orderedTaskType == NULL)
      EXCEPT(AppException::INIT_ERROR,"Could not find Ordered Task Actor.");
   dtCore::RefPtr<dtDAL::ActorType> rollupTaskType = GetGameManager()->FindActorType("dtcore.Tasks","Rollup Task Actor");
   if (rollupTaskType == NULL)
      EXCEPT(AppException::INIT_ERROR,"Could not find Rollup Task Actor.");

   // task - root - event - start record  where are all the comments for this code
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskStartRecordProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(GetGameManager()->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskStart = static_cast<dtActors::TaskActorGameEvent &>
      (taskStartRecordProxy->GetGameActor());
   taskStart.SetGameEvent(TestAARGameEvent::EVENT_START_RECORD.get());
   taskStart.SetMinOccurances(1);
   taskStart.SetName("Start a Record");
   taskStart.SetNotifyLMSOnUpdate(true);
   taskStartRecordProxy->SetId(dtCore::UniqueId("Start a Record"));
   GetGameManager()->AddActor(*taskStartRecordProxy,false,false);

   // task - root - event - create 10 boxes
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskDrop10BoxesProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(GetGameManager()->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskDrop10Boxes = static_cast<dtActors::TaskActorGameEvent &>
      (taskDrop10BoxesProxy->GetGameActor());
   taskDrop10Boxes.SetGameEvent(TestAARGameEvent::EVENT_BOX_PLACED.get());
   taskDrop10Boxes.SetMinOccurances(10);
   taskDrop10Boxes.SetName("Drop 10 boxes");
   taskDrop10Boxes.SetNotifyLMSOnUpdate(true);
   taskDrop10BoxesProxy->SetId(dtCore::UniqueId("Drop 10 boxes"));
   GetGameManager()->AddActor(*taskDrop10BoxesProxy,false,false);

   // * task - root - base - move camera, any direction
   mTaskMoveCameraProxy = dynamic_cast<dtActors::TaskActorProxy *>(GetGameManager()->CreateActor(*taskType).get());
   mTaskMoveCamera = static_cast<dtActors::TaskActor*>(&(mTaskMoveCameraProxy->GetGameActor()));
   mTaskMoveCamera->SetNotifyLMSOnUpdate(true);
   mTaskMoveCameraProxy->SetId(dtCore::UniqueId("TaskMoveCamera"));
   mTaskMoveCameraProxy->SetName("Move Camera");
   mTaskMoveCameraProxy->SetId(dtCore::UniqueId("Move Camera"));
   GetGameManager()->AddActor(*mTaskMoveCameraProxy,false,false);

   // SETUP THE NESTED TASKS

   // task - root - ordered (blocking) - 'Place Objects'
   dtCore::RefPtr<dtActors::TaskActorOrderedProxy> taskPlaceObjectsProxy =
      dynamic_cast<dtActors::TaskActorOrderedProxy*>(GetGameManager()->CreateActor(*orderedTaskType).get());
   dtActors::TaskActorOrdered &taskPlaceObjects = static_cast<dtActors::TaskActorOrdered &>
      (taskPlaceObjectsProxy->GetGameActor());
   taskPlaceObjects.SetFailureType(dtActors::TaskActorOrdered::FailureType::BLOCK);
   taskPlaceObjects.SetName("Place Objects (Ordered)");
   taskPlaceObjects.SetNotifyLMSOnUpdate(true);
   taskPlaceObjectsProxy->SetId(dtCore::UniqueId("Place Objects (Ordered)"));
   GetGameManager()->AddActor(*taskPlaceObjectsProxy,false,false);

   //   task - child - rollup - (.75 to pass) - 'Move Player'
   dtCore::RefPtr<dtActors::TaskActorRollupProxy> taskMovePlayerProxy =
      dynamic_cast<dtActors::TaskActorRollupProxy*>(GetGameManager()->CreateActor(*rollupTaskType).get());
   dtActors::TaskActorRollup &taskMovePlayer = static_cast<dtActors::TaskActorRollup &>
      (taskMovePlayerProxy->GetGameActor());
   taskMovePlayer.SetName("Move the Player (Rollup)");
   taskMovePlayerProxy->SetId(dtCore::UniqueId("Move the Player (Rollup)"));
   taskMovePlayer.SetPassingScore(0.75f); // only need 3 to succeed
   taskMovePlayer.SetNotifyLMSOnUpdate(true);
   taskPlaceObjectsProxy->AddSubTaskProxy(*taskMovePlayerProxy);
   GetGameManager()->AddActor(*taskMovePlayerProxy,false,false);

   //      task - child - event - move left (.25)
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskPlayerLeftProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(GetGameManager()->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskPlayerLeft = static_cast<dtActors::TaskActorGameEvent &>
      (taskPlayerLeftProxy->GetGameActor());
   taskPlayerLeft.SetGameEvent(TestAARGameEvent::EVENT_PLAYER_LEFT.get());
   taskPlayerLeft.SetMinOccurances(1);
   taskPlayerLeft.SetWeight(0.25f);
   taskPlayerLeft.SetName("Turn Player Left");
   taskPlayerLeft.SetNotifyLMSOnUpdate(true);
   taskPlayerLeftProxy->SetId(dtCore::UniqueId("Turn Player Left"));
   taskMovePlayerProxy->AddSubTaskProxy(*taskPlayerLeftProxy);
   GetGameManager()->AddActor(*taskPlayerLeftProxy,false,false);

   //      task - child - event - move right (.25)
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskPlayerRightProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(GetGameManager()->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskPlayerRight = static_cast<dtActors::TaskActorGameEvent &>
      (taskPlayerRightProxy->GetGameActor());
   taskPlayerRight.SetGameEvent(TestAARGameEvent::EVENT_PLAYER_RIGHT.get());
   taskPlayerRight.SetMinOccurances(1);
   taskPlayerRight.SetWeight(0.25f);
   taskPlayerRight.SetName("Turn Player Right");
   taskPlayerRight.SetNotifyLMSOnUpdate(true);
   taskPlayerRightProxy->SetId(dtCore::UniqueId("Turn Player Right"));
   taskMovePlayerProxy->AddSubTaskProxy(*taskPlayerRightProxy);
   GetGameManager()->AddActor(*taskPlayerRightProxy,false,false);

   //      task - child - event - move forward (.25)
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskPlayerForwardProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(GetGameManager()->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskPlayerForward = static_cast<dtActors::TaskActorGameEvent &>
      (taskPlayerForwardProxy->GetGameActor());
   taskPlayerForward.SetGameEvent(TestAARGameEvent::EVENT_PLAYER_FORWARD.get());
   taskPlayerForward.SetMinOccurances(1);
   taskPlayerForward.SetWeight(0.25f);
   taskPlayerForward.SetName("Move Player Forward");
   taskPlayerForward.SetNotifyLMSOnUpdate(true);
   taskPlayerForwardProxy->SetId(dtCore::UniqueId("Move Player Forward"));
   taskMovePlayerProxy->AddSubTaskProxy(*taskPlayerForwardProxy);
   GetGameManager()->AddActor(*taskPlayerForwardProxy,false,false);

   //      task - child - event - move back (.25)
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskPlayerBackProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(GetGameManager()->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskPlayerBack = static_cast<dtActors::TaskActorGameEvent &>
      (taskPlayerBackProxy->GetGameActor());
   taskPlayerBack.SetGameEvent(TestAARGameEvent::EVENT_PLAYER_BACKWARD.get());
   taskPlayerBack.SetMinOccurances(1);
   taskPlayerBack.SetWeight(0.25f);
   taskPlayerBack.SetName("Move Player Back");
   taskPlayerBack.SetNotifyLMSOnUpdate(true);
   taskPlayerBackProxy->SetId(dtCore::UniqueId("Move Player Back"));
   taskMovePlayerProxy->AddSubTaskProxy(*taskPlayerBackProxy);
   GetGameManager()->AddActor(*taskPlayerBackProxy,false,false);

   //   task - child - event - place 5 boxes
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> taskDrop5BoxesProxy =
      dynamic_cast<dtActors::TaskActorGameEventProxy*>(GetGameManager()->CreateActor(*eventTaskType).get());
   dtActors::TaskActorGameEvent &taskDrop5Boxes = static_cast<dtActors::TaskActorGameEvent &>
      (taskDrop5BoxesProxy->GetGameActor());
   taskDrop5Boxes.SetGameEvent(TestAARGameEvent::EVENT_BOX_PLACED.get());
   taskDrop5Boxes.SetMinOccurances(5);
   taskDrop5Boxes.SetName("Drop 5 boxes");
   taskDrop5Boxes.SetNotifyLMSOnUpdate(true);
   taskDrop5BoxesProxy->SetId(dtCore::UniqueId("Drop 5 boxes"));
   taskPlaceObjectsProxy->AddSubTaskProxy(*taskDrop5BoxesProxy);
   GetGameManager()->AddActor(*taskDrop5BoxesProxy,false,false);
}

//////////////////////////////////////////////////////////////////////////
void TestAARMessageProcessor::Reset()
{
   GetGameManager()->DeleteAllActors();
   mLmsComponent->ClearTaskList();

   dtCore::System::Instance()->Step();

   dtCore::RefPtr<dtDAL::ActorType> playerType = GetGameManager()->FindActorType("ExampleActors", "TestPlayer");
   dtCore::RefPtr<dtDAL::ActorProxy> player = GetGameManager()->CreateActor(*playerType);
   mPlayer = dynamic_cast<dtGame::GameActorProxy*>(player.get());
   GetGameManager()->AddActor(*mPlayer, false, false);

   dtDAL::StringActorProperty *prop = static_cast<dtDAL::StringActorProperty*>(mPlayer->GetProperty("mesh"));
   std::string path = dtCore::FindFileInPathList("models/physics_happy_sphere.ive");
   if(!path.empty())
   {
      prop->SetValue(path);
   }
   else
   {
      LOG_ERROR("Failed to find the physics_happy_sphere file.");
   }

   dtGame::GMComponent *gmc = GetGameManager()->GetComponentByName("TestInputComponent");
   if(gmc != NULL)
   {
      static_cast<TestAARInput*>(gmc)->SetPlayerActor(*mPlayer);
   }

   SetupTasks();
}

//////////////////////////////////////////////////////////////////////////
void TestAARMessageProcessor::PrintTasks()
{
   std::ostringstream printer;
   std::vector<dtCore::RefPtr<dtGame::GameActorProxy> > tasks;
   std::vector<dtCore::RefPtr<dtGame::GameActorProxy> >::iterator itor;

   printer << "Number of Top Level Tasks: " << mLmsComponent->GetNumTopLevelTasks() <<
      " Total Number of Tasks: " << mLmsComponent->GetNumTasks();

   mLmsComponent->GetAllTasks(tasks);
   printer << std::endl << "Task List:" << std::endl;
   for (itor=tasks.begin(); itor!=tasks.end(); ++itor)
   {
      printer << "\tTask Name: " << (*itor)->GetName() <<
         " Complete: " << (*itor)->GetProperty("Complete")->GetStringValue() << std::endl;
   }

   std::cout << printer.str() << std::endl;
}

//////////////////////////////////////////////////////////////////////////
void TestAARMessageProcessor::OnReceivedTags(const std::vector<dtGame::LogTag> &newTagList)
{
   // left this cruft in for debugging
   //std::ostringstream ss;
   //ss << "## RECEIVED TAG LIST ##: [" << newTagList.size() << "] tags";
   //std::cout << ss.str() << std::endl;
}

//////////////////////////////////////////////////////////////////////////
void TestAARMessageProcessor::OnReceivedKeyframes(const std::vector<dtGame::LogKeyframe> &newKeyframesList)
{
   // left this cruft in for debugging...
   //std::ostringstream ss;
   //ss << "## RECEIVED Keyframes LIST ##: [" << newKeyframesList.size() << "] keyframes";
   //std::cout << ss.str() << std::endl;
}

//////////////////////////////////////////////////////////////////////////
void TestAARMessageProcessor::UpdateTaskCamera()
{
   if(mLogController->GetLastKnownStatus().GetStateEnum() != dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
   {
      if(!mTaskMoveCamera->IsComplete())
      {
         mTaskMoveCamera->SetScore(1.0);
         mTaskMoveCamera->SetComplete(true);

         dtActors::TaskActorProxy &proxy =
            static_cast<dtActors::TaskActorProxy&>(mTaskMoveCamera->GetGameActorProxy());
         proxy.NotifyActorUpdate();
      }
   }
}

///////////////////////////////////////////////////////////////////////////
void TestAARMessageProcessor::UpdatePlayerActor(const dtGame::ActorUpdateMessage &aum)
{
   dtDAL::ActorProxy *gap = GetGameManager()->FindActorById(aum.GetAboutActorId());
   if(gap != mPlayer)
      return;

   dtDAL::FloatActorProperty *playerVelocity = static_cast<dtDAL::FloatActorProperty*>(mPlayer->GetProperty("velocity"));
   dtDAL::FloatActorProperty *playerTurnRate = static_cast<dtDAL::FloatActorProperty*>(mPlayer->GetProperty("turnrate"));
 
   const dtGame::MessageParameter *mp = aum.GetUpdateParameter("Velocity");
   if(mp != NULL)
      playerVelocity->SetValue(static_cast<const dtGame::FloatMessageParameter*>(mp)->GetValue());

   mp = aum.GetUpdateParameter("Turn Rate");
   if(mp != NULL)
      playerTurnRate->SetValue(static_cast<const dtGame::FloatMessageParameter*>(mp)->GetValue());
}
