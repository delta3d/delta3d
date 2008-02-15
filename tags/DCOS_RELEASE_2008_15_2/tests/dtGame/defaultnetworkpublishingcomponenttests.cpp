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
 * @author Eddie Johnson and David Guthrie
 */
#include <prefix/dtgameprefix-src.h>

#include <dtUtil/log.h>
#include <dtUtil/macros.h>


#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/globals.h>

#include <dtDAL/datatype.h>

#include <dtGame/messageparameter.h>
#include <dtGame/machineinfo.h>
#include <dtGame/gameactor.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/defaultnetworkpublishingcomponent.h>
#include <dtGame/defaultmessageprocessor.h>

#include <dtActors/engineactorregistry.h>

#include "testcomponent.h"

#include <cppunit/extensions/HelperMacros.h>

#include <dtABC/application.h>
extern dtABC::Application& GetGlobalApplication();

#ifdef DELTA_WIN32
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif
namespace dtGame
{
   class DefaultNetworkPublishingComponentTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(DefaultNetworkPublishingComponentTests);
   
         CPPUNIT_TEST(TestPublishActor);
         CPPUNIT_TEST(TestUpdateActor);
         CPPUNIT_TEST(TestDeleteActor);
         CPPUNIT_TEST(TestUpdateUnpublishedActor);
   
      CPPUNIT_TEST_SUITE_END();
   
   public:
      void setUp();
      void tearDown();

      void TestPublishActor();
      void TestUpdateActor();
      void TestDeleteActor();
      void TestUpdateUnpublishedActor();

   private:
   
      dtUtil::Log* mLogger;
   
      dtCore::RefPtr<GameManager> mGameManager;
      dtCore::RefPtr<DefaultNetworkPublishingComponent> mNetPubComp;
      dtCore::RefPtr<DefaultMessageProcessor> mDefMsgProc;
      dtCore::RefPtr<TestComponent> mTestComp;
      dtCore::RefPtr<GameActorProxy> mGameActorProxy;
   
   };
   
   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(DefaultNetworkPublishingComponentTests);
   
   
   //////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponentTests::setUp()
   {
      try
      {
         dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
         mLogger = &dtUtil::Log::GetInstance("defaultnetworkpublishingcomponenttests.cpp");
   
         mGameManager = new dtGame::GameManager(*GetGlobalApplication().GetScene());
         mGameManager->SetApplication(GetGlobalApplication());
         
         mNetPubComp = new DefaultNetworkPublishingComponent;
         mDefMsgProc = new DefaultMessageProcessor;
         mTestComp = new TestComponent;
         
         mGameManager->AddComponent(*mDefMsgProc, GameManager::ComponentPriority::HIGHEST);
         mGameManager->AddComponent(*mNetPubComp, GameManager::ComponentPriority::NORMAL);
         mGameManager->AddComponent(*mTestComp, GameManager::ComponentPriority::NORMAL);
         
         mGameManager->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, mGameActorProxy);
         
         dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
         dtCore::System::GetInstance().Start();

         mTestComp->reset();
         //Publish the actor.
         mGameManager->AddActor(*mGameActorProxy, false, false);
         dtCore::System::GetInstance().Step();
      }
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL((std::string("Error: ") + ex.ToString()).c_str());
      }
   
   }
   
   //////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponentTests::tearDown()
   {
      if(mGameManager.valid())
      {
         try
         {
            dtCore::System::GetInstance().SetPause(false);
            dtCore::System::GetInstance().Stop();
   
            mGameManager->DeleteAllActors(true);
   
            mGameManager = NULL;
            mNetPubComp  = NULL;
            mDefMsgProc  = NULL;
            mTestComp    = NULL;
         }
         catch(const dtUtil::Exception &e)
         {
            CPPUNIT_FAIL((std::string("Error: ") + e.ToString()).c_str());
         }
      }
   
   }
   
   //////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponentTests::TestPublishActor()
   {
      mGameManager->PublishActor(*mGameActorProxy);
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL(1U, unsigned(mTestComp->GetReceivedDispatchNetworkMessages().size()));
      const ActorUpdateMessage* createMessage = dynamic_cast<const ActorUpdateMessage*>(mTestComp->GetReceivedDispatchNetworkMessages()[0].get());
      CPPUNIT_ASSERT(createMessage != NULL);
      CPPUNIT_ASSERT(createMessage->GetMessageType() == MessageType::INFO_ACTOR_CREATED);
      CPPUNIT_ASSERT_EQUAL(createMessage->GetAboutActorId(), mGameActorProxy->GetId());
   }

   //////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponentTests::TestDeleteActor()
   {
      mGameManager->PublishActor(*mGameActorProxy);
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      mTestComp->reset();

      mGameManager->DeleteActor(*mGameActorProxy);
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL(1U, unsigned(mTestComp->GetReceivedDispatchNetworkMessages().size()));
      const Message* deleteMessage = mTestComp->GetReceivedDispatchNetworkMessages()[0].get();
      CPPUNIT_ASSERT(deleteMessage != NULL);
      CPPUNIT_ASSERT(deleteMessage->GetMessageType() == MessageType::INFO_ACTOR_DELETED);
      CPPUNIT_ASSERT_EQUAL(deleteMessage->GetAboutActorId(), mGameActorProxy->GetId());
   }

   //////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponentTests::TestUpdateActor()
   {
      mGameManager->PublishActor(*mGameActorProxy);
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      mTestComp->reset();

      mGameActorProxy->NotifyFullActorUpdate();
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();
      
      CPPUNIT_ASSERT_EQUAL(1U, unsigned(mTestComp->GetReceivedDispatchNetworkMessages().size()));
      const ActorUpdateMessage* updateMessage = dynamic_cast<const ActorUpdateMessage*>(mTestComp->GetReceivedDispatchNetworkMessages()[0].get());
      CPPUNIT_ASSERT(updateMessage != NULL);
      CPPUNIT_ASSERT(updateMessage->GetMessageType() == MessageType::INFO_ACTOR_UPDATED);
      CPPUNIT_ASSERT_EQUAL(updateMessage->GetAboutActorId(), mGameActorProxy->GetId());
   }

   //////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponentTests::TestUpdateUnpublishedActor()
   {
      mGameActorProxy->NotifyFullActorUpdate();
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();
      
      CPPUNIT_ASSERT_EQUAL(0U, unsigned(mTestComp->GetReceivedDispatchNetworkMessages().size()));

      mGameManager->DeleteActor(*mGameActorProxy);
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL(0U, unsigned(mTestComp->GetReceivedDispatchNetworkMessages().size()));
   }
}
