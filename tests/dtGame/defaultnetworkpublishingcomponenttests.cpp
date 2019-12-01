/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 * @author Eddie Johnson and David Guthrie
 */

#include <prefix/unittestprefix.h>

#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>

#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>

#include <dtCore/datatype.h>

#include <dtGame/messageparameter.h>
#include <dtGame/machineinfo.h>
#include <dtGame/gameactor.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/defaultnetworkpublishingcomponent.h>

#include <dtActors/engineactorregistry.h>

#include "basegmtests.h"

#include <dtABC/application.h>

namespace dtGame
{
   class DefaultNetworkPublishingComponentTests : public BaseGMTestFixture
   {
      typedef BaseGMTestFixture BaseClass;

      CPPUNIT_TEST_SUITE(DefaultNetworkPublishingComponentTests);

         CPPUNIT_TEST(TestPublishActor);
         CPPUNIT_TEST(TestUpdateActor);
         CPPUNIT_TEST(TestDeleteActor);
         CPPUNIT_TEST(TestUpdateUnpublishedActor);
         CPPUNIT_TEST(TestAdditionalTypesToPublishAddRemove);
         CPPUNIT_TEST(TestAdditionalTypesToPublishFunction);

      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestPublishActor();
      void TestUpdateActor();
      void TestDeleteActor();
      void TestUpdateUnpublishedActor();
      void TestAdditionalTypesToPublishAddRemove();
      void TestAdditionalTypesToPublishFunction();

   private:

      dtCore::RefPtr<DefaultNetworkPublishingComponent> mNetPubComp;
      dtCore::RefPtr<GameActorProxy> mGameActorProxy;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(DefaultNetworkPublishingComponentTests);


   //////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponentTests::setUp()
   {
      BaseClass::setUp();
      try
      {
         mNetPubComp = new DefaultNetworkPublishingComponent;

         mGM->AddComponent(*mNetPubComp, GameManager::ComponentPriority::NORMAL);

         mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, mGameActorProxy);

         //Publish the actor.
         mGM->AddActor(*mGameActorProxy, false, false);
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
      mNetPubComp  = NULL;
      BaseClass::tearDown();
   }

   //////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponentTests::TestPublishActor()
   {
      mGM->PublishActor(*mGameActorProxy);
      dtCore::System::GetInstance().Step(0.016f);
      dtCore::System::GetInstance().Step(0.016f);

      CPPUNIT_ASSERT_EQUAL(1U, unsigned(mTestComp->GetReceivedDispatchNetworkMessages().size()));
      const ActorUpdateMessage* createMessage = dynamic_cast<const ActorUpdateMessage*>(mTestComp->GetReceivedDispatchNetworkMessages()[0].get());
      CPPUNIT_ASSERT(createMessage != NULL);
      CPPUNIT_ASSERT(createMessage->GetMessageType() == MessageType::INFO_ACTOR_CREATED);
      CPPUNIT_ASSERT_EQUAL(createMessage->GetAboutActorId(), mGameActorProxy->GetId());
   }

   //////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponentTests::TestAdditionalTypesToPublishAddRemove()
   {
      CPPUNIT_ASSERT(!mNetPubComp->PublishesAdditionalMessageType(MessageType::INFO_ACTOR_UPDATED));
      CPPUNIT_ASSERT(!mNetPubComp->PublishesAdditionalMessageType(MessageType::INFO_GAME_EVENT));
      mNetPubComp->AddMessageTypeToPublish(MessageType::INFO_GAME_EVENT);
      CPPUNIT_ASSERT(mNetPubComp->PublishesAdditionalMessageType(MessageType::INFO_GAME_EVENT));
      mNetPubComp->RemoveMessageTypeToPublish(MessageType::INFO_GAME_EVENT);
      CPPUNIT_ASSERT(!mNetPubComp->PublishesAdditionalMessageType(MessageType::INFO_GAME_EVENT));
   }

   //////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponentTests::TestAdditionalTypesToPublishFunction()
   {
      mNetPubComp->AddMessageTypeToPublish(MessageType::INFO_GAME_EVENT);
      dtCore::RefPtr<GameEventMessage> gaMsg;
      mGM->GetMessageFactory().CreateMessage(MessageType::INFO_GAME_EVENT, gaMsg);
      mGM->SendMessage(*gaMsg);
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL(1U, unsigned(mTestComp->GetReceivedDispatchNetworkMessages().size()));
      CPPUNIT_ASSERT(mTestComp->FindDispatchNetworkMessageOfType(MessageType::INFO_GAME_EVENT).valid());

      mTestComp->reset();

      dtCore::RefPtr<MachineInfo> mi = new MachineInfo("some other info");
      gaMsg->SetSource(*mi);
      mGM->SendMessage(*gaMsg);
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_MESSAGE("The message should not have been published because it had the wrong machine info",
               mTestComp->GetReceivedDispatchNetworkMessages().empty());

      mTestComp->reset();

      gaMsg->SetSource(mGM->GetMachineInfo());
      mNetPubComp->RemoveMessageTypeToPublish(MessageType::INFO_GAME_EVENT);
      mGM->SendMessage(*gaMsg);
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_MESSAGE("The message should not have been published because the type was removed from the list",
               mTestComp->GetReceivedDispatchNetworkMessages().empty());
   }

   //////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponentTests::TestDeleteActor()
   {
      mGM->PublishActor(*mGameActorProxy);
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      mTestComp->reset();

      mGM->DeleteActor(*mGameActorProxy);
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
      mGM->PublishActor(*mGameActorProxy);
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

      mGM->DeleteActor(*mGameActorProxy);
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL(0U, unsigned(mTestComp->GetReceivedDispatchNetworkMessages().size()));
   }
}
