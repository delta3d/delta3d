/* -*-c++-*-
* testAnim - testanim (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2007-2008, Alion Science and Technology Corporation
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
* Bradley Anderegg
*/

#include "testanim.h"
#include "testaniminput.h"

#include <dtUtil/hotspotdefinition.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/exception.h>

#include <dtCore/globals.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/object.h>
#include <dtCore/shadermanager.h>
#include <dtCore/tripod.h>

#include <dtDAL/map.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/project.h>

#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>
#include <dtGame/logcontroller.h>
#include <dtGame/gameactor.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/exceptionenum.h>

#include <dtAnim/animationcomponent.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/attachmentcontroller.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3danimator.h>
#include <dtAnim/sequencemixer.h>
#include <dtAnim/animatable.h>

#include <dtAnim/animationgameactor.h>
#include <dtAnim/animactorregistry.h>

#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Shape>

////////////////////////////////////////////////////////////////////////////////

extern "C" TEST_ANIM_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
{
   return new TestAnim;
}

extern "C" TEST_ANIM_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint* entryPoint)
{
   delete entryPoint;
}

////////////////////////////////////////////////////////////////////////////////
TestAnim::TestAnim()
   : dtGame::GameEntryPoint()
   , mAnimationHelper(NULL)
   , mFMM(NULL)
   , mPerformanceTest(false)
{

}

////////////////////////////////////////////////////////////////////////////////
TestAnim::~TestAnim()
{

}

////////////////////////////////////////////////////////////////////////////////
void TestAnim::Initialize(dtGame::GameApplication& app, int argc, char **argv)
{
   if (argc > 1)
   {
      std::string arg1(argv[1]);
      if (arg1 == "--performanceTest")
      {
         mPerformanceTest = true;
      }
      else
      {
         std::cout << std::endl<< "Usage: " << argv[0] << " testAnim [--performanceTest]" << std::endl;
         throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR,"Usage error", __FILE__, __LINE__);
      }
   }

   if (mPerformanceTest)
   {
      mFMM = new dtCore::FlyMotionModel(app.GetKeyboard(), app.GetMouse());
      mFMM->SetTarget(app.GetCamera());
   }
}


////////////////////////////////////////////////////////////////////////////////
void TestAnim::OnStartup(dtGame::GameApplication& app)
{
   app.GetWindow()->SetWindowTitle("TestAnim");

   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" +
      dtCore::GetDeltaRootPath() + "/examples/data" + ";");

   std::string context = dtCore::GetDeltaRootPath() + "/examples/data/demoMap";

   typedef std::vector<dtDAL::ActorProxy* > ProxyContainer;
   ProxyContainer proxies;
   ProxyContainer groundActor;

   dtGame::GameManager& gameManager = *app.GetGameManager();

   try
   {
      dtDAL::Project::GetInstance().SetContext(context, true);
      gameManager.ChangeMap("TestAnim");
      dtCore::System::GetInstance().Start();
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();
      gameManager.FindActorsByName("CharacterEntity", proxies);
      gameManager.FindActorsByName("GroundActor", groundActor);
   }
   catch (dtUtil::Exception& e)
   {
      LOG_ERROR("Can't find the project context or load the map. Exception follows.");
      e.LogException(dtUtil::Log::LOG_ERROR);
   }

   //dtDAL::Project::GetInstance().GetResourcePath(dtDAL::ResourceDescriptor(""));
   //dtCore::ShaderManager::GetInstance().LoadShaderDefinitions();

   dtGame::DefaultMessageProcessor* dmp = new dtGame::DefaultMessageProcessor();

   dtAnim::AnimationComponent* animComp = new dtAnim::AnimationComponent();

   gameManager.AddComponent(*dmp,dtGame::GameManager::ComponentPriority::HIGHEST);
   gameManager.AddComponent(*animComp, dtGame::GameManager::ComponentPriority::NORMAL);

   if (!mPerformanceTest)
   {
      TestAnimInput* inputComp = new TestAnimInput("TestAnimInput");
      gameManager.AddComponent(*inputComp, dtGame::GameManager::ComponentPriority::NORMAL);

      ProxyContainer::iterator iter = proxies.begin();
      ProxyContainer::iterator endIter = proxies.end();

      for (;iter != endIter; ++iter)
      {
         dtAnim::AnimationGameActorProxy* gameProxy = dynamic_cast<dtAnim::AnimationGameActorProxy*>(*iter);

         if (gameProxy != NULL)
         {
            static bool first = true;

            //the first one will be the player
            InitializeAnimationActor(gameProxy, animComp, true, app.GetCamera());

            if (first)
            {
               inputComp->SetAnimationHelper(*mAnimationHelper);
               inputComp->SetPlayerActor(*gameProxy);
               first = false;
            }
         }
      }
   }
   else
   {
      osg::Vec3 startPos(0.0f, 0.0f, 10.0f);

      for (int i = 0; i < 10; ++i, startPos[0] += 2.0f)
      {
         for (int j = 0; j < 10; ++j, startPos[1] += 2.0f)
         {
            dtCore::RefPtr<dtAnim::AnimationGameActorProxy> proxy;
            gameManager.CreateActor(*dtAnim::AnimActorRegistry::ANIMATION_ACTOR_TYPE, proxy);
            if (proxy.valid())
            {
               gameManager.AddActor(*proxy);

               dtAnim::AnimationGameActor* actor = dynamic_cast<dtAnim::AnimationGameActor*>(&proxy->GetGameActor());
               actor->SetModel("SkeletalMeshes/marine.xml");
               InitializeAnimationActor(proxy.get(), animComp, false, app.GetCamera());

               proxy->SetTranslation(startPos);
            }
         }
         startPos[1] = 0.0f;
      }

      //      app.GetCamera()->SetNextStatisticsType();

   }

   if (!groundActor.empty())
   {
      dtDAL::ActorProxy* proxy = dynamic_cast<dtDAL::ActorProxy*>(groundActor.front());
      if (proxy)
      {
         dtCore::Transformable* transform = dynamic_cast<dtCore::Transformable*>(proxy->GetActor());
         if (transform)
         {
            animComp->SetTerrainActor(transform);
         }
      }
   }
   else
   {
      LOG_ERROR("Cannot find ground");
   }

   gameManager.DebugStatisticsTurnOn(false, false, 5);

   //   app.GetWindow()->SetKeyRepeat(false);
}

