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
#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/basemodeldata.h>
#include <dtGame/messagetype.h>
#include <dtAnim/modeldatabase.h>
#include <dtActors/engineactorregistry.h>
#include <dtActors/gamemeshactor.h>
#include <dtUtil/mathdefines.h>

#include <cal3d/model.h>

const dtCore::RefPtr<dtCore::SystemComponentType> ProceduralAnimationComponent::TYPE(new dtCore::SystemComponentType("ProceduralAnimationComponent","GMComponents",
      "Test component that demos procedural animation.",
      dtGame::BaseInputComponent::DEFAULT_TYPE));

////////////////////////////////////////////////////////////////////////////////

ProceduralAnimationComponent::ProceduralAnimationComponent()
   : dtGame::BaseInputComponent(*TYPE)
{}

////////////////////////////////////////////////////////////////////////////////
ProceduralAnimationComponent::~ProceduralAnimationComponent()
{}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationComponent::ProcessMessage(const dtGame::Message& message)
{
   if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
   {
      const dtGame::TickMessage& tickMessage
         = static_cast<const dtGame::TickMessage&>(message);

      if ( ! mActorList.empty())
      {
         typedef std::vector<ProceduralAnimationActor*> AnimActorList;
         AnimActorList::iterator curIter = mActorList.begin();
         AnimActorList::iterator endIter = mActorList.end();
         for ( ; curIter != endIter; ++curIter)
         {
            (*curIter)->GetModelWrapper()->UpdateAnimation(tickMessage.GetDeltaSimTime());
         }
      }
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      CreateIKActorsForAesthetics();

      // IK Actors need access to their pose databases so set them here
      InitializeIKActors();

      // Set the behavior for each of the actors
      for (size_t actorIndex = 0; actorIndex < mActorList.size(); ++actorIndex)
      {
         ProceduralAnimationActor::eMode mode = ProceduralAnimationActor::MODE_WATCH;
            //(actorIndex % 2) ? ProceduralAnimationActor::MODE_WATCH: ProceduralAnimationActor::MODE_AIM;

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
         dtCore::RefPtr<ProceduralAnimationActorProxy> actor;
         gameManager.CreateActor(*TestActorLibraryRegistry::IK_ACTOR_TYPE, actor);
         if (actor.valid())
         {
            gameManager.AddActor(*actor, false, false);

            actor->GetComponent<dtAnim::AnimationHelper>()->SetSkeletalMesh(dtCore::ResourceDescriptor("SkeletalMeshes:PoseMeshMarine:Eye_Marine_with_posemesh.xml"));

            actor->SetTranslation(currentPosition);

            ProceduralAnimationActor* drawable;
            actor->GetDrawable(drawable);
            mActorList.push_back(drawable);
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

   // For lack of gravity, snap actors to the nearest point below or above.
   gcData.SetGroundClampType(dtGame::GroundClampTypeEnum::FULL);

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

      dtGame::GameActorProxy& actor = mActorList[actorIndex]->GetGameActorProxy();

      // Add this actor to the ground clamp batch
      mGroundClamper->ClampToGround(dtGame::BaseGroundClamper::GroundClampRangeType::RANGED,
         0.0, transform, actor, gcData, true);
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
      // TODO:
   }
}

////////////////////////////////////////////////////////////////////////////////
dtCore::Transformable* ProceduralAnimationComponent::GetTerrain()
{
   dtActors::GameMeshActor* meshActor = NULL;
   GetGameManager()->FindActorByType(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, meshActor);

   if (meshActor)
   {
      dtCore::Transformable* terrain = NULL;
      meshActor->GetDrawable(terrain);

      return terrain;
   }
   else
   {
      LOG_WARNING("Unable to find terrain in map.");
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationComponent::SetAimTarget(dtCore::Transformable* transformable)
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
   //dtAnim::ModelDatabase& database = dtAnim::ModelDatabase::GetInstance();

   dtAnim::BaseModelWrapper* wrapper = actor->GetComponent<dtAnim::AnimationHelper>()->GetModelWrapper();

   // See if this core model already has a pose mesh database that can be shared
   PoseMeshMap::iterator mapIter = mPoseMeshMap.find(wrapper);
   if (mapIter != mPoseMeshMap.end())
   {
      return mapIter->second.get();
   }

   // Get access to the pose mesh file name
   dtAnim::BaseModelData* modelData = wrapper->GetModelData();

   if (!modelData->GetPoseMeshFilename().empty())
   {
      // Load up the pose mesh data
      dtAnim::PoseMeshDatabase* newPoseDatabase = new dtAnim::PoseMeshDatabase(wrapper);
      newPoseDatabase->LoadFromFile(modelData->GetPoseMeshFilename());

      mPoseMeshMap.insert(std::make_pair(wrapper, newPoseDatabase));

      return newPoseDatabase;
   }

   LOG_WARNING("IKActor: " + actor->GetName() + " has no pose mesh!");
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
