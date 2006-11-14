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
 * William E. Johnson II
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtLMS/lmstaskstatus.h>
#include <dtLMS/lmsmessage.h>
#include <dtLMS/lmsmessagetype.h>
#include <dtLMS/lmscomponent.h>
#include <dtLMS/lmsexceptionenum.h>
#include <dtLMS/lmsmessagevalue.h>
#include <dtLMS/lmsclientsocket.h>
#include <dtCore/scene.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtGame/gamemanager.h>
#include <dtActors/taskactor.h>
#include <dtActors/engineactorregistry.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #define W32
#endif

class TestLmsComponent : public dtLMS::LmsComponent
{
   public:

      TestLmsComponent(const std::string &name) : 
         dtLMS::LmsComponent(name) 
      {

      } 

      TestLmsComponent(const std::string &host, int port, bool reverseBytes) : 
         dtLMS::LmsComponent(host, port, reverseBytes)
      {

      }

      dtLMS::LmsMessage TranslateObjectiveScoreMessage(const dtCore::UniqueId &taskID, float taskScore)
      {
         return dtLMS::LmsComponent::TranslateObjectiveScoreMessage(taskID, taskScore);
      }

      dtLMS::LmsMessage TranslateObjectiveCompleteMessage(const dtCore::UniqueId &taskID, bool taskIsComplete)
      {
         return dtLMS::LmsComponent::TranslateObjectiveCompleteMessage(taskID, taskIsComplete);
      }

      void SetNeedValidSocket(bool need)
      {
         dtLMS::LmsComponent::SetNeedValidSocket(need);
      }

      bool GetNeedValidSocket() const 
      {
         return dtLMS::LmsComponent::GetNeedValidSocket();
      }

      void SendLmsUpdate(dtGame::GameActorProxy &proxy)
      {
         dtLMS::LmsComponent::SendLmsUpdate(proxy);
      }

   protected:

      virtual ~TestLmsComponent() { }
};

class LMSTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(LMSTests);
   
      CPPUNIT_TEST(TestTaskStatus);
      CPPUNIT_TEST(TestLmsMessage);
      CPPUNIT_TEST(TestLmsComponentHelperMethods);
      CPPUNIT_TEST(TestLmsComponentMessaging);

   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp();
      void tearDown();
      void TestTaskStatus();
      void TestLmsMessage();
      void TestLmsComponentHelperMethods();
      void TestLmsComponentMessaging();

   private:

      dtCore::RefPtr<dtGame::GameManager> mGameManager;
};

CPPUNIT_TEST_SUITE_REGISTRATION(LMSTests);

void LMSTests::setUp()
{
   dtCore::Scene *scene = new dtCore::Scene;
   mGameManager = new dtGame::GameManager(*scene);
}

void LMSTests::tearDown()
{
   if(mGameManager.valid())
   {
      mGameManager->DeleteAllActors(true);
      mGameManager = NULL;
   }
}

void LMSTests::TestTaskStatus()
{
   dtLMS::LmsTaskStatus taskStatus;
   CPPUNIT_ASSERT_EQUAL(taskStatus.GetScore(), 0.0f);
   CPPUNIT_ASSERT(!taskStatus.GetCompleted());

   float score = 5.132f;
   taskStatus.SetCompleted(true);
   taskStatus.SetScore(score);
   CPPUNIT_ASSERT(taskStatus.GetCompleted());
   CPPUNIT_ASSERT_EQUAL(taskStatus.GetScore(), score);
}

void LMSTests::TestLmsMessage()
{
   dtLMS::LmsMessage msg;
   CPPUNIT_ASSERT(msg.GetDelimiter() == ":");
   CPPUNIT_ASSERT(msg.GetMessageType() == dtLMS::LmsMessageType::SIMULATION);
   CPPUNIT_ASSERT(msg.GetObjectiveID().empty());
   CPPUNIT_ASSERT(msg.GetSenderID().empty());
   CPPUNIT_ASSERT(msg.GetValue().empty());

   msg.SetDelimiter(":");
   msg.SetMessageType(dtLMS::LmsMessageType::OBJECTIVE_SCORE);
   msg.SetObjectiveID("ID");
   msg.SetSenderID("ME");
   msg.SetValue("Amaranthine");

   CPPUNIT_ASSERT(msg.GetDelimiter() == ":");
   CPPUNIT_ASSERT(msg.GetMessageType() == dtLMS::LmsMessageType::OBJECTIVE_SCORE);
   CPPUNIT_ASSERT(msg.GetObjectiveID() == "ID");
   CPPUNIT_ASSERT(msg.GetSenderID() == "ME");
   CPPUNIT_ASSERT(msg.GetValue() == "Amaranthine");
}