////////////////////////////////////////////////////////////////////////////////
void TestAnim::OnShutdown(dtGame::GameApplication& app)
{
}

////////////////////////////////////////////////////////////////////////////////
void TestAnim::InitializeAnimationActor(dtAnim::AnimationGameActorProxy* gameProxy,
                                        dtAnim::AnimationComponent* animComp,
                                        bool isPlayer, dtCore::Camera *camera)
{
   dtAnim::AnimationGameActor* actor = dynamic_cast<dtAnim::AnimationGameActor*>(&gameProxy->GetGameActor());

   if (actor != NULL)
   {
      dtAnim::AnimationHelper* helper = actor->GetHelper();

      if (helper != NULL)
      {
         //we must register the helper with the animation component
         animComp->RegisterActor(*gameProxy, *helper);


         //since we are doing hardware skinning there is no need for
         //the physique driver
         //TODO: this should be refactored out of here and into the place that decides
         //      whether or not we are doing hardware skinning
         helper->GetAnimator()->SetPhysiqueDriver(NULL);

         if (isPlayer)
         {
            mAnimationHelper = helper;
            mAnimationHelper->SetGroundClamp(true);

            //attach the Camera to the Actor using a Tripod
            dtCore::Tripod *tripod = new dtCore::Tripod(camera, actor);
            tripod->SetTetherMode( dtCore::Tripod::TETHER_WORLD_REL );
            tripod->SetOffset( 0.f, -5.f, 1.25f, 0.f, 0.f, 0.f);


            //attach a pack to the guy's back
            dtCore::RefPtr<dtCore::Object> attachment = new dtCore::Object("CamelPack");
            attachment->LoadFile("/models/camelpack.ive");

            dtUtil::HotSpotDefinition hotspotDef;
            hotspotDef.mName = "backpack";
            hotspotDef.mParentName = "Bip02 Spine2";
            hotspotDef.mLocalTranslation.set(0.25f, -0.125f, 0.0f);

            osg::Matrix attRot = osg::Matrix::rotate(osg::DegreesToRadians(90.f), osg::Vec3(0.f,1.f,0.f));
            attRot *= osg::Matrix::rotate(osg::DegreesToRadians(180.f), osg::Vec3(0.f,0.f,1.f));
            hotspotDef.mLocalRotation = attRot.getRotate();

            mAnimationHelper->GetAttachmentController().AddAttachment(*attachment, hotspotDef);
            actor->AddChild(attachment.get());
         }
         else
         {
            helper->PlayAnimation("Walk");
            helper->GetSequenceMixer().GetActiveAnimation("Walk")->SetStartDelay(dtUtil::RandFloat(20.0f, 40.0f));
            helper->GetSequenceMixer().ForceRecalculate();
         }
      }
   }
}

