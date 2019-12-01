/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

#include "terrainactor.h"
#include <dtCore/enginepropertytypes.h>
#include <dtCore/actorproxyicon.h>
#include <dtCore/project.h>

#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include <dtCore/shadergroup.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/shadermanager.h>
#include <dtCore/scene.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/functor.h>
#include <dtUtil/nodemask.h>
#include <dtCore/transform.h>

#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>
#include <dtGame/invokable.h>

#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/StateSet>

#include <osgDB/ReadFile>
#include <osgDB/Registry>

#include <osgUtil/GLObjectsVisitor>

#include <iostream>

#include <dtPhysics/physicsreaderwriter.h>
#include <dtPhysics/geometry.h>

namespace dtExample
{
   ///////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ///////////////////////////////////////////////////////////////////////////////
   static const std::string LOAD_NODE_TERRAIN_TIMER;
   // time between checks for loaded terrain.
   static const float LOAD_NODE_TIMER_TIMEOUT = 0.1;



   ///////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ///////////////////////////////////////////////////////////////////////////////
   LoadNodeTask::LoadNodeTask()
      : mUseFileCaching(true)
      , mComplete(false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   LoadNodeTask::~LoadNodeTask()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LoadNodeTask::operator()()
   {
      if (!mFileToLoad.empty())
      {
         dtCore::RefPtr<osgDB::ReaderWriter::Options> options;
         if (mLoadOptions.valid())
         {
            options = mLoadOptions;
         }
         else
         {
            options = new osgDB::ReaderWriter::Options;
         }

         if (mUseFileCaching)
         {
            options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);
         }
         else
         {
            options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_NONE);
         }

         options->setOptionString("loadMaterialsToStateSet");

         mLoadedNode = NULL;

         try
         {
            mLoadedNode = osgDB::readNodeFile(mFileToLoad, options.get());
            mComplete = true;
         }
         catch(...)
         {
            LOG_ERROR("Exception thrown trying to load terrain:" + mFileToLoad);
            mComplete = true;
         }
      }
      else
      {
         mComplete = true;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Node* LoadNodeTask::GetLoadedNode()
   {
      return mLoadedNode;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const osg::Node* LoadNodeTask::GetLoadedNode() const
   {
      return mLoadedNode;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool LoadNodeTask::IsComplete() const
   {
      return mComplete;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LoadNodeTask::ResetData()
   {
      mLoadedNode = NULL;
      mComplete = false;
   }

   DT_IMPLEMENT_ACCESSOR(LoadNodeTask, bool, UseFileCaching);
   DT_IMPLEMENT_ACCESSOR(LoadNodeTask, std::string, FileToLoad);
   DT_IMPLEMENT_ACCESSOR(LoadNodeTask, dtCore::RefPtr<osgDB::ReaderWriter::Options>, LoadOptions);



   ///////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ///////////////////////////////////////////////////////////////////////////////
   const std::string TerrainDrawable::DEFAULT_NAME = "Terrain";

   ///////////////////////////////////////////////////////////////////////////////
   TerrainActor::TerrainActor()
   {
      SetClassName("dtExample::TerrainActor");
   }

   /////////////////////////////////////////////////////////////////////////////
   void TerrainActor::OnRemovedFromWorld()
   {
      dtGame::GameActorProxy::OnRemovedFromWorld();
   }


   ///////////////////////////////////////////////////////////////////////////////
   void TerrainActor::BuildPropertyMap()
   {
      dtGame::GameActorProxy::BuildPropertyMap();
      TerrainDrawable* ta = NULL;
      GetDrawable(ta);

      static const dtUtil::RefString GROUP_("Terrain");

      static const dtUtil::RefString PROPERTY_LOAD_TERRAIN_MESH_WITH_CACHING("LoadTerrainMeshWithCaching");
      static const dtUtil::RefString PROPERTY_TERRAIN_MESH("TerrainMesh");

      static const dtUtil::RefString PROPERTY_TERRAIN_PHYSICS_MODE_DESC("The mode for the physics, DISABLED disables all physics, DEFERRED"
               " uses the cull visitor to load in the background for paged terrains, and IMMEDIATE loads the physics immediately based on the"
               " properties on this actor.");

      AddProperty(new dtCore::ResourceActorProperty(*this, dtCore::DataType::TERRAIN,
         PROPERTY_TERRAIN_MESH,
         PROPERTY_TERRAIN_MESH,
         dtCore::ResourceActorProperty::SetFuncType(ta, &TerrainDrawable::LoadFile),
         "Loads in a terrain mesh for this object", GROUP_));

      AddProperty(new dtCore::BooleanActorProperty(
         PROPERTY_LOAD_TERRAIN_MESH_WITH_CACHING,
         PROPERTY_LOAD_TERRAIN_MESH_WITH_CACHING,
         dtCore::BooleanActorProperty::SetFuncType(ta, &TerrainDrawable::SetLoadTerrainMeshWithCaching),
         dtCore::BooleanActorProperty::GetFuncType(ta, &TerrainDrawable::GetLoadTerrainMeshWithCaching),
         "Enables OSG caching when loading the terrain mesh.", GROUP_));

   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon* TerrainActor::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtCore::ActorProxyIcon("billboards/staticmesh.png");
      }

      return mBillBoardIcon.get();
   }

   ///////////////////////////////////////////////////////////////////
   void TerrainActor::BuildActorComponents()
   {
      BaseClass::BuildActorComponents();

      dtCore::RefPtr<dtPhysics::PhysicsActComp> physAC = new dtPhysics::PhysicsActComp();

      dtCore::RefPtr<dtPhysics::PhysicsObject> pobj = dtPhysics::PhysicsObject::CreateNew("Terrain");
      pobj->SetPrimitiveType(dtPhysics::PrimitiveType::TERRAIN_MESH);
      pobj->SetMechanicsType(dtPhysics::MechanicsType::STATIC);
      pobj->SetCollisionGroup(COLLISION_GROUP_TERRAIN);
      physAC->AddPhysicsObject(*pobj);
      physAC->SetAutoCreateOnEnteringWorld(true);

      AddComponent(*physAC);

   }

   ///////////////////////////////////////////////////////////////////
   void TerrainActor::BuildInvokables()
   {
      dtGame::GameActorProxy::BuildInvokables();

      static const dtUtil::RefString LOAD_CHECK_TIMER_INVOKABLE("LOAD_CHECK_TIMER_INVOKABLE");
      AddInvokable(*new dtGame::Invokable(LOAD_CHECK_TIMER_INVOKABLE,
         dtUtil::MakeFunctor(&TerrainActor::HandleNodeLoaded, this)));

      RegisterForMessagesAboutSelf(dtGame::MessageType::INFO_TIMER_ELAPSED, LOAD_CHECK_TIMER_INVOKABLE);
   }

   ///////////////////////////////////////////////////////////////////
   void TerrainActor::HandleNodeLoaded(const dtGame::TimerElapsedMessage& timerElapsed)
   {
      TerrainDrawable* ta = NULL;
      GetDrawable(ta);
      if (ta != NULL)
      {
         if (ta->CheckForTerrainLoaded())
         {
            GetGameManager()->ClearTimer(LOAD_NODE_TERRAIN_TIMER, this);
            ta->GetOSGNode()->setNodeMask(dtUtil::NodeMask::TERRAIN_GEOMETRY | dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY | dtUtil::NodeMask::SHADOW_RECEIVE);

            ta->SetupTerrainPhysics();
         }
      }
   }


   ///////////////////////////////////////////////////////////////
   void TerrainDrawable::RemovedFromScene(dtCore::Scene* scene)
   {
      scene->ResetDatabasePager();
   }


   ///////////////////////////////////////////////////////////////
   TerrainDrawable::TerrainDrawable(dtGame::GameActorProxy& owner)
      : BaseClass(owner)
      , mNeedToLoad(false)
      , mLoadTerrainMeshWithCaching(false)
   {
      SetName(DEFAULT_NAME);
   }

   /////////////////////////////////////////////////////////////////////////////
   TerrainDrawable::~TerrainDrawable()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void TerrainDrawable::OnEnteredWorld()
   {
      dtGame::GameActor::OnEnteredWorld();
      LoadFile(mLoadedFile);
   }

   /////////////////////////////////////////////////////////////////////////////
   void TerrainDrawable::AddedToScene(dtCore::Scene* scene)
   {
      BaseClass::AddedToScene(scene);
      //Actually load the file, even if it's empty string so that if someone were to
      //load a mesh, remove it from the scene, then try to clear the mesh, this actor will still
      //work.
      if (GetGameActorProxy().IsInSTAGE())
      {
         LoadFile(mLoadedFile);
      }
   }


   /////////////////////////////////////////////////////////////////////////////
   void TerrainDrawable::SetupTerrainPhysics()
   {
      GetComponent(mHelper);

      if (!mTerrainNode.valid())
      {
         return;
      }

      // For STAGE
      if (GetGameActorProxy().GetGameManager() == NULL)
      {
         return;
      }


      //Set the helper name to match the actor name.
      mHelper->SetName(GetName());

      /*dtCore::RefPtr<dtGame::Message> terrainMessage;
      GetGameActorProxy().GetGameManager()->GetMessageFactory().CreateMessage(MessageType::INFO_TERRAIN_LOADED, terrainMessage);
      terrainMessage->SetAboutActorId(GetUniqueId());
      GetGameActorProxy().GetGameManager()->SendMessage(*terrainMessage);*/
   }


   /////////////////////////////////////////////////////////////////////////////
   void TerrainDrawable::LoadFile(const std::string& fileName)
   {
      // Don't do anything if the filenames are the same unless we still need to load
      if (!mNeedToLoad && mLoadedFile == fileName)
      {
         return;
      }

      //Don't actually load the file unless we are in the scene.
      if (GetSceneParent() != NULL)
      {
         //We should always clear the geometry.  If LoadFile fails, we should have no geometry.
         if (GetMatrixNode()->getNumChildren() != 0)
         {
            GetMatrixNode()->removeChild(0, GetMatrixNode()->getNumChildren());
         }
         // If the terrain changes, unload the physics.
         dtPhysics::PhysicsActComp* pac = NULL;
         GetComponent(pac);
         if (pac != NULL)
         {
            pac->CleanUp();
         }

         // Empty string is not an error, just has no geometry.
         if (!fileName.empty())
         {
            if (mLoadNodeTask.valid())
            {
               // If you try to change the terrain while it's loading
               // then tough, we just have to block until it's done.
               mLoadNodeTask->WaitUntilComplete();
               mLoadNodeTask->ResetData();
            }
            else
            {
               mLoadNodeTask = new LoadNodeTask();
            }

            mLoadNodeTask->SetUseFileCaching(mLoadTerrainMeshWithCaching);
            mLoadNodeTask->SetFileToLoad(fileName);

            dtCore::RefPtr<osgDB::ReaderWriter::Options> options;
            if (!options.valid())
            {
               options = new osgDB::ReaderWriter::Options;
               mLoadNodeTask->SetLoadOptions(options);
            }

            options->setOptionString("loadMaterialsToStateSet");

            // Can't load in the background if we aren't in the GM.
            if (!GetGameActorProxy().IsInGM())
            {
               dtUtil::ThreadPool::AddTask(*mLoadNodeTask, dtUtil::ThreadPool::IMMEDIATE);
               dtUtil::ThreadPool::ExecuteTasks();
               CheckForTerrainLoaded();
            }
            else
            {
               dtUtil::ThreadPool::AddTask(*mLoadNodeTask, dtUtil::ThreadPool::IO);
               // This timer is repeating, so it must be cleared when it's over.
               GetGameActorProxy().GetGameManager()->SetTimer(LOAD_NODE_TERRAIN_TIMER, &GetGameActorProxy(), LOAD_NODE_TIMER_TIMEOUT, true, true);
            }
         }

         // go ahead and start this because even if the loading fails later
         // it still tried, and we don't want any code thinking it hasn't attempted to load yet.
         mNeedToLoad = false;

      }
      else
      {
         mNeedToLoad = true;
      }
      mLoadedFile = fileName;
   }

   ///////////////////////////////////////////////////////////////////
   bool TerrainDrawable::CheckForTerrainLoaded()
   {
      if (!mLoadNodeTask.valid())
      {
         // It's done already or nothing is happening.
         return mTerrainNode.valid();
      }

      if (!mLoadNodeTask->IsComplete())
      {
         return false;
      }

      // It is "complete" so wait to make sure the task clears the thread pool.
      mLoadNodeTask->WaitUntilComplete();

      if (mLoadNodeTask->GetLoadedNode() != NULL)
      {
         mTerrainNode = mLoadNodeTask->GetLoadedNode();

         //osg::StateSet* ss = mTerrainNode->getOrCreateStateSet();
//         ss->setRenderBinDetails(RenderingSupportComponent::RENDER_BIN_TERRAIN, "TerrainBin");

         // Run a visitor to switch to VBO's instead of DrawArrays (the OSG default)
         // Turning this on had a catastrophic impact on performance. OFF is better for now.
         //osgUtil::GLObjectsVisitor nodeVisitor(osgUtil::GLObjectsVisitor::SWITCH_ON_VERTEX_BUFFER_OBJECTS);
         //mTerrainNode->accept(nodeVisitor);
         GetMatrixNode()->addChild(mTerrainNode.get());

         if (!GetShaderGroup().empty())
         {
            // TODO - Figure out why we have to do this next hack.
            // Hack Explanation - if the actor was a prototype, then setting the shader doesn't seem
            // to take effect. So, we set it again. However, we have to set it back to empty string
            // or the SetShaderGroup won't do anything.
            std::string shaderToSet = GetShaderGroup();
            SetShaderGroup(""); // clear the shader so that it will accept the new setting
            SetShaderGroup(shaderToSet);
         }

         mLoadNodeTask = NULL;
      }

      return true;

   }

   ///////////////////////////////////////////////////////////////////
   void TerrainDrawable::SetLoadTerrainMeshWithCaching(bool enable)
   {
      mLoadTerrainMeshWithCaching = enable;
   }

   ///////////////////////////////////////////////////////////////////
   bool TerrainDrawable::GetLoadTerrainMeshWithCaching()
   {
      return mLoadTerrainMeshWithCaching;
   }

}
