/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
 */

#ifndef DELTA_SCENE
#define DELTA_SCENE

// scene.h: interface for the Scene class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/deltadrawable.h>
#include <dtUtil/deprecationmgr.h>

#include <osg/Vec3>

#include <osg/GL>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Group;
}

namespace osgParticle
{
   class ParticleSystem;
}
/// @endcond

namespace dtUtil
{
   class ConfigProperties;
}

namespace dtCore
{
   class Transformable;
   class DatabasePager;
   class DeltaDrawable;
   class Light;
   class View;

   class SceneImpl;

   /**
    *  Scene: This class encapsulates the root of the delta scene graph
    */
   class DT_CORE_EXPORT Scene : public DeltaDrawable
   {
      DECLARE_MANAGEMENT_LAYER(Scene)

   public:

      enum Mode
      {
         POINT = GL_POINT,
         LINE = GL_LINE,
         FILL = GL_FILL
      };

      enum Face
      {
         FRONT,
         BACK,
         FRONT_AND_BACK
      };

      /**
       * Default constructor.  Will create and use the default physics controller.
       * @param name : Optional string used to name this particular Scene
       */
      Scene(const std::string& name = "scene");

   protected:

      virtual ~Scene();

   public:
      ///Get a pointer to the internal scene node
      virtual osg::Node* GetOSGNode();

      ///Get a pointer to the internal scene node
      virtual const osg::Node* GetOSGNode() const;

      ///Get a pointer to the internal scene node
      osg::Group* GetSceneNode();
      /**
       *  This function will remove all children of the current scene node,
       *  and add them to this new node, and then set the current scene node to
       *  be this one.  It then notifies the camera group to change all scene nodes applicable.
       *  And finally must remove all drawables and re-add then in case they are holding onto the
       *  scene root.
       */
      void SetSceneNode(osg::Group* newSceneNode);

      ///Add a DeltaDrawable as a child of the Scene Node
      virtual bool AddChild(DeltaDrawable* child);

      /**
       * This has been deprecated: use the AddChild method instead.
       * Add a DeltaDrawable to the Scene to be viewed.
       */
      DEPRECATE_FUNC void AddDrawable(DeltaDrawable* drawable);

      ///Remove a DeltaDrawable from the Scene
      virtual void RemoveChild(DeltaDrawable* child);

      /**
       * Deprecated: use the RemoveChild method instead
       * Remove a DeltaDrawable from the Scene
       */
      DEPRECATE_FUNC void RemoveDrawable(DeltaDrawable* drawable);

      ///clears the scene.
      void RemoveAllDrawables();

      /** Deprecated. Use GetChild instead.
       * Get a handle to the DeltaDrawable using the supplied index number.
       * @param i the index of the DeltaDrawable that was added with AddDrawable()
       * @return The selected DeltaDrawable.  Could be NULL.
       */
      DEPRECATE_FUNC DeltaDrawable* GetDrawable(unsigned int i) const;

      /** Deprecated.  Use GetChildIndex instead.
       * Get the index number of the supplied drawable
       */
      DEPRECATE_FUNC unsigned int GetDrawableIndex(const DeltaDrawable* drawable) const;

      /** Get the number of DeltaDrawables which have been directly added to the Scene
       * using AddChild().
       * @return The number of DeltaDrawables that have been directly added to the Scene
       */
      unsigned int GetNumberOfAddedDrawable() const;

      /**
       * Get all the DeltaDrawables that are currently in the scene.  This will
       * return all the DeltaDrawables added by AddChild(), plus their DeltaDrawable
       * children.
       * @see GetDrawable(), GetNumberOfAddedDrawable()
       * @return a container of all the DeltaDrawables, including their children,
       * that are in this Scene.
       */
      std::vector<dtCore::DeltaDrawable*> GetAllDrawablesInTheScene() const;

      //Set the State for rendering.  Wireframe, Fill polygons, or vertex points.
      void SetRenderState(Face face, Mode mode);
      ///Get a pointer to the internal scene node
      const std::pair<Face,Mode> GetRenderState() const;

      /**
       * Get the height of terrain at the specified (X,Y) below given height range
       * [minZ, maxZ] (default [10k, -10k]). This essentially does an intersection
       * check of the whole scene from (X,Y,maxZ) to (X,Y,minZ). Any geometry that
       * intersects first is considered the "terrain".
       *
       * @param heightOfTerrain : The found height of terrain, if found
       * @param x : The X location to check for height of terrain
       * @param y : The Y location to check for height of terrain
       * @param maxZ : The Z location to check below for height of terrain
       * @param minZ : The Z location to check above for height of terrain
       *
       * @return bool : true if and only if a height was found
       */
      bool GetHeightOfTerrain(float& heightOfTerrain, float x, float y, float maxZ = 10000.0f, float minZ = -10000.0f);

      ///Performs collision detection and updates physics
      virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

      ///The user data associated with "collision" messages
      struct DT_CORE_EXPORT CollisionData
      {
         Transformable* mBodies[2]; ///<The bodies colliding
         osg::Vec3 mLocation; ///<The collision location
         osg::Vec3 mNormal; ///<The collision normal
         float mDepth; ///<The penetration depth
      };


      /// Returns a pointer to the light specified by the param number
      Light* GetLight(const int number);
      const Light* GetLight(const int number) const;
      /// Returns a pointer to the light specified by the current string
      Light* GetLight(const std::string& name);
      const Light* GetLight(const std::string& name) const;

      /// Registers a light using the light number
      void RegisterLight(Light* light);

      /// Unreferences the current light, by number, Note: does not erase
      void UnRegisterLight(Light* light);

      /// Use the internal scene light
      void UseSceneLight(bool lightState = true);

      /**
       * This function clears out the database pager and then re-registers the remaining LOD's.
       *  A good time to do this is on map load.
       */
      void ResetDatabasePager();

   protected:

      friend class View;

      /// Exists for unit tests.
      bool IsAssignedToView(dtCore::View& view) const;

      /// define the owner mView of this instance
      void RemoveView(dtCore::View& view);
      void AddView(dtCore::View& view);

      void SetDatabasePager(dtCore::DatabasePager *pager);
      /// Made public so I can be unit tested better.
      dtCore::DatabasePager* GetDatabasePager() const;

   private:

      // Disallowed to prevent compile errors on VS2003. It apparently
      // creates this functions even if they are not used, and if
      // this class is forward declared, these implicit functions will
      // cause compiler errors for missing calls to "ref".
      Scene& operator=(const Scene&);
      Scene(const Scene&);

      void Ctor();

      void GetDrawableChildren(std::vector<dtCore::DeltaDrawable*>& children,
                               dtCore::DeltaDrawable& parent) const;

      void UpdateViewSet();

      SceneImpl* mImpl;
   };
}


#endif // DELTA_SCENE
