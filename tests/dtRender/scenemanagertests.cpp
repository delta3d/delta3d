/*
   * Delta3D Open Source Game and Simulation Engine
   * Copyright (C) 2014, Caper Holdings, LLC
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
   * Bradley Anderegg
   */
#include <prefix/unittestprefix.h>

#include <cppunit/extensions/HelperMacros.h>
#include <dtRender/scenemanager.h>
#include <dtRender/simplescene.h>
#include <dtRender/guiscene.h>
#include <dtRender/shadowscene.h>
#include <dtRender/scenegroup.h>

#include <dtABC/application.h>
#include <dtRender/dtrenderactorregistry.h>

#include <dtCore/infinitelight.h>

#include <dtGame/basemessages.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/gamemanager.h>

#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/system.h>

#include <cctype>
#include <cmath>

extern dtABC::Application& GetGlobalApplication();


namespace dtRender
{
   
   class SceneManagerTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(SceneManagerTests);
      CPPUNIT_TEST(TestAddRemoveScenes);
      CPPUNIT_TEST(TestSceneOrder);
      CPPUNIT_TEST(TestPushPopScene);
      CPPUNIT_TEST(TestFindScene);
      CPPUNIT_TEST_SUITE_END();

   public:
      SceneManagerTests() {}
      ~SceneManagerTests() {}

      void setUp();
      void tearDown();

      void TestAddRemoveScenes();

      void TestSceneOrder();
      void TestFindScene();
      void TestPushPopScene();

   private:

      void CheckSceneOrder(SceneEnum& se);
      SimpleScene* CreateSimpleScene(SceneEnum& se, RenderOrder ro);


      dtCore::RefPtr<dtUtil::Log> mLogger;

      dtCore::RefPtr<dtGame::GameManager> mGameManager;
      dtCore::RefPtr<dtRender::SceneManager> mSceneManager;      
      dtCore::RefPtr<dtGame::IEnvGameActorProxy> mParentProxy;      
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(SceneManagerTests);

