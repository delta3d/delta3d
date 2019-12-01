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

#include <dtUtil/hotspotdefinition.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/exception.h>

#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/object.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/scene.h>
#include <dtCore/shadermanager.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>

#include <dtCore/map.h>
#include <dtCore/actorproxy.h>
#include <dtCore/actorproxy.h>
#include <dtCore/project.h>

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
   , mMotionModel(NULL)
   , mPerformanceTest(false)
{

}

////////////////////////////////////////////////////////////////////////////////
TestAnim::~TestAnim()
{

}

////////////////////////////////////////////////////////////////////////////////
void TestAnim::Initialize(dtABC::BaseABC& app, int argc, char **argv)
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
         throw dtGame::GameApplicationConfigException("Usage error", __FILE__, __LINE__);
      }
   }

   if (mPerformanceTest)
   {
      mMotionModel = new dtCore::FlyMotionModel(app.GetKeyboard(), app.GetMouse());
   }
   else
   {
      mMotionModel = new dtCore::OrbitMotionModel(app.GetKeyboard(), app.GetMouse());
   }
   
   mMotionModel->SetTarget(app.GetCamera());
}

////////////////////////////////////////////////////////////////////////////////
void TestAnim::OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gameManager)
{
   mMessageProcComponent = new dtGame::DefaultMessageProcessor();
   mAnimationComponent = new dtAnim::AnimationComponent();

   gameManager.AddComponent(*mMessageProcComponent,dtGame::GameManager::ComponentPriority::HIGHEST);
   gameManager.AddComponent(*mAnimationComponent, dtGame::GameManager::ComponentPriority::NORMAL);

   mInputComponent = new TestAnimInput();
   gameManager.AddComponent(*mInputComponent, dtGame::GameManager::ComponentPriority::NORMAL);

   app.GetWindow()->SetWindowTitle("TestAnim");

   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/data" + ";");

   std::string context = dtUtil::GetDeltaRootPath() + "/examples/data";

   typedef std::vector<dtCore::BaseActorObject* > ProxyContainer;
   ProxyContainer proxies;
   ProxyContainer groundActor;

   try
   {
      dtCore::Project::GetInstance().SetContext(context, true);
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

   ProxyContainer::iterator iter = proxies.begin();
   ProxyContainer::iterator endIter = proxies.end();

   for (;iter != endIter; ++iter)
   {
      dtAnim::AnimationGameActor* gameProxy = dynamic_cast<dtAnim::AnimationGameActor*>(*iter);

      if (gameProxy != NULL)
      {
         static bool first = true;

         //the first one will be the player
         InitializeAnimationActor(gameProxy, mAnimationComponent, true, app.GetCamera());

         if (first)
         {
            mInputComponent->SetAnimationHelper(*mAnimationHelper);
            mInputComponent->SetPlayerActor(*gameProxy);
            first = false;
         }
      }
   }

   if (mPerformanceTest)
   {
      osg::Vec3 startPos(0.0f, 0.0f, 10.0f);

      for (int i = 0; i < 10; ++i, startPos[0] += 2.0f)
      {
         for (int j = 0; j < 10; ++j, startPos[1] += 2.0f)
         {
            dtCore::RefPtr<dtAnim::AnimationGameActor> actor;
            gameManager.CreateActor(*dtAnim::AnimActorRegistry::ANIMATION_ACTOR_TYPE, actor);
            if (actor.valid())
            {
               gameManager.AddActor(*actor);

               actor->GetComponent<dtAnim::AnimationHelper>()->SetSkeletalMesh(dtCore::ResourceDescriptor("SkeletalMeshes:Marine:marine.xml"));
               InitializeAnimationActor(actor, mAnimationComponent, false, app.GetCamera());

               actor->SetTranslation(startPos);

            }
         }
         startPos[1] = 0.0f;
      }
   }

   if (!groundActor.empty())
   {
      dtCore::BaseActorObject* actor = dynamic_cast<dtCore::BaseActorObject*>(groundActor.front());
      if (actor != NULL)
      {
         dtCore::Transformable* transform = actor->GetDrawable<dtCore::Transformable>();
         if (transform != NULL)
         {
            mAnimationComponent->SetTerrainActor(transform);
         }
      }
   }
   else
   {
      LOG_ERROR("Cannot find ground");
   }
}

////////////////////////////////////////////////////////////////////////////////
void TestAnim::OnShutdown(dtABC::BaseABC& app, dtGame::GameManager& gm)
{
   dtGame::GameEntryPoint::OnShutdown(app, gm);
}

////////////////////////////////////////////////////////////////////////////////
void TestAnim::InitializeAnimationActor(dtAnim::AnimationGameActor* gameProxy,
                                        dtAnim::AnimationComponent* animComp,
                                        bool isPlayer, dtCore::Camera *camera)
{
   dtAnim::AnimationGameActor* actor = dynamic_cast<dtAnim::AnimationGameActor*>(gameProxy);

   if (actor != NULL)
   {
      dtAnim::AnimationHelper* helper = actor->GetComponent<dtAnim::AnimationHelper>();

      if (helper != NULL)
      {
         if (isPlayer)
         {
            mAnimationHelper = helper;
            mAnimationHelper->SetGroundClamp(true);

            dtCore::Transformable* tx;
            actor->GetDrawable(tx);
            dtCore::Transform originalXform;
            tx->GetTransform(originalXform);

            // Temporarily set the character to the origin
            // just to be sure things are attached to their
            // proper locations.
            dtCore::Transform xform;
            osg::Vec3 offset;
            //osg::Vec3 originalPos = originalXform.GetTranslation();
            xform.SetTranslation(offset);

            dtCore::RefPtr<dtCore::Transformable> cameraPivot = new dtCore::Transformable;
            cameraPivot->SetTransform(xform);
            tx->AddChild(cameraPivot);
            offset.set(0.0f, 0.0f, 1.25f);
            xform.SetTranslation(offset);
            cameraPivot->SetTransform(xform, dtCore::Transformable::REL_CS);
            
            cameraPivot->AddChild(camera);
            offset.set(0.0f, -5.0f, 0.0f);
            xform.SetTranslation(offset);
            camera->SetTransform(xform, dtCore::Transformable::REL_CS);

            dtCore::OrbitMotionModel* omm
               = dynamic_cast<dtCore::OrbitMotionModel*>(mMotionModel.get());
            if (omm != NULL)
            {
               omm->SetDistance(0.0f);
            }
            mMotionModel->SetTarget(cameraPivot.get());

            mAnimationHelper->ModelLoadedSignal.connect_slot(this, &TestAnim::PlayerLoadCallback);
         }
         else
         {
            helper->ModelLoadedSignal.connect_slot(this, &TestAnim::NonPlayerLoadCallback);
         }
      }
   }
}

void TestAnim::PlayerLoadCallback(dtAnim::AnimationHelper* helper)
{
   //attach a pack to the guy's back
   dtCore::RefPtr<dtCore::Object> attachment = new dtCore::Object("CamelPack");
   attachment->LoadFile("/StaticMeshes/camelpack.ive");

   dtUtil::HotSpotDefinition hotspotDef;
   hotspotDef.mName = "backpack";
   hotspotDef.mParentName = "Bip02 Spine2";
   hotspotDef.mLocalTranslation.set(0.25f, -0.125f, 0.0f);

   osg::Matrix attRot = osg::Matrix::rotate(osg::DegreesToRadians(90.f), osg::Vec3(0.f,1.f,0.f));
   attRot *= osg::Matrix::rotate(osg::DegreesToRadians(180.f), osg::Vec3(0.f,0.f,1.f));
   hotspotDef.mLocalRotation = attRot.getRotate();

   helper->GetAttachmentController()->AddAttachment(*attachment, hotspotDef);

   dtGame::GameActorProxy* actor;
   helper->GetOwner(actor);

   dtCore::Transformable* tx;
   actor->GetDrawable(tx);

   tx->AddChild(attachment.get());
}

void TestAnim::NonPlayerLoadCallback(dtAnim::AnimationHelper* helper)
{
   helper->PlayAnimation("Walk");
   helper->GetSequenceMixer().GetActiveAnimation("Walk")->SetStartDelay(dtUtil::RandFloat(20.0f, 40.0f));
   helper->GetSequenceMixer().ForceRecalculate();
}
