/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * @author David Guthrie
 */
#include <cppunit/extensions/HelperMacros.h>
#include <osg/Vec3>
#include <osg/Math>
#include <dtCore/system.h>
#include <dtGame/gamemanager.h> 
#include <dtGame/exceptionenum.h>
#include <dtGame/deadreckoningcomponent.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

class DeadReckoningComponentTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(DeadReckoningComponentTests);

      CPPUNIT_TEST(TestDeadReckoningHelperDefaults);
      CPPUNIT_TEST(TestDeadReckoningHelperProperties);
      CPPUNIT_TEST(TestTerrainQuery);
      CPPUNIT_TEST(TestActorRegistration);
      CPPUNIT_TEST(TestSimpleBehavior);

   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp()
      {
         dtCore::System::Instance()->SetShutdownOnWindowClose(false);
         dtCore::System::Instance()->Start();
         mGM = new dtGame::GameManager(*new dtCore::Scene());
         mDeadReckoningComponent = new dtGame::DeadReckoningComponent("DeadReckoningComponent");
         mGM->AddComponent(*mDeadReckoningComponent, dtGame::GameManager::ComponentPriority::NORMAL);
         mGM->LoadActorRegistry(mTestGameActorRegistry);
      }
      
      void tearDown()
      {
         dtCore::System::Instance()->Stop();
         mGM->DeleteAllActors(true);
         mGM->UnloadActorRegistry(mTestGameActorRegistry);
         mGM = NULL;
      }

      void TestDeadReckoningHelperDefaults()
      {
         dtCore::RefPtr<dtGame::DeadReckoningHelper> helper = new dtGame::DeadReckoningHelper;
	      CPPUNIT_ASSERT_MESSAGE("Updated flag should default to false", !helper->IsUpdated());
	      CPPUNIT_ASSERT_MESSAGE("DeadReckoning algorithm should default to NONE.", 
	      helper->GetDeadReckoningAlgorithm() == dtGame::DeadReckoningAlgorithm::NONE);
         CPPUNIT_ASSERT_MESSAGE("Flying should default to false", !helper->IsFlying());
         
	      osg::Vec3 vec(0.0f, 0.0f, 0.0f);
	 
         CPPUNIT_ASSERT(helper->GetLastKnownTranslation() == vec);
         CPPUNIT_ASSERT(helper->GetLastKnownRotation() == vec);
         CPPUNIT_ASSERT(helper->GetVelocityVector() == vec);
         CPPUNIT_ASSERT(helper->GetAccelerationVector() == vec);
         CPPUNIT_ASSERT(helper->GetAngularVelocityVector() == vec);
      }

      void TestDeadReckoningHelperProperties()
      {
         dtCore::RefPtr<dtGame::DeadReckoningHelper> helper = new dtGame::DeadReckoningHelper;
      	helper->SetFlying(true);
         CPPUNIT_ASSERT(helper->IsUpdated());
         CPPUNIT_ASSERT(helper->IsFlying());
	 
         helper->ClearUpdated();

         CPPUNIT_ASSERT(!helper->IsUpdated());
 
         helper->SetDeadReckoningAlgorithm(dtGame::DeadReckoningAlgorithm::STATIC);
         CPPUNIT_ASSERT_MESSAGE("DeadReckoning algorithm should be STATIC.",
         helper->GetDeadReckoningAlgorithm() == dtGame::DeadReckoningAlgorithm::STATIC);
         CPPUNIT_ASSERT(helper->IsUpdated());
         helper->ClearUpdated();

         
         osg::Vec3 vec(3.1f, 9900.032f, 493.738f);
	 
         helper->SetLastKnownTranslation(vec);
         CPPUNIT_ASSERT(helper->GetLastKnownTranslation() == vec);
         CPPUNIT_ASSERT(helper->IsUpdated());
         helper->ClearUpdated();

         helper->SetLastKnownRotation(vec);
         CPPUNIT_ASSERT(helper->GetLastKnownRotation() == vec);
         CPPUNIT_ASSERT(helper->IsUpdated());
         helper->ClearUpdated();

         helper->SetVelocityVector(vec);
         CPPUNIT_ASSERT(helper->GetVelocityVector() == vec);
         CPPUNIT_ASSERT(helper->IsUpdated());
         helper->ClearUpdated();

         helper->SetAccelerationVector(vec);
         CPPUNIT_ASSERT(helper->GetAccelerationVector() == vec);
         CPPUNIT_ASSERT(helper->IsUpdated());
         helper->ClearUpdated();

         helper->SetAngularVelocityVector(vec);
         CPPUNIT_ASSERT(helper->GetAngularVelocityVector() == vec);
         CPPUNIT_ASSERT(helper->IsUpdated());
         helper->ClearUpdated();
      }

      void TestTerrainQuery()
      {
         CPPUNIT_ASSERT(mDeadReckoningComponent->GetTerrainActorName() == "Terrain");
         CPPUNIT_ASSERT(mDeadReckoningComponent->GetTerrainActor() == NULL);
         
         dtCore::RefPtr<dtDAL::ActorType> terrainType;
         dtCore::RefPtr<dtDAL::ActorProxy> terrain;
         terrainType = mGM->FindActorType("dtcore.Terrain", "Infinite Terrain");
         CPPUNIT_ASSERT(terrainType.valid());
         terrain = mGM->CreateActor(*terrainType);
         terrain->SetName("Terrain");
         mGM->AddActor(*terrain);
         
         //It should still think there is no terrain because it should cache the value.
         CPPUNIT_ASSERT(mDeadReckoningComponent->GetTerrainActor() == NULL);
         terrain->SetName("Q");
         mDeadReckoningComponent->SetTerrainActorName("Q");
         CPPUNIT_ASSERT(mDeadReckoningComponent->GetTerrainActorName() == "Q");
         CPPUNIT_ASSERT(mDeadReckoningComponent->GetTerrainActor() == terrain.get());
      }

      void TestActorRegistration()
      {
         dtCore::RefPtr<dtDAL::ActorType> type;
         dtCore::RefPtr<dtGame::GameActorProxy> actor;
         type = mGM->FindActorType("ExampleActors", "Test1Actor");
         CPPUNIT_ASSERT(type.valid());
         mGM->CreateActor(*type, actor);
         CPPUNIT_ASSERT(actor.valid());
         dtCore::RefPtr<dtGame::DeadReckoningHelper> helper = new dtGame::DeadReckoningHelper;
         mDeadReckoningComponent->RegisterActor(*actor, *helper);
         CPPUNIT_ASSERT(mDeadReckoningComponent->IsRegisteredActor(*actor));
         mDeadReckoningComponent->UnregisterActor(*actor);
         CPPUNIT_ASSERT(!mDeadReckoningComponent->IsRegisteredActor(*actor));
      }
      
      void TestSimpleBehavior()
      {
         dtCore::RefPtr<dtDAL::ActorType> type;
         dtCore::RefPtr<dtGame::GameActorProxy> actor;
         dtCore::RefPtr<dtGame::DeadReckoningHelper> helper = new dtGame::DeadReckoningHelper;

         type = mGM->FindActorType("ExampleActors", "Test1Actor");
         CPPUNIT_ASSERT(type.valid());
         mGM->CreateActor(*type, actor);
         CPPUNIT_ASSERT(actor.valid());
         
         mGM->CreateActor(*type, actor);
         CPPUNIT_ASSERT(actor.valid());
         mGM->AddActor(*actor, true, false);
         mDeadReckoningComponent->RegisterActor(*actor, *helper);
         CPPUNIT_ASSERT(mDeadReckoningComponent->IsRegisteredActor(*actor));
         
         dtCore::Transform xform;
         actor->GetGameActor().GetTransform(&xform);
         osg::Vec3 vec;
         xform.GetTranslation(vec);
         CPPUNIT_ASSERT(osg::equivalent(vec.x(), 0.0f, 1e-2f) &&
                        osg::equivalent(vec.y(), 0.0f, 1e-2f) &&
                        osg::equivalent(vec.z(), 0.0f, 1e-2f)
                        );
         xform.GetRotation(vec);
         CPPUNIT_ASSERT(osg::equivalent(vec.x(), 0.0f, 1e-2f) &&
                        osg::equivalent(vec.y(), 0.0f, 1e-2f) &&
                        osg::equivalent(vec.z(), 0.0f, 1e-2f)
                        );
                              
         osg::Vec3 setVec = osg::Vec3(1.0, 1.2, 1.3);
         
         helper->SetLastKnownTranslation(setVec);
         helper->SetLastKnownRotation(setVec);
         helper->SetDeadReckoningAlgorithm(dtGame::DeadReckoningAlgorithm::NONE);
         helper->SetFlying(true);
         
         dtCore::System::Instance()->Step();
      
         actor->GetGameActor().GetTransform(&xform);
         xform.GetTranslation(vec);
         CPPUNIT_ASSERT(osg::equivalent(vec.x(), 0.0f, 1e-2f) &&
                        osg::equivalent(vec.y(), 0.0f, 1e-2f) &&
                        osg::equivalent(vec.z(), 0.0f, 1e-2f)
                        );
         xform.GetRotation(vec);
         CPPUNIT_ASSERT(osg::equivalent(vec.x(), 0.0f, 1e-2f) &&
                        osg::equivalent(vec.y(), 0.0f, 1e-2f) &&
                        osg::equivalent(vec.z(), 0.0f, 1e-2f)
                        );
      
         helper->SetDeadReckoningAlgorithm(dtGame::DeadReckoningAlgorithm::STATIC);
         dtCore::System::Instance()->Step();
      
         actor->GetGameActor().GetTransform(&xform);
         xform.GetTranslation(vec);
         CPPUNIT_ASSERT(osg::equivalent(vec.x(), setVec.x(), 1e-2f) &&
                        osg::equivalent(vec.y(), setVec.y(), 1e-2f) &&
                        osg::equivalent(vec.z(), setVec.z(), 1e-2f)
                        );
         xform.GetRotation(vec);
         CPPUNIT_ASSERT(osg::equivalent(vec.x(), setVec.x(), 1e-2f) &&
                        osg::equivalent(vec.y(), setVec.y(), 1e-2f) &&
                        osg::equivalent(vec.z(), setVec.z(), 1e-2f)
                        );
      
         
         mDeadReckoningComponent->UnregisterActor(*actor);
         CPPUNIT_ASSERT(!mDeadReckoningComponent->IsRegisteredActor(*actor));
      }


   private:

      dtCore::RefPtr<dtGame::GameManager> mGM;
      dtCore::RefPtr<dtGame::DeadReckoningComponent> mDeadReckoningComponent;
      static const std::string mTestGameActorRegistry;
};

CPPUNIT_TEST_SUITE_REGISTRATION(DeadReckoningComponentTests);

const std::string DeadReckoningComponentTests::mTestGameActorRegistry("testGameActorLibrary");

