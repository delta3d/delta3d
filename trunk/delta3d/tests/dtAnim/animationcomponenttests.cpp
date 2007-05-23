/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Delta3D
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
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>

#include <dtAnim/animationcomponent.h>
#include <dtAnim/animationhelper.h>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>
#include <dtCore/globals.h>
#include <dtCore/timer.h>

#include <dtGame/gamemanager.h>

#include <dtDAL/map.h>
#include <dtDAL/project.h>

#include <dtActors/animationgameactor2.h>
#include <dtActors/engineactorregistry.h>

#include <string>

using namespace dtGame;

namespace dtAnim
{

   class TestAnimHelper: public AnimationHelper
   {
      bool mHasBeenUpdated;

   public:
      TestAnimHelper():mHasBeenUpdated(false){}

      bool HasBeenUpdated(){return mHasBeenUpdated;}

      void Update(float dt)
      {
         mHasBeenUpdated = true;
      }
   };

   class AnimationComponentTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( AnimationComponentTests );
         CPPUNIT_TEST( TestAnimationComponent );
         CPPUNIT_TEST( TestAnimationPerformance );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         void TestAnimationComponent(); 
         void TestAnimationPerformance();

      private:

         dtCore::RefPtr<dtUtil::Log> mLogger;
         dtCore::RefPtr<GameManager> mGM;
         dtCore::RefPtr<AnimationComponent> mAnimComp;
         dtCore::RefPtr<GameActorProxy> mTestGameActor;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( AnimationComponentTests );

   void AnimationComponentTests::setUp()
   {
      mLogger = &dtUtil::Log::GetInstance("animationcomponenttests.cpp");

      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();
      mGM = new GameManager(*new dtCore::Scene());
      mAnimComp = new AnimationComponent();
      mGM->AddComponent(*mAnimComp, GameManager::ComponentPriority::NORMAL);

      mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, mTestGameActor);
      CPPUNIT_ASSERT(mTestGameActor.valid());

   }

   void AnimationComponentTests::tearDown()
   {
      dtCore::System::GetInstance().Stop();
      if(mGM.valid())
      {
         mTestGameActor = NULL;
         mGM->DeleteAllActors(true);
         mGM = NULL;
      }
      mAnimComp = NULL;
   }


   void AnimationComponentTests::TestAnimationComponent()
   {
      dtCore::RefPtr<TestAnimHelper> helper = new TestAnimHelper();

      mAnimComp->RegisterActor(*mTestGameActor, *helper);

      CPPUNIT_ASSERT(mAnimComp->IsRegisteredActor(*mTestGameActor));

      CPPUNIT_ASSERT(mAnimComp->GetHelperForProxy(*mTestGameActor) == helper.get());

      dtCore::System::GetInstance().Step();  

      CPPUNIT_ASSERT(helper->HasBeenUpdated());
   }

   void AnimationComponentTests::TestAnimationPerformance()
   {      
      typedef std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > ProxyContainer;
      ProxyContainer proxies;

      //load map
      try
      {
         std::string context = dtCore::GetDeltaRootPath() + "/tests/data/ProjectContext";
         dtDAL::Project::GetInstance().SetContext(context, true);
         mGM->ChangeMap("AnimationPerformance");
         
         dtDAL::Project::GetInstance().GetMap("AnimationPerformance").FindProxies(proxies, "CharacterEntity");      
      
      }
      catch (dtUtil::Exception &e)
      {
         CPPUNIT_FAIL(e.ToString());
      }

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, "Performance testing AnimationComponent with " + dtUtil::ToString(proxies.size()) + " animated entities.");
      
      //register animation actors
      ProxyContainer::iterator iter, end;
      for(iter = proxies.begin(), end = proxies.end(); iter != end; ++iter)
      {
         dtGame::GameActorProxy* gameProxy = dynamic_cast<dtGame::GameActorProxy*>((*iter).get());
         if(gameProxy)
         {
            dtActors::AnimationGameActor2* actor = dynamic_cast<dtActors::AnimationGameActor2*>(&gameProxy->GetGameActor());

            if(actor)
            { 
               mAnimComp->RegisterActor(*gameProxy, *actor->GetHelper());               
            }
    
         }
      }
   

      //lets do some performance testing
      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, "Testing performance of PlayAnimation on AnimationHelper");

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      dtCore::Timer timer;
      dtCore::Timer_t timerStart, timerEnd;

      timerStart = timer.Tick();
      for(iter = proxies.begin(), end = proxies.end(); iter != end; ++iter)
      {
         dtGame::GameActorProxy* gameProxy = dynamic_cast<dtGame::GameActorProxy*>((*iter).get());
         if(gameProxy)
         {
            dtActors::AnimationGameActor2* actor = dynamic_cast<dtActors::AnimationGameActor2*>(&gameProxy->GetGameActor());

            if(actor)
            { 
               actor->GetHelper()->PlayAnimation("Walk");
            }      
         } 
      }
      timerEnd = timer.Tick();

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, "Time Results for Play Animation: " + dtUtil::ToString(timer.DeltaMil(timerStart, timerEnd)) + " ms");
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      int numUpdates = 60;
      float updateTime = 1.0f / 60.0f;
      dtCore::RefPtr<dtGame::Message> message = mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL);

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, "Testing performance of " + dtUtil::ToString(numUpdates) + " updates on AnimationComponent");

      timerStart = timer.Tick();
      for(int i = 0; i < numUpdates; ++i)
      {
         mAnimComp->ProcessMessage(*message.get());
      }
      timerEnd = timer.Tick();

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, "Time Results for Update: " + dtUtil::ToString(timer.DeltaMil(timerStart, timerEnd)) + " ms");
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      
      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, "Testing performance of ClearAnimation()");

      timerStart = timer.Tick();
      for(iter = proxies.begin(), end = proxies.end(); iter != end; ++iter)
      {
         dtGame::GameActorProxy* gameProxy = dynamic_cast<dtGame::GameActorProxy*>((*iter).get());
         if(gameProxy)
         {
            dtActors::AnimationGameActor2* actor = dynamic_cast<dtActors::AnimationGameActor2*>(&gameProxy->GetGameActor());

            if(actor)
            { 
               actor->GetHelper()->ClearAnimation("Walk", 0.0);
            }      
         }
      }
      timerEnd = timer.Tick();

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, "Time Results for Clear Animation: " + dtUtil::ToString(timer.DeltaMil(timerStart, timerEnd)) + " ms");
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      
      //close map
      try
      {
         if(!mGM->GetCurrentMap().empty())
         {
            dtDAL::Project::GetInstance().CloseMap(dtDAL::Project::GetInstance().GetMap(mGM->GetCurrentMap()), true);
            dtDAL::Project::GetInstance().DeleteMap(dtDAL::Project::GetInstance().GetMap(mGM->GetCurrentMap()));
         }
      }
      catch(dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.ToString());
      }
    
}

}//namespace dtAnim