void LMSTests::TestLmsComponentHelperMethods()
{
   dtCore::RefPtr<TestLmsComponent> lmsComp = new TestLmsComponent("TestLmsComponent");
   CPPUNIT_ASSERT(lmsComp.valid());
   
   dtCore::UniqueId id, senderId;;
   float taskScore = 5.4f;
   std::ostringstream oss;
   oss << taskScore;

   dtLMS::LmsMessage msg = lmsComp->TranslateObjectiveScoreMessage(id, taskScore);
   CPPUNIT_ASSERT_MESSAGE("The objective id should be the value passed in", msg.GetObjectiveID() == id.ToString());
   CPPUNIT_ASSERT_MESSAGE("The sender id should be be empty", msg.GetSenderID().empty());
   CPPUNIT_ASSERT_MESSAGE("The value should also be what was passed in", msg.GetValue() == oss.str());
   CPPUNIT_ASSERT_MESSAGE("The message type should have been set correctly", msg.GetMessageType() == dtLMS::LmsMessageType::OBJECTIVE_SCORE);

   msg = lmsComp->TranslateObjectiveCompleteMessage(id, false);
   CPPUNIT_ASSERT_MESSAGE("The id should have been set correctly", msg.GetObjectiveID() == id.ToString());
   CPPUNIT_ASSERT_MESSAGE("The sender id should be empty", msg.GetSenderID().empty());
   CPPUNIT_ASSERT_MESSAGE("The value should be set correctly", msg.GetValue() == dtLMS::LmsMessageValue::ObjectiveCompletionValue::INCOMPLETE.GetName());
   CPPUNIT_ASSERT_MESSAGE("The message should be set correctly", msg.GetMessageType() == dtLMS::LmsMessageType::OBJECTIVE_COMPLETION);

   msg.SetSenderID(senderId.ToString());

   std::string msgString = msg.ToString();
   oss.str("");
   oss << msg.GetSenderID() << msg.GetDelimiter() << msg.GetMessageType().GetName() 
       << msg.GetDelimiter() << msg.GetValue() << msg.GetDelimiter() << msg.GetObjectiveID();

   CPPUNIT_ASSERT_EQUAL(oss.str(), msgString);

   dtLMS::LmsMessage otherMsg;
   otherMsg.BuildFromString(oss.str());
   CPPUNIT_ASSERT_EQUAL(msg.GetMessageType(), otherMsg.GetMessageType());
   CPPUNIT_ASSERT_EQUAL(msg.GetSenderID(),    otherMsg.GetSenderID());
   CPPUNIT_ASSERT_EQUAL(msg.GetObjectiveID(), otherMsg.GetObjectiveID());
   CPPUNIT_ASSERT_EQUAL(msg.GetValue(),       otherMsg.GetValue());
   CPPUNIT_ASSERT_EQUAL(msg.GetDelimiter(),   otherMsg.GetDelimiter());
}

