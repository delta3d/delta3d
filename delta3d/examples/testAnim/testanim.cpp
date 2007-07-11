/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
#include "testanim.h"
#include "testaniminput.h"

#include <dtUtil/hotspotdefinition.h>

#include <dtCore/globals.h>
#include <dtCore/collisionmotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>

#include <dtDAL/map.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/project.h>

#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>
#include <dtGame/logcontroller.h> 
#include <dtGame/gameactor.h>
#include <dtGame/defaultmessageprocessor.h>

#include <dtAnim/animationcomponent.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/attachmentcontroller.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3danimator.h>

#include <dtActors/animationgameactor2.h>

#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Shape>


extern "C" TEST_ANIM_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
{
   return new TestAnim;
}

//////////////////////////////////////////////////////////////////////////
extern "C" TEST_ANIM_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint* entryPoint)
{
   delete entryPoint;
}

//////////////////////////////////////////////////////////////////////////
TestAnim::TestAnim()
{
   
}

//////////////////////////////////////////////////////////////////////////
TestAnim::~TestAnim()
{

}

//////////////////////////////////////////////////////////////////////////
void TestAnim::Initialize(dtGame::GameApplication& app, int argc, char **argv)
{
}

//////////////////////////////////////////////////////////////////////////
dtCore::ObserverPtr<dtGame::GameManager> TestAnim::CreateGameManager(dtCore::Scene& scene)
{ 
   return dtGame::GameEntryPoint::CreateGameManager(scene);
}

//////////////////////////////////////////////////////////////////////////
void TestAnim::OnStartup()
{
   GetGameManager()->GetApplication().GetWindow()->SetWindowTitle("TestAnim");

   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" + 
                               dtCore::GetDeltaRootPath() + "/examples/data" + ";");

   std::string context = dtCore::GetDeltaRootPath() + "/examples/data/demoMap";

   typedef std::vector<dtDAL::ActorProxy* > ProxyContainer;
   ProxyContainer proxies;
   ProxyContainer groundActor;

   dtGame::GameManager& gameManager = *GetGameManager();

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

   dtGame::DefaultMessageProcessor *dmp = new dtGame::DefaultMessageProcessor();
   TestAnimInput* inputComp = new TestAnimInput("TestAnimInput"); 
   
   dtAnim::AnimationComponent* animComp = new dtAnim::AnimationComponent();

   gameManager.AddComponent(*dmp,dtGame::GameManager::ComponentPriority::HIGHEST);
   gameManager.AddComponent(*inputComp, dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*animComp, dtGame::GameManager::ComponentPriority::NORMAL);

   dtCore::RefPtr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(), 0.02f, 0.3));
   dtCore::RefPtr<osg::Geode> geode = new osg::Geode;
   geode->addDrawable(drawable.get());
   
   dtCore::RefPtr<dtCore::Transformable> attachment = new dtCore::Transformable;
   attachment->GetOSGNode()->asGroup()->addChild(geode.get());
   
   dtUtil::HotSpotDefinition hotspotDef;
   hotspotDef.mName = "jojo";
   hotspotDef.mParentName = "Bip02 Head";
   hotspotDef.mLocalTranslation = osg::Vec3(0.1f, 0.0f, 0.0f);
   
   //register helper 
   if(!proxies.empty())
   {
      dtGame::GameActorProxy* gameProxy = dynamic_cast<dtGame::GameActorProxy*>(proxies.front());

      if(gameProxy != NULL)
      {
         dtActors::AnimationGameActor2* actor = dynamic_cast<dtActors::AnimationGameActor2*>(&gameProxy->GetGameActor());

         if(actor != NULL)
         {
            mAnimationHelper = actor->GetHelper();
            mAnimationHelper->SetGroundClamp(true);
            
//            std::vector<std::string> bones;
//            mAnimationHelper->GetModelWrapper()->GetCoreBoneNames(bones);
//            if (bones.size() > 0)
//            {
//               hotspotDef.mParentName = bones.front();
//            }
            
            mAnimationHelper->GetAttachmentController().AddAttachment(*attachment, hotspotDef);
            actor->AddChild(attachment.get());
            

            //since we are doing hardware skinning there is no need for 
            //the physique driver
            //TODO: this should be refactored out of here and into the place that decides 
            //      whether or not we are doing hardware skinning
            mAnimationHelper->GetAnimator()->SetPhysiqueDriver(NULL);

            //we must register the helper with the animation component
            animComp->RegisterActor(*gameProxy, *mAnimationHelper);   

            //set camera offset
            dtCore::Transform trans;
            trans.SetTranslation(-1.0f, 5.5f, 1.5f);
            trans.SetRotation(180.0f, -2.0f, 0.0f); 

            actor->AddChild(gameManager.GetApplication().GetCamera());
            gameManager.GetApplication().GetCamera()->SetTransform(trans, dtCore::Transformable::REL_CS);

            inputComp->SetAnimationHelper(*mAnimationHelper);
            inputComp->SetPlayerActor(*gameProxy);
         }
      }
   }
   else
   {
      LOG_ERROR("Cannot find character.");
   }

   if(!groundActor.empty())
   {
      dtDAL::ActorProxy* proxy = dynamic_cast<dtDAL::ActorProxy*>(groundActor.front());
      if(proxy)
      {
         dtCore::Transformable* transform = dynamic_cast<dtCore::Transformable*>(proxy->GetActor());
         if(transform)
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

   gameManager.GetApplication().GetWindow()->SetKeyRepeat(false);
}

