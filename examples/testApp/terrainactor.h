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
 */

#ifndef DELTA_TEST_APP_TERRAIN_ACTOR
#define DELTA_TEST_APP_TERRAIN_ACTOR

#include "export.h"
#include <dtGame/gameactor.h>

#include <dtPhysics/physicstypes.h>
#include <dtPhysics/physicsobject.h>
#include <dtPhysics/physicscomponent.h>
#include <dtPhysics/physicsactcomp.h>

#include <dtUtil/threadpool.h>

// Sadly, this includes a lot, but it's needed for the options on the LoadNodeTask
#include <osgDB/ReaderWriter>



namespace dtGame
{
   class TimerElapsedMessage;
}



namespace dtExample
{

   class TEST_APP_EXPORT LoadNodeTask : public dtUtil::ThreadPoolTask
   {
   public:
      LoadNodeTask();

      virtual void operator()();

      osg::Node* GetLoadedNode();
      const osg::Node* GetLoadedNode() const;

      /// Check to see if the loading is complete.  If it returns true, call WaitUntilComplete() to make sure.
      bool IsComplete() const;

      virtual void ResetData();

      DT_DECLARE_ACCESSOR(bool, UseFileCaching);
      DT_DECLARE_ACCESSOR(std::string, FileToLoad);
      DT_DECLARE_ACCESSOR(dtCore::RefPtr<osgDB::ReaderWriter::Options>, LoadOptions);

   protected:
      virtual ~LoadNodeTask();
   private:
      dtCore::RefPtr<osg::Node> mLoadedNode;
      volatile bool mComplete;
   };



   class TEST_APP_EXPORT TerrainDrawable : public dtGame::GameActor
   {
   public:
      typedef dtGame::GameActor BaseClass;

      static const std::string DEFAULT_NAME;
      /// Constructor
      TerrainDrawable(dtGame::GameActorProxy& parent);

      /**
       * Loads a mesh file which contains terrain.
       * @param fileName The file of the terrain mesh to load.
       * @note Although terrain meshes are the same "type" of file as static meshes
       *  and other geometry, mesh terrains have a special resource of type
       *  DataType::TERRAIN.
       */
      void LoadFile(const std::string& fileName);

      /// Turn on or off texture, and geometry caching when loading the terrain.
      void SetLoadTerrainMeshWithCaching(bool enable);
      /// @return true if texture, and geometry caching when loading the terrain is on.
      bool GetLoadTerrainMeshWithCaching();

      virtual void AddedToScene(dtCore::Scene* scene);
      virtual void RemovedFromScene(dtCore::Scene* scene);

      /*virtual*/ void OnEnteredWorld();

      dtPhysics::PhysicsActComp& GetHelper() { return *mHelper; }
      const dtPhysics::PhysicsActComp& GetHelper() const { return *mHelper; }

      /**
       * Override this to do something after the terrain loads.
       * To override, Call the base class and check the base return
       * value to see if you should do any post load behavior.
       * The caller uses the return value, so be sure to propagate it.
       * Keep in mind that the loading process could be complete, but
       * the terrain node will be null because the loading failed.
       * @return true if the terrain loading completed.
       */
      virtual bool CheckForTerrainLoaded();

      /// This is called if CheckForTerrainLoaded returns true.
      virtual void SetupTerrainPhysics();

   protected:

      /// Destructor
      virtual ~TerrainDrawable();

   private:

      void LoadMeshFromFile(const std::string& filename, const std::string& materialType);

      dtCore::RefPtr<dtPhysics::PhysicsActComp> mHelper;

      dtCore::RefPtr<osg::Node> mTerrainNode;

      dtCore::RefPtr<LoadNodeTask> mLoadNodeTask;

      std::string mLoadedFile;
      //This doesn't load the file unless it's in a scene, so this flag tells it to load
      bool mNeedToLoad;

      bool mLoadTerrainMeshWithCaching;
   };



   class TEST_APP_EXPORT TerrainActor : public dtGame::GameActorProxy
   {
   public:
      typedef dtGame::GameActorProxy BaseClass;

      static const int COLLISION_GROUP_TERRAIN = 0;

      /**
       * Constructor
       */
      TerrainActor();

      virtual void OnRemovedFromWorld();

      /**
       * Adds the properties to the actor.
       */
      virtual void BuildPropertyMap();

      /// overridden to initialize the physics.
      virtual void BuildActorComponents();

      /// overridden to setup the timer invokable.
      virtual void BuildInvokables();

      /// Creates the actor we are encapsulating
      virtual void CreateDrawable() { SetDrawable(*new TerrainDrawable(*this)); }

      /**
       * Gets the billboard used to represent static meshes if this proxy's
       * render mode is RenderMode::DRAW_BILLBOARD_ICON.
       * @return a pointer to the icon
       */
      virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

      /**
       * Gets the current render mode for positional lights.
       * @return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.
       */
      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
      }

      void HandleNodeLoaded(const dtGame::TimerElapsedMessage& timerElapsed);

   protected:

      /**
       * Destructor
       */
      virtual ~TerrainActor() { }

   private:
   };
}

#endif
