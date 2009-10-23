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
 * Michael Guerrero
 */

#include "proceduralanimationcomponent.h"
#include "proceduralanimationactor.h"
#include "testactorlibraryregistry.h"

#include <dtABC/application.h>
#include <dtGame/basemessages.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtActors/staticmeshactorproxy.h>
#include <dtActors/engineactorregistry.h>
#include <dtUtil/mathdefines.h>

const std::string &ProceduralAnimationComponent::NAME = "ProceduralAnimationComponent";

////////////////////////////////////////////////////////////////////////////////

ProceduralAnimationComponent::ProceduralAnimationComponent(const std::string &name)
   : dtGame::BaseInputComponent(name)
{
   // nada
}

////////////////////////////////////////////////////////////////////////////////
ProceduralAnimationComponent::~ProceduralAnimationComponent()
{

}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationComponent::ProcessMessage(const dtGame::Message &message)
{
   if(message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      CreateIKActorsForAesthetics();

      // IK Actors need access to their pose databases so set them here
      InitializeIKActors(); 

      // Set the behavior for each of the actors
      for (size_t actorIndex = 0; actorIndex < mActorList.size(); ++actorIndex)
      {
         ProceduralAnimationActor::eMode mode = 
            (actorIndex % 2) ? ProceduralAnimationActor::MODE_WATCH: ProceduralAnimationActor::MODE_AIM;
         
         mActorList[actorIndex]->SetMode(mode);
         mActorList[actorIndex]->SetTarget(GetGameManager()->GetApplication().GetCamera());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationComponent::CreateIKActorGrid(const osg::Vec3& startPos, 
                                                     const osg::Vec3& forwardDirection, 
                                                     const osg::Vec3& sideDirection, 
                                                     int forwardCount, int sideCount,
                                                     bool perturb)
{
   dtGame::GameManager& gameManager = *GetGameManager();

   osg::Vec3 currentPosition = startPos;

   // Dynamically create a grid of actors
   for (int i = 0; i < forwardCount; ++i)
   {
      currentPosition = startPos + sideDirection * i;

      for (int j = 0; j < sideCount; ++j)
      {
         dtCore::RefPtr<ProceduralAnimationActorProxy> proxy;
         gameManager.CreateActor(*TestActorLibraryRegistry::IK_ACTOR_TYPE, proxy);
         if (proxy.valid())
         {
            gameManager.AddActor(*proxy, false, false);

            ProceduralAnimationActor* actor = dynamic_cast<ProceduralAnimationActor*>(&proxy->GetGameActor());
            actor->SetModel("SkeletalMeshes/PoseMeshMarine/Eye_Marine_with_posemesh.xml");

            proxy->SetTranslation(currentPosition);

            mActorList.push_back(actor);
         }

         currentPosition += forwardDirection;

         if (perturb)
         {
            float factor = dtUtil::RandFloat(-0.5f, 0.5f);
            currentPosition += osg::Vec3(factor, factor, factor);
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationComponent::CreateIKActorsForPerfTest()
{
   const int HORIZONTAL_ELEMENTS = 15;
   const int VERTICAL_ELEMENTS = 15;
   const float ELEMENT_SPACE = 2.0f;

   const float START_X = -(HORIZONTAL_ELEMENTS / 2) * ELEMENT_SPACE + ELEMENT_SPACE * 0.5f;
   const float START_Z = -(VERTICAL_ELEMENTS / 2) * ELEMENT_SPACE;

   osg::Vec3 startPos(START_X, osg::square(HORIZONTAL_ELEMENTS) / 2.5f, START_Z);

   CreateIKActorGrid(startPos, osg::Z_AXIS * ELEMENT_SPACE, osg::X_AXIS * ELEMENT_SPACE,
      VERTICAL_ELEMENTS, HORIZONTAL_ELEMENTS, false);
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationComponent::CreateIKActorsForAesthetics()
{
   // Ground clamp dynamically created actors
   mGroundClamper = new dtGame::DefaultGroundClamper;
   mGroundClamper->SetTerrainActor(GetTerrain());

   dtGame::GroundClampingData gcData;
   gcData.SetAdjustRotationToGround(false);
   gcData.SetUseModelDimensions(false);

   const int HORIZONTAL_ELEMENTS = 5;
   const int VERTICAL_ELEMENTS = 5;
   const float ELEMENT_SPACE = 4.0f;

   const float START_X = -(HORIZONTAL_ELEMENTS / 2) * ELEMENT_SPACE + ELEMENT_SPACE * 0.5f;
   const float START_Y = -(VERTICAL_ELEMENTS / 2) * ELEMENT_SPACE;

   osg::Vec3 startPos(START_X, START_Y, 0.0f);

   CreateIKActorGrid(startPos, osg::Y_AXIS * ELEMENT_SPACE, osg::X_AXIS * ELEMENT_SPACE,
      VERTICAL_ELEMENTS, HORIZONTAL_ELEMENTS); 

   for (size_t actorIndex = 0; actorIndex < mActorList.size(); ++actorIndex)
   {
      dtCore::Transform transform;
      mActorList[actorIndex]->GetTransform(transform, dtCore::Transformable::REL_CS);

      dtGame::GameActorProxy& proxy = mActorList[actorIndex]->GetGameActorProxy();

      // Add this actor to the ground clamp batch
      mGroundClamper->ClampToGround(dtGame::BaseGroundClamper::GroundClampingType::RANGED,
         0.0, transform, proxy, gcData, true);
   }

   // Run the batch ground clamp
   mGroundClamper->FinishUp();
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationComponent::InitializeIKActors()
{
   // Give all IK actors access to the database
   for (size_t actorIndex = 0; actorIndex < mActorList.size(); ++actorIndex)
   {      
      dtAnim::PoseMeshDatabase* posemeshDatabase = GetPoseMeshDatabaseForActor(mActorList[actorIndex]);

      // IKActors typically have a pose mesh with which to do their IK
      if (posemeshDatabase)
      {
         mActorList[actorIndex]->SetPoseMeshDatabase(posemeshDatabase);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
dtCore::Transformable* ProceduralAnimationComponent::GetTerrain()
{
   dtActors::StaticMeshActorProxy* terrainProxy = NULL;
   GetGameManager()->FindActorByType(*dtActors::EngineActorRegistry::STATIC_MESH_ACTOR_TYPE, terrainProxy);

   if (terrainProxy)
   {
      dtCore::Transformable* terrain = NULL;
      terrainProxy->GetActor(terrain);

      return terrain;
   }
   else
   {
      LOG_WARNING("Unable to find terrain in map.");
   }
 
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationComponent::SetAimTarget(const dtCore::Transformable* transformable)
{
   // Give all IK actors something to aim at
   for (size_t actorIndex = 0; actorIndex < mActorList.size(); ++actorIndex)
   {  
      mActorList[actorIndex]->SetMode(ProceduralAnimationActor::MODE_AIM);
      mActorList[actorIndex]->SetTarget(transformable);
   }
}

//////////////////////////////////////////////////////////////////////////
bool ProceduralAnimationComponent::HandleKeyPressed(const dtCore::Keyboard* keyBoard, int key)
{
   bool handled = true;

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
         return false;
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
void ProceduralAnimationComponent::InitializePerformanceTest()
{
   // Dynamically create a grid of actors
   CreateIKActorsForPerfTest();

   // IK Actors need access to their pose databases so set them here
   InitializeIKActors();

   SetAimTarget(GetGameManager()->GetApplication().GetCamera());
}

////////////////////////////////////////////////////////////////////////////////
/// Each core model needs to work with its own pose mesh database.
/// This is not necessary for this example since they all share the same mesh but 
/// is important in a real application that has more than one core model
dtAnim::PoseMeshDatabase* ProceduralAnimationComponent::GetPoseMeshDatabaseForActor(ProceduralAnimationActor* actor)
{
   dtAnim::Cal3DDatabase& calDatabase = dtAnim::Cal3DDatabase::GetInstance();

   dtAnim::Cal3DModelWrapper* wrapper = actor->GetHelper()->GetModelWrapper();
   CalCoreModel* coreModel = wrapper->GetCalModel()->getCoreModel();

   // See if this core model already has a pose mesh database that can be shared
   std::map<CalCoreModel*, IKDatabase>::iterator mapIter;
   mapIter = mPoseMeshMap.find(coreModel);

   if (mapIter != mPoseMeshMap.end())
   {
      return mapIter->second.get();
   }

   // Get access to the pose mesh file name
   dtAnim::Cal3DModelData* modelData = calDatabase.GetModelData(*wrapper);

   if (!modelData->GetPoseMeshFilename().empty())
   {
      // Load up the pose mesh data
      dtAnim::PoseMeshDatabase* newPoseDatabase = new dtAnim::PoseMeshDatabase(wrapper);
      newPoseDatabase->LoadFromFile(modelData->GetPoseMeshFilename());

      mPoseMeshMap.insert(std::make_pair(coreModel, newPoseDatabase));

      return newPoseDatabase;
   }

   LOG_WARNING("IKActor: " + actor->GetName() + " has no pose mesh!");
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
