/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
 * MG
 */

#include "directorcomponent.h"

#include <dtABC/application.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtCore/deltawin.h>
#include <dtCore/transform.h>
#include <dtCore/collisionmotionmodel.h>
#include <dtCore/collisioncategorydefaults.h>
#include <dtActors/engineactorregistry.h>
#include <dtDirector/director.h>

const std::string& DirectorComponent::NAME = "DirectorComponent";

////////////////////////////////////////////////////////////////////////////////

DirectorComponent::DirectorComponent(const std::string& name)
   : dtGame::BaseInputComponent(name)
   , mMotionModel(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
DirectorComponent::~DirectorComponent()
{
}

////////////////////////////////////////////////////////////////////////////////
void DirectorComponent::ProcessMessage(const dtGame::Message& message)
{
   if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      OnMapLoaded();
   }
}

////////////////////////////////////////////////////////////////////////////////
bool DirectorComponent::HandleKeyPressed(const dtCore::Keyboard* keyBoard, int key)
{
   bool handled = false;

   switch (key)
   {
   case osgGA::GUIEventAdapter::KEY_Escape:
      {
         GetGameManager()->GetApplication().Quit();
         return true;
      }
   case '~':
      {
         GetGameManager()->GetApplication().SetNextStatisticsType();
         return true;
      }
   default:
      break;
   };

   if (!handled)
   {
      return GetGameManager()->GetApplication().KeyPressed(keyBoard, key);
   }

   return handled;
}

////////////////////////////////////////////////////////////////////////////////
void DirectorComponent::OnMapLoaded()
{
   // Here we are setting up our camera motion model so it is an FPS style
   // game.  Note that we do nothing specific for Director.  That is because
   // the map itself contains a Director Actor inside it, this actor handles
   // the execution of a Director Script while the map is loaded.

   dtGame::GameManager* gm = GetGameManager();
   dtABC::Application& app = gm->GetApplication();
   dtCore::Camera* camera  = gm->GetApplication().GetCamera();

   // Make sure collision detection is performed on the camera and with the trigger
   camera->SetCollisionSphere(1.0f);
   camera->SetCollisionCollideBits(COLLISION_CATEGORY_MASK_PROXIMITYTRIGGER);
   app.GetScene()->AddChild(camera);

   // Allow the player to walk around the level and collide with objects
   dtCore::CollisionMotionModel* motionModel =
      new dtCore::CollisionMotionModel(1.5f, 0.2f, 0.1f, 0.05f, app.GetScene(), app.GetKeyboard(), app.GetMouse());

   // Prevent the motion model from colliding with the camera
   motionModel->GetFPSCollider().SetCollisionBitsForFeet(COLLISION_CATEGORY_MASK_OBJECT);
   motionModel->GetFPSCollider().SetCollisionBitsForTorso(COLLISION_CATEGORY_MASK_OBJECT);

   motionModel->SetScene(&gm->GetScene());
   motionModel->SetTarget(camera);

   mMotionModel = motionModel;

   app.GetWindow()->ShowCursor(false);

   std::vector<dtDAL::BaseActorObject*> proxyList;
   gm->FindActorsByType(*dtActors::EngineActorRegistry::STATIC_MESH_ACTOR_TYPE, proxyList);

   // Try to eliminate interpenetration overhead (ODE is the bottleneck here)
   for (size_t proxyIndex = 0; proxyIndex < proxyList.size(); ++proxyIndex)
   {
      dtCore::Transformable* actor =
         dynamic_cast<dtCore::Transformable*>(proxyList[proxyIndex]->GetActor());

      actor->SetCollisionCategoryBits(COLLISION_CATEGORY_MASK_OBJECT);
      actor->SetCollisionCollideBits(COLLISION_CATEGORY_MASK_CAMERA);
      //actor->RenderCollisionGeometry(true);
   }

   // Get the player start position
   dtDAL::BaseActorObject* playerStartProxy = NULL;
   gm->FindActorByType(*dtActors::EngineActorRegistry::PLAYER_START_ACTOR_TYPE, playerStartProxy);

   dtCore::Transformable* playerStart =
      dynamic_cast<dtCore::Transformable*>(playerStartProxy->GetActor());

   dtCore::Transform startTransform;
   playerStart->GetTransform(startTransform);
   camera->SetTransform(startTransform);
}

////////////////////////////////////////////////////////////////////////////////
