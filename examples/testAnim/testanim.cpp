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

#include <dtCore/globals.h>
#include <dtCore/collisionmotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtDAL/project.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>
#include <dtGame/logcontroller.h> 
#include <dtDAL/actorproxy.h>
#include <dtGame/gameactor.h>
#include <dtActors/animationgameactor2.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtActors/animationgameactor2.h>
#include <dtAnim/animationcomponent.h>
#include <dtAnim/animationhelper.h>
#include <dtDAL/map.h>
#include <dtCore/scene.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/map.h>


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
  app.GetWindow()->SetWindowTitle("TestAnim");
}

//////////////////////////////////////////////////////////////////////////
dtCore::ObserverPtr<dtGame::GameManager> TestAnim::CreateGameManager(dtCore::Scene& scene)
{ 
   return dtGame::GameEntryPoint::CreateGameManager(scene);
}

//////////////////////////////////////////////////////////////////////////
void TestAnim::OnStartup()
{
   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" + 
                               dtCore::GetDeltaRootPath() + "/examples/data" + ";");

   std::string context = dtCore::GetDeltaRootPath() + "/examples/data/demoMap";

   typedef std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > ProxyContainer;
   ProxyContainer proxies;
   ProxyContainer groundActor;

   dtGame::GameManager &gameManager = *GetGameManager();

   try
   {
      dtDAL::Project::GetInstance().SetContext(context, true);
      gameManager.ChangeMap("TestAnim");
      dtDAL::Project::GetInstance().GetMap("TestAnim").FindProxies(proxies, "CharacterEntity");      
      dtDAL::Project::GetInstance().GetMap("TestAnim").FindProxies(groundActor, "GroundActor");      
   }
   catch (dtUtil::Exception &e)
   {
      LOG_ERROR("Can't find the project context: " + e.What());
   }

   
   dtCore::DeltaWin *win = gameManager.GetApplication().GetWindow();

   dtGame::DefaultMessageProcessor *dmp = new dtGame::DefaultMessageProcessor("DefaultMessageProcessor");
   TestAnimInput* inputComp = new TestAnimInput("TestInputComponent"); 
   
   dtAnim::AnimationComponent *animComp = new dtAnim::AnimationComponent();

   gameManager.AddComponent(*dmp,dtGame::GameManager::ComponentPriority::HIGHEST);
   gameManager.AddComponent(*inputComp, dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*animComp, dtGame::GameManager::ComponentPriority::NORMAL);

     //register helper 
   if(!proxies.empty())
   {
      ProxyContainer::iterator iter = proxies.begin();
      dtGame::GameActorProxy* gameProxy = dynamic_cast<dtGame::GameActorProxy*>((*iter).get());
      if(gameProxy)
      {
         dtActors::AnimationGameActor2* actor = dynamic_cast<dtActors::AnimationGameActor2*>(&gameProxy->GetGameActor());

         if(actor)
         { 
            mAnimationHelper = actor->GetHelper();
            mAnimationHelper->SetGroundClamp(true);
            animComp->RegisterActor(*gameProxy, *mAnimationHelper);            

            //set camera offset
            dtCore::Transform trans;
            trans.SetTranslation(-1.0f, 5.5f, 1.5f);
            trans.SetRotation(180.0f, -2.0f, 0.0f); 
            gameManager.GetApplication().GetCamera()->SetTransform(trans);

            actor->AddChild(gameManager.GetApplication().GetCamera());

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
      ProxyContainer::iterator iter = groundActor.begin();
      dtDAL::ActorProxy* proxy = dynamic_cast<dtDAL::ActorProxy*>((*iter).get());
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

}

void TestAnim::OnShutdown()
{    
   const std::string mapName = GetGameManager()->GetCurrentMap();

   dtDAL::Map& map = dtDAL::Project::GetInstance().GetMap(mapName);
   dtDAL::Project::GetInstance().CloseMap(map, true);
}

