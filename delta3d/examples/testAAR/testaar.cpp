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

#include "testaar.h"

#include <dtCore/object.h>
#include <dtCore/globals.h>
#include <dtCore/flymotionmodel.h>
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
#include <dtActors/taskactor.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/transformableactorproxy.h>

#include <ctime>

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(AppException);
AppException AppException::INIT_ERROR("INIT_ERROR");


//////////////////////////////////////////////////////////////////////////
AARApplication::AARApplication() : dtABC::Application("testaarconfig.xml"), mKeyIsPressed(false)
{
   mSimSpeedFactor = 1.0;
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
   dtGame::ServerLoggerComponent *logger = new dtGame::ServerLoggerComponent(*logStream);
      
   mLogController = new dtGame::LogController();
   mLogController->SignalReceivedStatus().connect_slot(this, &AARApplication::OnReceivedStatus);
   mLogController->SignalReceivedRejection().connect_slot(this, &AARApplication::OnReceivedRejection);
   
   mClientGM->AddComponent(*logger, dtGame::GameManager::ComponentPriority::NORMAL);
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
   #if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
      mClientGM->LoadActorRegistry("testGameActorLibraryd");
   #else
      mClientGM->LoadActorRegistry("testGameActorLibrary");
   #endif  
   
   SetupScene();
   SetupTasks();
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::SetupScene()
{
   mPlayer = CreateNewMovingActor("models/physics_happy_sphere.ive", 0.0f, 0.0f, false);
   mPlayerIsValid = true;

   dtCore::Object *terrain = new dtCore::Object();
   terrain->LoadFile("models/dirt.ive");
   GetScene()->AddDrawable(terrain);  
   
   mMotionModel = new dtCore::FlyMotionModel(GetKeyboard(),GetMouse());
   mMotionModel->SetTarget(GetCamera());
   dtCore::Transform tx(0.0f,-10.0f,0.0f);
   GetCamera()->SetTransform(&tx); 
}

//////////////////////////////////////////////////////////////////////////
void AARApplication::SetupTasks()
{
   //Get our actor types...
   dtCore::RefPtr<dtDAL::ActorType> taskType = mClientGM->FindActorType("dtcore.Tasks","Task Actor");
   if (taskType == NULL)
      EXCEPT(AppException::INIT_ERROR,"Could not find task actor type.");
   dtCore::RefPtr<dtDAL::ActorType> objCountType = mClientGM->FindActorType("ExampleActors","Counter Task");
   if (objCountType == NULL)
      EXCEPT(AppException::INIT_ERROR,"Could not find counter task actor.");
    
   //Create a root task.  This represents a logical top level suite of tasks to complete in this
   //demo application.  
   mRootTask = dynamic_cast<dtActors::TaskActorProxy *>(mClientGM->CreateActor(*taskType).get());
   if (mRootTask == NULL)
      EXCEPT(AppException::INIT_ERROR,"Error creating root task actor.");
   else
      mRootTask->SetName("Task Suite");
   
   //Create a task to track the number of objects (boxes or cylinders) placed in the world
   //by the player.  For this task, the player must place 5 objects to complete the task.
   mObjectPlacementTask = dynamic_cast<dtActors::TaskActorProxy*>(mClientGM->CreateActor(*objCountType).get());
   if (mObjectPlacementTask == NULL)
      EXCEPT(AppException::INIT_ERROR,"Error creating counter task actor.");
   mObjectPlacementTask->GetProperty("Counter")->SetStringValue("0");
   mObjectPlacementTask->GetProperty("TargetCount")->SetStringValue("5");
   mObjectPlacementTask->SetName("Object Placement Task");
   
   //Add all tasks to the game manager, and add the counter task as a child of our top level task
   //suite.
   mRootTask->AddSubTaskProxy(*mObjectPlacementTask);
   mClientGM->AddActor(*mObjectPlacementTask,false,false);
   mClientGM->AddActor(*mRootTask,false,false);   
}

//////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<dtGame::GameActorProxy> AARApplication::CreateNewMovingActor(
   const std::string &meshName, float velocity, float turnRate, bool bSetLocation)
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
      zScale = Random(0.80f, 1.2f);
      xScale = Random(0.80f, 1.2f) * zScale;
      yScale = Random(0.80f, 1.2f) * zScale;
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
   if (mPlayerIsValid)
   {
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
}

//////////////////////////////////////////////////////////////////////////
bool AARApplication::HandleKeyPressed(dtCore::Keyboard *keyBoard, 
   Producer::KeyboardKey key, Producer::KeyCharacter character)
{
   dtABC::Application::KeyPressed(keyBoard,key,character); 
   
   std::ostringstream ss;   
   switch (key)
   {
      case Producer::Key_space:
         mLogController->RequestServerGetStatus();
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

      case Producer::Key_bracketright:
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
         PlaceActor();         
         break;
         
      case Producer::Key_T:
         PrintTasks();
         break;

      case Producer::Key_Return:
         GetCamera()->SetNextStatisticsType();
         break;

      default:
         break;         
   };  

   return true;
}

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
      
   //Update our simple counter task.
   dtDAL::IntActorProperty *prop = static_cast<dtDAL::IntActorProperty *>
      (mObjectPlacementTask->GetProperty("Counter"));
   prop->SetValue(prop->GetValue()+1);
}
//////////////////////////////////////////////////////////////////////////
void AARApplication::OnReceivedStatus(const dtGame::LogStatus &newStatus)
{
   std::cout << "##STATUS MESSAGE##" << newStatus << std::endl;
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
