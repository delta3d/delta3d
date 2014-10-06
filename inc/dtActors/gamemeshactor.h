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
 * Curtiss Murphy
 */

#ifndef DELTA_GAMEMESHACTOR_H
#define DELTA_GAMEMESHACTOR_H

#include <dtGame/gameactor.h>
#include <dtGame/gameactorproxy.h>
#include <dtCore/plugin_export.h>
#include <dtCore/loadable.h>
#include <dtCore/model.h>
#include <dtUtil/getsetmacros.h>

//#include <map>
//#include <vector>

namespace dtCore
{
   class Scene;
}

namespace dtActors
{
   class GameMeshActor;

   /**
    * This class is the game actor for a basic game object that has a single static mesh.
    * Use this base class if you are using the Game Manager and want to create a
    * simple game object that has one mesh.  It is basically a GameActor with the
    * code from StaticMeshActor in dtActors.
    * @see GameActor
    * @see GameMeshProxy
    * @see StaticMeshActor
    */
   class DT_PLUGIN_EXPORT GameMeshDrawable : public dtGame::GameActor
   {
   public:
      /**
       * Constructs a default task actor.
       * @param proxy The actor proxy owning this task actor.
       * @param desc An optional description of this task actor.
       */
      GameMeshDrawable(dtGame::GameActorProxy& parent);

      /**
       * Loads a mesh file.
       * @param fileName The filename of the mesh to load.
       */
      virtual void SetMesh(const std::string& meshFile);

      /**
       * Called when the actor has been added to the game manager.
       */
      virtual void AddedToScene(dtCore::Scene* scene);

      /**
       * Sets whether we will use the cache when we load files.  This must be set
       * before loading the mesh or it will have no effect.  To make this work, the
       * model is now loaded on entering the world the first time.
       * @param value new use cache value (default is true)
       */
      void SetUseCache(bool value) { mUseCache = value; }

      /**
       * Gets whether we will use the cache when we load files.  This must be set
       * before loading the mesh or it will have no effect.  To make this work, the
       * model is now loaded on entering the world the first time.
       * @return the use cache value (default is true).
       */
      bool GetUseCache() const { return mUseCache; }

      /**
       * Sets the scale on this object
       * @param xyz The scale vector
       */
      void SetScale(const osg::Vec3& xyz);

      /**
       * Returns the scale vector
       * @return The scale
       */
      osg::Vec3 GetScale() const;

      /**
       * Returns the matrix transform of this object
       * @return The osg::MatrixTransform
       */
      osg::MatrixTransform& GetMatrixTransform();

      /**
       * Returns the matrix transform of this object
       * @return The osg::MatrixTransform
       */
      const osg::MatrixTransform& GetMatrixTransform() const;

      /**
       * Returns the osg node for the mesh of this object
       * @return The osg::Node
       */
      osg::Node* GetMeshNode();

      /**
       * Returns the osg node for the mesh of this object
       * @return The osg::Node
       */
      const osg::Node* GetMeshNode() const;

      /**
       * Sets the models rotation
       * @param v3 the hpr
       */
      void SetModelRotation(const osg::Vec3& v3);

      /**
       * Returns the scale vector
       * @return The scale
       */
      osg::Vec3 GetModelRotation();

      /**
       * Sets the models translation
       * @param v3 the xyz
       */
      void SetModelTranslation(const osg::Vec3& v3);

      /**
       * Returns the scale vector
       * @return The scale
       */
      osg::Vec3 GetModelTranslation();

   protected:

      /**
       * Destroys this actor.
       */
      virtual ~GameMeshDrawable();

      /**
       * Inner class for implementing the loadable interface
       */
      class GameMeshLoader : public dtCore::Loadable
      {
         public:
            GameMeshLoader() {};
            ~GameMeshLoader() {};

            /**
             * Sets the mesh file name on the super class.  Provided since the
             * base class doesn't let you set this without using load.
             */
            void SetMeshFilename(const std::string& newFilename)
            {
               mFilename = newFilename;
            }
      };

   private:
      // Does the work of loading the mesh.
      void LoadMesh();

      GameMeshLoader mLoader;
      bool mUseCache;

      dtCore::RefPtr<dtCore::Model> mModel;
      dtCore::RefPtr<osg::Node> mMeshNode;
   };

   /**
    * This class is the proxy for a basic game object that has a single static mesh.
    * Use this base class if you are using the Game Manager and want to create a
    * simple game object that has one mesh.  It is basically a GameActorProxy with the
    * code from StaticMeshActorProxy in dtActors. Note, this class does NOT cache
    * geometry in memory. The reasoning is that since this is a dynamic GameActor
    * that is intended to respond to message and have behavior, changes to its
    * geometry (damaged states, etc.) should happen to each instances. If you
    * want to share geometry for a static actor, check out StaticMeshActorProxy.
    * @see GameActorProxy
    * @see GameMeshActor
    * @see StaticMeshActorProxy
    */
   class DT_PLUGIN_EXPORT GameMeshActor : public dtGame::GameActorProxy
   {
   public:

      /**
       * Constructs the proxy.
       */
      GameMeshActor();

      /**
       * Builds the property map for the task actor proxy.  These properties
       * wrap the specified properties located in the actor.
       */
      virtual void BuildPropertyMap();

      /**
       * Registers any invokables used by the proxy.  The invokables
       * allow the actor to hook into the game manager messages system.
       */
      virtual void BuildInvokables();

      /**
       * Gets the method by which this static mesh is rendered. This is used by STAGE.
       * @return If there is no geometry currently assigned, this
       *  method will return RenderMode::DRAW_BILLBOARD_ICON.  If
       *  there is geometry assigned to this static mesh, RenderMode::DRAW_ACTOR
       *  is returned.
       */
      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode();

      /**
       * Gets the billboard used to represent static mesh if this proxy's
       * render mode is RenderMode::DRAW_BILLBOARD_ICON. Used by STAGE.
       * @return
       */
      virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, MeshResource);

   protected:

      /**
       * Destroys the proxy.
       */
      virtual ~GameMeshActor();

      /**
       * Called by the game manager during creation of the proxy.  This method
       * creates the real actor and returns it.
       */
      virtual void CreateDrawable();

   private:
   };

} // namespace dtActors

#endif // DELTA_GAMEMESHACTOR_H