   /////////////////////////////////////////////////////////
   void SceneManagerTests::setUp()
   {
      mLogger = &dtUtil::Log::GetInstance("SceneManagerTests.cpp");

      try
      {
         mGameManager = new dtGame::GameManager(*GetGlobalApplication().GetScene());
         mGameManager->SetApplication(GetGlobalApplication());
         dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
         dtCore::System::GetInstance().Start();

         mGameManager->LoadActorRegistry("dtRender");
         mGameManager->CreateActor(*RenderActorRegistry::SCENE_MANAGER_ACTOR_TYPE, mParentProxy);

         CPPUNIT_ASSERT(mParentProxy.valid());
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.ToString());
      }
   }

   /////////////////////////////////////////////////////////
   void SceneManagerTests::tearDown()
   {
      dtCore::System::GetInstance().SetPause(false);
      dtCore::System::GetInstance().Stop();
    
      mParentProxy = NULL;
    
      if (mGameManager.valid())
      {
         mGameManager->DeleteAllActors();
         mGameManager = NULL;
      }

   }

   /////////////////////////////////////////////////////////
   void SceneManagerTests::TestAddRemoveScenes()
   {
      mGameManager->SetEnvironmentActor(mParentProxy.get());


      dtGame::IEnvGameActorProxy* gap = mGameManager->GetEnvironmentActor();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The SceneManager should be set as the environment actor when it is added to the GameManager", mParentProxy->GetId(), gap->GetId());

      {
         SceneManager* sm = NULL;
         gap->GetDrawable(sm);

         CPPUNIT_ASSERT_MESSAGE("The proxies drawable should be a SceneManager class.", sm != NULL);

         mSceneManager = sm;
      }

      //test scene construction
      const int numScenes = SceneEnum::NUM_SCENES.GetSceneNumber();

      {
         int children = int(mSceneManager->GetNumEnvironmentChildren());
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The SceneManager should have a child for each scene enumeration.", numScenes, children);
      }
      
      for(int i = 0; i < numScenes;++i)
      {
         SceneEnum& curScene = SceneEnum::FindSceneByNumber(i);

         SceneGroup* sg = mSceneManager->GetSceneGroup(curScene);
         
         int numChildren = int(sg->GetNumChildren());
         if(curScene == SceneEnum::DEFAULT_SCENE)
         {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The default scene should have one scene child to add drawables too.", 1, numChildren);
         }
         else
         {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The scene groups should not have any children yet.", 0, numChildren);
         }

      }

      //now create some scenes
      
      //PRE RENDER SCENE
      dtCore::RefPtr<SimpleSceneActor> simpleScenePreRender;
      mGameManager->CreateActor(*RenderActorRegistry::SIMPLE_SCENE_ACTOR_TYPE, simpleScenePreRender);

      CPPUNIT_ASSERT_MESSAGE("The proxies should not be NULL.", simpleScenePreRender.valid());

      SimpleScene* ssPreRender = NULL;
      simpleScenePreRender->GetDrawable(ssPreRender);
      CPPUNIT_ASSERT_MESSAGE("The proxies drawable should be a SimpleScene class.", ssPreRender != NULL);

      ssPreRender->SetSceneEnum(SceneEnum::PRE_RENDER);
      mGameManager->AddActor(*simpleScenePreRender);

      {
         SceneGroup* sg = mSceneManager->GetSceneGroup(SceneEnum::PRE_RENDER);

         int numChildren = int(sg->GetNumChildren());
         
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The Pre Render scene should now have one child.", 1, numChildren);
      }


      //POST RENDER SCENE
      dtCore::RefPtr<SimpleSceneActor> simpleScenePostRender;
      mGameManager->CreateActor(*RenderActorRegistry::SIMPLE_SCENE_ACTOR_TYPE, simpleScenePostRender);

      CPPUNIT_ASSERT_MESSAGE("The proxies should not be NULL.", simpleScenePostRender.valid());
      
      SimpleScene* ssPostRender = NULL;
      simpleScenePostRender->GetDrawable(ssPostRender);
      CPPUNIT_ASSERT_MESSAGE("The proxies drawable should be a SimpleScene class.", ssPostRender != NULL);

      ssPostRender->SetSceneEnum(SceneEnum::POST_RENDER);
      mGameManager->AddActor(*simpleScenePostRender);

      {
         SceneGroup* sg = mSceneManager->GetSceneGroup(SceneEnum::POST_RENDER);

         int numChildren = int(sg->GetNumChildren());

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The Pre Render scene should now have one child.", 1, numChildren);
      }


   }


   /////////////////////////////////////////////////////////
   void SceneManagerTests::TestSceneOrder()
   {
      mGameManager->SetEnvironmentActor(mParentProxy.get());


      dtGame::IEnvGameActorProxy* gap = mGameManager->GetEnvironmentActor();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The SceneManager should be set as the environment actor when it is added to the GameManager", mParentProxy->GetId(), gap->GetId());

      {
         SceneManager* sm = NULL;
         gap->GetDrawable(sm);

         CPPUNIT_ASSERT_MESSAGE("The proxies drawable should be a SceneManager class.", sm != NULL);

         mSceneManager = sm;
      }

      //create a bunch of scenes and make sure they are ordered properly
      CreateSimpleScene(SceneEnum::PRE_RENDER, 5);
      CreateSimpleScene(SceneEnum::PRE_RENDER, 6);
      CreateSimpleScene(SceneEnum::PRE_RENDER, 7);
      CreateSimpleScene(SceneEnum::PRE_RENDER, 3);
      CreateSimpleScene(SceneEnum::PRE_RENDER, 4); 
      CreateSimpleScene(SceneEnum::PRE_RENDER, 2);

      CheckSceneOrder(SceneEnum::PRE_RENDER);

      //make sure it still works with multiple scenes having the same render order
      //we save off a few to remove later
      CreateSimpleScene(SceneEnum::POST_RENDER, 1);
      CreateSimpleScene(SceneEnum::POST_RENDER, 1);
      SimpleScene* ss1 = CreateSimpleScene(SceneEnum::POST_RENDER, 1);
      CreateSimpleScene(SceneEnum::POST_RENDER, 3);
      SimpleScene* ss3 = CreateSimpleScene(SceneEnum::POST_RENDER, 3); 
      CreateSimpleScene(SceneEnum::POST_RENDER, 1);
      CreateSimpleScene(SceneEnum::POST_RENDER, 2);
      SimpleScene* ss2 = CreateSimpleScene(SceneEnum::POST_RENDER, 2);
      CreateSimpleScene(SceneEnum::POST_RENDER, 2);

      CheckSceneOrder(SceneEnum::POST_RENDER);

      CPPUNIT_ASSERT_MESSAGE("SceneManager should have this child", mSceneManager->ContainsActor(*ss1));
      CPPUNIT_ASSERT_MESSAGE("SceneManager should have this child", mSceneManager->ContainsActor(*ss2));
      CPPUNIT_ASSERT_MESSAGE("SceneManager should have this child", mSceneManager->ContainsActor(*ss3));
      
      mSceneManager->RemoveActor(*ss2);
      CheckSceneOrder(SceneEnum::POST_RENDER);

      mSceneManager->RemoveActor(*ss1);
      CheckSceneOrder(SceneEnum::POST_RENDER);

      mSceneManager->RemoveActor(*ss3);
      CheckSceneOrder(SceneEnum::POST_RENDER);


      CPPUNIT_ASSERT_MESSAGE("SceneManager should have removed this child", !mSceneManager->ContainsActor(*ss1));
      CPPUNIT_ASSERT_MESSAGE("SceneManager should have removed this child", !mSceneManager->ContainsActor(*ss2));
      CPPUNIT_ASSERT_MESSAGE("SceneManager should have removed this child", !mSceneManager->ContainsActor(*ss3));


      SimpleScene* ss1_2 = CreateSimpleScene(SceneEnum::POST_RENDER, 1);
      CreateSimpleScene(SceneEnum::POST_RENDER, 3);
      SimpleScene* ss3_2 = CreateSimpleScene(SceneEnum::POST_RENDER, 3); 
      CreateSimpleScene(SceneEnum::POST_RENDER, 1);
      CreateSimpleScene(SceneEnum::POST_RENDER, 2);
      SimpleScene* ss2_2 = CreateSimpleScene(SceneEnum::POST_RENDER, 2);
      CreateSimpleScene(SceneEnum::POST_RENDER, 2);

      CheckSceneOrder(SceneEnum::POST_RENDER);

      CPPUNIT_ASSERT_MESSAGE("SceneManager should have this child", mSceneManager->ContainsActor(*ss1_2));
      CPPUNIT_ASSERT_MESSAGE("SceneManager should have this child", mSceneManager->ContainsActor(*ss2_2));
      CPPUNIT_ASSERT_MESSAGE("SceneManager should have this child", mSceneManager->ContainsActor(*ss3_2));


      mSceneManager->RemoveActor(*ss3_2);
      CheckSceneOrder(SceneEnum::POST_RENDER);

      mSceneManager->RemoveActor(*ss1_2);
      CheckSceneOrder(SceneEnum::POST_RENDER);

      mSceneManager->RemoveActor(*ss2_2);
      CheckSceneOrder(SceneEnum::POST_RENDER);

      CPPUNIT_ASSERT_MESSAGE("SceneManager should have removed this child", !mSceneManager->ContainsActor(*ss1_2));
      CPPUNIT_ASSERT_MESSAGE("SceneManager should have removed this child", !mSceneManager->ContainsActor(*ss2_2));
      CPPUNIT_ASSERT_MESSAGE("SceneManager should have removed this child", !mSceneManager->ContainsActor(*ss3_2));


   }


   /////////////////////////////////////////////////////////
   void SceneManagerTests::TestPushPopScene()
   {
      mGameManager->SetEnvironmentActor(mParentProxy.get());

      dtGame::IEnvGameActorProxy* gap = mGameManager->GetEnvironmentActor();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The SceneManager should be set as the environment actor when it is added to the GameManager", mParentProxy->GetId(), gap->GetId());

      {
         SceneManager* sm = NULL;
         gap->GetDrawable(sm);

         CPPUNIT_ASSERT_MESSAGE("The proxies drawable should be a SceneManager class.", sm != NULL);

         mSceneManager = sm;
      }

      //create a bunch of scenes and make sure they are ordered properly
      SimpleScene* ss5 = CreateSimpleScene(SceneEnum::PRE_RENDER, 5);
      SimpleScene* ss6 = CreateSimpleScene(SceneEnum::PRE_RENDER, 6);
      SimpleScene* ss7 = CreateSimpleScene(SceneEnum::PRE_RENDER, 7);
      SimpleScene* ss3 = CreateSimpleScene(SceneEnum::PRE_RENDER, 3);
      SimpleScene* ss4 = CreateSimpleScene(SceneEnum::PRE_RENDER, 4); 
      SimpleScene* ss2 = CreateSimpleScene(SceneEnum::PRE_RENDER, 2);

      CheckSceneOrder(SceneEnum::PRE_RENDER);

      //now push and pop and remove them, testing the results
      //Light is a simple object to add and remove
            
      dtCore::RefPtr<dtCore::Light> light0 = new dtCore::InfiniteLight(0);
      dtCore::RefPtr<dtCore::Light> light1 = new dtCore::InfiniteLight(1);
      dtCore::RefPtr<dtCore::Light> light2 = new dtCore::InfiniteLight(2);
      dtCore::RefPtr<dtCore::Light> light3 = new dtCore::InfiniteLight(3);
      dtCore::RefPtr<dtCore::Light> light4 = new dtCore::InfiniteLight(4);

      {
         dtCore::RefPtr<SimpleSceneActor> proxy;

         //trying to push a non child scene on the stack should not work
         SceneGroup* sg = mSceneManager->GetSceneGroup(SceneEnum::PRE_RENDER);
         int numchildStart = sg->GetNumChildren();
      
         mGameManager->CreateActor(*RenderActorRegistry::SIMPLE_SCENE_ACTOR_TYPE, proxy);

         CPPUNIT_ASSERT_MESSAGE("The proxy should not be NULL.", proxy.valid());

         SimpleScene* ss = NULL;
         proxy->GetDrawable(ss);
         CPPUNIT_ASSERT_MESSAGE("The proxies drawable should be a SimpleScene class.", ss != NULL);

         ss->SetSceneEnum(SceneEnum::PRE_RENDER);
         ss->SetChildOrder(3);

         //try to push a scene that is not a child of the scene manager, this should fail
         //mSceneManager->PushScene(*ss);

         //CPPUNIT_ASSERT_MESSAGE("The scene stack should still be empty.", mSceneManager->GetCurrentScene() == NULL );
         //CPPUNIT_ASSERT_EQUAL_MESSAGE("The scene should not have added any new children.", unsigned(numchildStart), sg->GetNumChildren());

         //now add the scene check the order, and try to push it on the stack
         mGameManager->AddActor(*proxy);

         mSceneManager->PushScene(*ss);

         CPPUNIT_ASSERT_MESSAGE("The scene stack should not be empty.", mSceneManager->GetCurrentScene() == ss);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The scene should have added 1 new child.", unsigned(numchildStart + 1), sg->GetNumChildren());

         //now try a simple pop
         mSceneManager->PopScene();
         CPPUNIT_ASSERT_MESSAGE("The scene stack should be emptied.", mSceneManager->GetCurrentScene() == NULL);
         
         //check to make sure the scene is still ordered
         CheckSceneOrder(SceneEnum::PRE_RENDER);


         //and now lets add the lights while pushing and popping
         mSceneManager->PushScene(*ss2);
         mSceneManager->PushScene(*ss3);
         mSceneManager->PushScene(*ss4);
         mSceneManager->PushScene(*ss5);
         mSceneManager->PushScene(*ss6);
         mSceneManager->PushScene(*ss7);
         mSceneManager->PushScene(*ss);

         CPPUNIT_ASSERT_MESSAGE("The light should not be in the scene yet.", !mSceneManager->ContainsActor(*light0));

         mSceneManager->AddActor(*light0);;
         CPPUNIT_ASSERT_MESSAGE("Now the light should be in the scene.", mSceneManager->ContainsActor(*light0));

         //now see if ss is the scene with the new light
         CPPUNIT_ASSERT_MESSAGE("The light should be a child of scene ss.", light0->GetParent() == ss);

         //pop, add and check
         mSceneManager->PopScene();

         //scene 7 should be current
         CPPUNIT_ASSERT_MESSAGE("Scene 7 should be the current scene.", mSceneManager->GetCurrentScene() == ss7);

         CPPUNIT_ASSERT_MESSAGE("The light should not be in the scene yet.", !mSceneManager->ContainsActor(*light1));

         mSceneManager->AddActor(*light1);
         CPPUNIT_ASSERT_MESSAGE("Now the light should be in the scene.", mSceneManager->ContainsActor(*light1));

         //now see if ss is the scene with the new light
         CPPUNIT_ASSERT_MESSAGE("The light should be a child of scene ss7.", light1->GetParent() == ss7);

         //push pop do some more adding
         mSceneManager->PushScene(*ss);
         mSceneManager->PopScene();
         mSceneManager->PopScene();
         mSceneManager->PopScene();

         //scene 5 should be current
         CPPUNIT_ASSERT_MESSAGE("Scene 5 should be the current scene.", mSceneManager->GetCurrentScene() == ss5);

         mSceneManager->AddActor(*light2);
         CPPUNIT_ASSERT_MESSAGE("Now the light should be in scene 5.", mSceneManager->ContainsActor(*light2));
         CPPUNIT_ASSERT_MESSAGE("The light should be a child of scene ss7.", light2->GetParent() == ss5);


         //now try removing scenes that have been added to the stack
         mSceneManager->RemoveActor(*ss4);
         mSceneManager->RemoveActor(*ss3);

         //scene 2 should be current
         //CPPUNIT_ASSERT_MESSAGE("Scene 2 should be the current scene.", mSceneManager->GetCurrentScene() == ss2);

      }
   }

   /////////////////////////////////////////////////////////
   void SceneManagerTests::TestFindScene()
   {
      mGameManager->SetEnvironmentActor(mParentProxy.get());

      dtGame::IEnvGameActorProxy* gap = mGameManager->GetEnvironmentActor();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The SceneManager should be set as the environment actor when it is added to the GameManager", mParentProxy->GetId(), gap->GetId());

      {
         SceneManager* sm = NULL;
         gap->GetDrawable(sm);

         CPPUNIT_ASSERT_MESSAGE("The proxies drawable should be a SceneManager class.", sm != NULL);

         mSceneManager = sm;
      }

      //create a bunch of scenes 
      //SimpleScene* ss5 =
            CreateSimpleScene(SceneEnum::PRE_RENDER, 5);
      //SimpleScene* ss6 =
            CreateSimpleScene(SceneEnum::PRE_RENDER, 6);
      SimpleScene* ss7 = CreateSimpleScene(SceneEnum::PRE_RENDER, 7);
      //SimpleScene* ss3 =
            CreateSimpleScene(SceneEnum::PRE_RENDER, 3);
      SimpleScene* ss4 = CreateSimpleScene(SceneEnum::PRE_RENDER, 4); 
      SimpleScene* ss2 = CreateSimpleScene(SceneEnum::PRE_RENDER, 2);

      //SimpleScene* ss1_2 =
            CreateSimpleScene(SceneEnum::NON_TRANSPARENT_OBJECTS, 1);
      SimpleScene* ss3_2 = CreateSimpleScene(SceneEnum::NON_TRANSPARENT_OBJECTS, 3); 
      SimpleScene* ss2_2 = CreateSimpleScene(SceneEnum::NON_TRANSPARENT_OBJECTS, 2);
      
      //SimpleScene* ss1_3 =
            CreateSimpleScene(SceneEnum::FOREGROUND, 1);
      SimpleScene* ss3_3 = CreateSimpleScene(SceneEnum::FOREGROUND, 3); 
      
      SimpleScene* ss1_4 = CreateSimpleScene(SceneEnum::POST_RENDER, 1);
      SimpleScene* ss3_4 = CreateSimpleScene(SceneEnum::POST_RENDER, 3); 
      SimpleScene* ss2_4 = CreateSimpleScene(SceneEnum::POST_RENDER, 2);


      //now push and pop and remove them, testing the results
      //Light is a simple object to add and remove

      dtCore::RefPtr<dtCore::Light> light0 = new dtCore::InfiniteLight(0);
      dtCore::RefPtr<dtCore::Light> light1 = new dtCore::InfiniteLight(1);
      dtCore::RefPtr<dtCore::Light> light2 = new dtCore::InfiniteLight(2);
      dtCore::RefPtr<dtCore::Light> light3 = new dtCore::InfiniteLight(3);
      dtCore::RefPtr<dtCore::Light> light4 = new dtCore::InfiniteLight(4);
      dtCore::RefPtr<dtCore::Light> light5 = new dtCore::InfiniteLight(5);
      dtCore::RefPtr<dtCore::Light> light6 = new dtCore::InfiniteLight(6);
      dtCore::RefPtr<dtCore::Light> light7 = new dtCore::InfiniteLight(7);

      {
         dtCore::RefPtr<SimpleSceneActor> proxy;

         //trying to push a non child scene on the stack should not work
         //SceneGroup* sg = mSceneManager->GetSceneGroup(SceneEnum::PRE_RENDER);
         //int numchildStart = sg->GetNumChildren();

         mGameManager->CreateActor(*RenderActorRegistry::SIMPLE_SCENE_ACTOR_TYPE, proxy);

         CPPUNIT_ASSERT_MESSAGE("The proxy should not be NULL.", proxy.valid());

         CPPUNIT_ASSERT_MESSAGE("The light should not be in the scene yet.", !mSceneManager->ContainsActor(*light0));
         mSceneManager->PushScene(*ss4);

         mSceneManager->AddActor(*light0); // goes to ss4
         CPPUNIT_ASSERT_MESSAGE("Now the light should be in the scene.", mSceneManager->ContainsActor(*light0));

         //now see if ss4 is the scene with the new light
         CPPUNIT_ASSERT_MESSAGE("The light should be a child of scene 4.", light0->GetParent() == ss4);


         CPPUNIT_ASSERT_MESSAGE("The light should not be in the scene yet.", !mSceneManager->ContainsActor(*light1));

         mSceneManager->PushScene(*ss7);
         mSceneManager->AddActor(*light1); //goes to ss7
         CPPUNIT_ASSERT_MESSAGE("Now the light should be in the scene.", mSceneManager->ContainsActor(*light1));

         //now see if ss is the scene with the new light
         CPPUNIT_ASSERT_MESSAGE("The light should be a child of scene 7.", light1->GetParent() == ss7);
         
         //lets pop the stack till its empty
         mSceneManager->PopScene();
         mSceneManager->PopScene();

         CPPUNIT_ASSERT_MESSAGE("The SceneManager stack should be empty", mSceneManager->GetCurrentScene() == NULL);
         
         //push some more scenes and add some more lights
         mSceneManager->PushScene(*ss2_2);
         mSceneManager->PushScene(*ss3_2);
         mSceneManager->PushScene(*ss1_4);
         mSceneManager->PushScene(*ss3_4);
         mSceneManager->PushScene(*ss3_3);
         mSceneManager->PushScene(*ss2_4);

         mSceneManager->AddActor(*light2); //goes to 2_4
         mSceneManager->PopScene();

         mSceneManager->AddActor(*light3); //goes to 3_3
         mSceneManager->PopScene();

         mSceneManager->AddActor(*light4); // goes to 3_4
         mSceneManager->PopScene();

         mSceneManager->AddActor(*light5); //goes to 1_4
         mSceneManager->PopScene();

         mSceneManager->AddActor(*light6); //goes to 3_2
         mSceneManager->PopScene();

         mSceneManager->AddActor(*light7); //goes to 2_2
         
         //now lets do some searching
         {
            SceneBase* scene = mSceneManager->FindSceneByType(*SimpleScene::SIMPLE_SCENE);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("should return the first ordered scene of type", (SceneBase*)ss2, scene);


            const SceneBase* const_scene = mSceneManager->FindSceneByType(*SimpleScene::SIMPLE_SCENE);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("should return the first ordered scene of type", (const SceneBase*) ss2, const_scene);

            std::vector<dtRender::SceneBase*> vec;
            mSceneManager->GetAllScenesByType(*SimpleScene::SIMPLE_SCENE, vec);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("Should have one scene for each child added plus the default scene", std::size_t(15), vec.size());
         }

         {
            SceneBase* sceneForLight7 = mSceneManager->FindSceneForDrawable(*light7);
            SceneBase* sceneForLight6 = mSceneManager->FindSceneForDrawable(*light6);
            SceneBase* sceneForLight5 = mSceneManager->FindSceneForDrawable(*light5);
            SceneBase* sceneForLight4 = mSceneManager->FindSceneForDrawable(*light4);
            SceneBase* sceneForLight3 = mSceneManager->FindSceneForDrawable(*light3);
            SceneBase* sceneForLight2 = mSceneManager->FindSceneForDrawable(*light2);
            SceneBase* sceneForLight1 = mSceneManager->FindSceneForDrawable(*light1);
            SceneBase* sceneForLight0 = mSceneManager->FindSceneForDrawable(*light0);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("drawable should match scene added to", sceneForLight0,(SceneBase*) ss4);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("drawable should match scene added to", sceneForLight1,(SceneBase*) ss7);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("drawable should match scene added to", sceneForLight2,(SceneBase*) ss2_4);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("drawable should match scene added to", sceneForLight3,(SceneBase*) ss3_3);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("drawable should match scene added to", sceneForLight4,(SceneBase*) ss3_4);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("drawable should match scene added to", sceneForLight5,(SceneBase*) ss1_4);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("drawable should match scene added to", sceneForLight6,(SceneBase*) ss3_2);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("drawable should match scene added to", sceneForLight7,(SceneBase*) ss2_2);
      }
   }
}
   /////////////////////////////////////////////////////////
   void SceneManagerTests::CheckSceneOrder(SceneEnum& se)
   {
      //first check the existing number of children

      SceneGroup* sg = mSceneManager->GetSceneGroup(se);
      int numChildren = int(sg->GetNumChildren());
      int lastRenderOrder = -10000;
      
      for (int i = 0; i < numChildren; ++i)
      {
         SceneBase* sb = sg->GetSceneChild(i);

         CPPUNIT_ASSERT(sb != NULL);

         int newRenderOrder = sb->GetChildOrder();

         CPPUNIT_ASSERT_MESSAGE("The scenes should be ordered lowest to highest", lastRenderOrder <= newRenderOrder);
         lastRenderOrder = newRenderOrder;

         int osgIndex = sg->GetSceneNode()->getChildIndex(sb->GetOSGNode());
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The OSG scene children should match the delta drawable children", i, osgIndex);
      }

   }

   /////////////////////////////////////////////////////////
   SimpleScene* SceneManagerTests::CreateSimpleScene(SceneEnum& se, RenderOrder ro)
   {
      dtCore::RefPtr<SimpleSceneActor> proxy;

      //first check the existing number of children
      SceneGroup* sg = mSceneManager->GetSceneGroup(se);
      int numChildrenStart = int(sg->GetNumChildren());
      
      mGameManager->CreateActor(*RenderActorRegistry::SIMPLE_SCENE_ACTOR_TYPE, proxy);

      CPPUNIT_ASSERT_MESSAGE("The proxies should not be NULL.", proxy.valid());

      SimpleScene* ss = NULL;
      proxy->GetDrawable(ss);
      CPPUNIT_ASSERT_MESSAGE("The proxies drawable should be a SimpleScene class.", ss != NULL);

      ss->SetSceneEnum(se);
      ss->SetChildOrder(ro);

      mGameManager->AddActor(*proxy);

      int numChildren = int(sg->GetNumChildren());

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The scene group should have one more child.", numChildrenStart + 1, numChildren);

      return ss;
   }
}