void LMSTests::TestLmsComponentMessaging()
{
   dtCore::RefPtr<TestLmsComponent> lmsComp = new TestLmsComponent("TestLmsComponent");
   CPPUNIT_ASSERT(lmsComp.valid());
   mGameManager->AddComponent(*lmsComp, dtGame::GameManager::ComponentPriority::NORMAL);
   
   CPPUNIT_ASSERT(lmsComp->GetLmsClientSocket() == NULL);
   try
   {
      lmsComp->ConnectToLms();
   }
   catch(const dtUtil::Exception &)
   {
      // correct
   }
   CPPUNIT_ASSERT_MESSAGE("Even if the connection fails, the client socket should be valid", lmsComp->GetLmsClientSocket() != NULL);

   CPPUNIT_ASSERT(lmsComp->GetNeedValidSocket());
   lmsComp->SetNeedValidSocket(false);
   CPPUNIT_ASSERT(!lmsComp->GetNeedValidSocket());

   std::vector<dtLMS::LmsMessage> msgs;
   lmsComp->GetMessageVector(msgs);
   CPPUNIT_ASSERT(msgs.empty());

   dtCore::RefPtr<dtActors::TaskActorProxy> proxy;
   mGameManager->CreateActor(*dtActors::EngineActorRegistry::TASK_ACTOR_TYPE, proxy);
   CPPUNIT_ASSERT(proxy.valid());
   mGameManager->AddActor(*proxy, false, false);

   lmsComp->SendLmsUpdate(*proxy);

   lmsComp->GetMessageVector(msgs);
   CPPUNIT_ASSERT_MESSAGE("The Complete property is false, nothing should have happened", msgs.empty());

   static_cast<dtDAL::BooleanActorProperty*>(proxy->GetProperty("Complete"))->SetValue(true);
   lmsComp->SendLmsUpdate(*proxy);
   lmsComp->GetMessageVector(msgs);
   CPPUNIT_ASSERT(!msgs.empty());

   dtLMS::LmsMessage msg = msgs[0];
   CPPUNIT_ASSERT_MESSAGE("The id of the message should match the id of the proxy", msg.GetObjectiveID() == proxy->GetId().ToString());
   CPPUNIT_ASSERT_MESSAGE("The value of the message should match the property", 
      msg.GetValue() == dtLMS::LmsMessageValue::ObjectiveCompletionValue::COMPLETE.GetName());

   static_cast<dtDAL::BooleanActorProperty*>(proxy->GetProperty("Complete"))->SetValue(false);
   lmsComp->SendLmsUpdate(*proxy);
   lmsComp->GetMessageVector(msgs);
   CPPUNIT_ASSERT(!msgs.empty());

   msg = msgs[0];
   CPPUNIT_ASSERT_MESSAGE("The id of the message should match the id of the proxy", msg.GetObjectiveID() == proxy->GetId().ToString());
   CPPUNIT_ASSERT_MESSAGE("The value of the message should match the property", 
      msg.GetValue() == dtLMS::LmsMessageValue::ObjectiveCompletionValue::INCOMPLETE.GetName());

   static_cast<dtDAL::BooleanActorProperty*>(proxy->GetProperty("Complete"))->SetValue(false);
   lmsComp->SendLmsUpdate(*proxy);
   lmsComp->GetMessageVector(msgs);
   CPPUNIT_ASSERT_MESSAGE("Assigning the property to its same value should do nothing", msgs.empty());

   const float score = 0.8f;
   static_cast<dtDAL::FloatActorProperty*>(proxy->GetProperty("Score"))->SetValue(score);
   lmsComp->SendLmsUpdate(*proxy);
   lmsComp->GetMessageVector(msgs);
   CPPUNIT_ASSERT(!msgs.empty());
   
   msg = msgs[0];
   std::ostringstream oss;
   oss << score;
   CPPUNIT_ASSERT_MESSAGE("The value of the message should be correct", msg.GetValue() == oss.str());
   CPPUNIT_ASSERT_MESSAGE("The message type should be correct", msg.GetMessageType() == dtLMS::LmsMessageType::OBJECTIVE_SCORE);
   CPPUNIT_ASSERT_MESSAGE("The id of the message should be the proxy's id", msg.GetObjectiveID() == proxy->GetId().ToString());

   static_cast<dtDAL::FloatActorProperty*>(proxy->GetProperty("Score"))->SetValue(1.3f);
   lmsComp->SendLmsUpdate(*proxy);
   lmsComp->GetMessageVector(msgs);
   CPPUNIT_ASSERT(!msgs.empty());

   msg = msgs[0];
   oss.str("");
   oss << 1;
   CPPUNIT_ASSERT_MESSAGE("The value of the message should be 1, since the value is clamped", msg.GetValue() == oss.str());
   CPPUNIT_ASSERT_MESSAGE("The message type should be correct", msg.GetMessageType() == dtLMS::LmsMessageType::OBJECTIVE_SCORE);
   CPPUNIT_ASSERT_MESSAGE("The id of the message should be the proxy's id", msg.GetObjectiveID() == proxy->GetId().ToString());

   static_cast<dtDAL::FloatActorProperty*>(proxy->GetProperty("Score"))->SetValue(5.0f);
   lmsComp->SendLmsUpdate(*proxy);
   lmsComp->GetMessageVector(msgs);
   CPPUNIT_ASSERT_MESSAGE("The value is clamped, internally, nothing should have happened", msgs.empty());

   lmsComp = NULL;
}



