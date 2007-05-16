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

#include <dtAnim/animationcomponent.h>
#include <dtAnim/animationhelper.h>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>

#include <dtGame/gamemanager.h>


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
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         void TestAnimationComponent(); 

      private:

         dtCore::RefPtr<GameManager> mGM;
         dtCore::RefPtr<AnimationComponent> mAnimComp;
         dtCore::RefPtr<GameActorProxy> mTestGameActor;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( AnimationComponentTests );

   void AnimationComponentTests::setUp()
   {
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

   
}
