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

#include <dtCore/base.h>
#include <dtCore/refptr.h>
#include <dtCore/view.h>
#include <dtCore/light.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/databasepager.h>

#include <dtUtil/deprecationmgr.h>

#include <ode/common.h>
#include <ode/collision_space.h>

#include <osg/NodeVisitor>
#include <osg/Vec3>
#include <osg/observer_ptr>


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
   class Physical;
   class Transformable;

   /**
   *  Scene: This class encapsulates the root of the delta scene graph
   */
   class DT_CORE_EXPORT Scene : public Base
   {
      DECLARE_MANAGEMENT_LAYER(Scene)

   public:

      class DT_CORE_EXPORT ParticleSystemFreezer : public osg::NodeVisitor
      {
      public:

         ParticleSystemFreezer();
         void SetFreezing( bool freeze ) { mFreezing = freeze; }
         bool GetFreezing() const { return mFreezing; }

         virtual void apply( osg::Node& node );

      private:

         bool mFreezing;

         typedef std::map< osgParticle::ParticleSystem*, bool > ParticleSystemBoolMap;
         ParticleSystemBoolMap mPreviousFrozenState;
      };

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

      Scene(const std::string& name = "scene");
//      Scene(dtCore::View * view, const std::string& name = "scene", bool useSceneLight = true);
   protected:

      virtual ~Scene();

   public:
      ///Get a pointer to the internal scene node
      osg::Group * GetSceneNode() { return (mSceneNode.get()); }

      /*
      *  This function will remove all children of the current scene node,
      *  and add them to this new node, and then set the current scene node to
      *  be this one.  It then notifies the camera group to change all scene nodes applicable.
      *  And finally must remove all drawables and re-add then in case they are holding onto the
      *  scene root.
      */
      void SetSceneNode(osg::Group* newSceneNode);

      ///Add a DeltaDrawable to the Scene to be viewed.
      void AddDrawable( DeltaDrawable* drawable );

      ///Remove a DeltaDrawable from the Scene
      void RemoveDrawable( DeltaDrawable *drawable );

      ///clears the scene.
      void RemoveAllDrawables();

      ///Get a handle to the DeltaDrawable with the supplied index number
      DeltaDrawable* GetDrawable(unsigned int i) {return mAddedDrawables[i].get(); }

      //Set the State for rendering.  Wireframe, Fill polygons, or vertex points.
      void SetRenderState( Face face, Mode mode );
      const std::pair<Face,Mode> GetRenderState() const { return std::make_pair( mRenderFace, mRenderMode ); }

      ///Get the height of terrain at a given x,y
      float GetHeightOfTerrain( float x, float y );

      ///Get the ODE space ID
      dSpaceID GetSpaceID() const;

      ///Get the ODE world ID
      dWorldID GetWorldID() const;

      ///Get the ODE contact join group ID
      dJointGroupID GetContactJoinGroupID() const;

      ///Set the gravity vector
      void SetGravity(const osg::Vec3& gravity);
      ///Set the gravity vector
      void SetGravity(float x, float y, float z) { SetGravity( osg::Vec3(x,y,z) ); }

      ///Get the gravity vector
      void GetGravity(osg::Vec3& vec) const { vec = mGravity; }

      ///Get the gravity vector
      osg::Vec3 GetGravity() const { return mGravity; }

      ///Get the gravity vector
      void GetGravity(float &x, float &y, float &z) const { x = mGravity[0]; y = mGravity[1]; z = mGravity[2]; }

      ///Performs collision detection and updates physics
      virtual void OnMessage(MessageData *data);

      ///The user data associated with "collision" messages
      struct DT_CORE_EXPORT CollisionData
      {
         Transformable* mBodies[2]; ///<The bodies colliding
         osg::Vec3 mLocation; ///<The collision location
         osg::Vec3 mNormal; ///<The collision normal
         float mDepth; ///<The penetration depth
      };

      ///Supply a user-defined collision callback to replace the internal one
      void SetUserCollisionCallback( dNearCallback *func, void *data=0 );

      /**
       * Get the step size of the physics.  The physics will
       * be updated numerous times per frame based on this number.  For example,
       * if the delta frame rate is 33ms and the step size is 2ms, the physics
       * will be updated 16 times.
       *
       * @return the step size in seconds
       * @see SetPhysicsStepSize()
       */
      double GetPhysicsStepSize() const { return mPhysicsStepSize; }

      /// @see GetPhysicsStepSize()
      void SetPhysicsStepSize( double stepSize = 0.0 ) { mPhysicsStepSize = stepSize; };

      /// Register a Transformable with the Scene
      void RegisterCollidable( Transformable* collidable );

      /// UnRegister a Transformable with the Scene
      void UnRegisterCollidable( Transformable* collidable );

      /// Returns a pointer to the light specified by the param number
      Light* GetLight( const int number ) { return mLights[ number ].get(); }
      const Light* GetLight( const int number ) const { return mLights[ number ].get(); }
      /// Returns a pointer to the light specified by the current string
      Light* GetLight( const std::string& name );
      const Light* GetLight( const std::string& name ) const;

      /// Registers a light using the light number
      void RegisterLight( Light* light );

      /// Unreferences the current light, by number, Note: does not erase
      void UnRegisterLight( Light* light );

      /// Use the internal scene light
      void UseSceneLight( bool lightState = true );

      /// Get the index number of the supplied drawable
      unsigned int GetDrawableIndex( const DeltaDrawable* drawable ) const;

      /// Get the number of Drawables which have been directly added to the Scene
      unsigned int GetNumberOfAddedDrawable() const {return mAddedDrawables.size();}

      /// DEPRECATED 06/30/08 in favor of dtCore::View::EnablePaging()
      void EnablePaging()
      {
         DEPRECATE("void dtCore::Scene::EnablePaging()",
                   "void dtCore::View::SetDatabasePager( dtCore::DatabasePager *pager )");
      }

      /// DEPRECATED 06/30/08 in favor of dtCore::View::DisablePaging()
      void DisablePaging()
      {
         DEPRECATE("dtCore::Scene::DisablePaging()",
                   "void dtCore::View::SetDatabasePager( dtCore::DatabasePager *pager )");
      }

      /// Returns if paging is enabled
      bool IsPagingEnabled() const
      {
         DEPRECATE("dtCore::Scene::DisablePaging() const",
                   "dtCore::DatabasePager* dtCore::View::GetDatabasePager() const");
         return false;
      }

      /// DEPRECATED 06/30/08
      void SetPagingCleanup(double pCleanup)
      {
         DEPRECATE("dtCore::Scene::SetPagingCleanup(double)",
                   "N/A");
      }

      /// DEPRECATED 06/30/08
      double GetPagingCleanup()
      {
         DEPRECATE("double dtCore::Scene::GetPagingCleanup()",
                   "N/A");
         return 0.0;
      }

      /// DEPRECATED 06/30/08 in favor of dtCore::DataBasePager::SetConfiguration()
      void SetConfiguration(dtUtil::ConfigProperties* config)
      {
         DEPRECATE("void dtCore::Scene::SetConfiguration(dtUtil::ConfigProperties* config)",
                   "void dtCore::DatabasePager::SetConfiguration(dUtil::ConfigProperties *config");
      }

      /// DEPRECATED 06/30/08 in favor of dtCore::DataBasePager:GetConfiguration()
      dtUtil::ConfigProperties* GetConfiguration()
      {
         DEPRECATE("dtUtil::ConfigProperties* dtCore::Scene::GetConfiguration()",
                   "dtUtil::ConfigProperties* dtCore::DatabasePager::GetConfiguration()");

         return NULL;
      }

      /// DEPRECATED 06/30/08 in favor of dtCore::DataBasePager::GetConfiguration()
      const dtUtil::ConfigProperties* GetConfiguration() const
      {
         DEPRECATE("const dtUtil::ConfigProperties* dtCore::Scene::GetConfiguration() const",
                   "const dtUtil::ConfigProperties* dtCore::DatabasePager::GetConfiguration() const" );

         return NULL;
      }


   protected:

      friend class View;

      /// Exists for unit tests.
      bool IsAssignedToView(dtCore::View& view) const;

      /// define the owner mView of this instance
      void RemoveView(dtCore::View& view);
      void AddView(dtCore::View& view);

      void SetDatabasePager( dtCore::DatabasePager *pager );
      /// Made public so I can be unit tested better.
      dtCore::DatabasePager* GetDatabasePager() const;

   private:

      // Disallowed to prevent compile errors on VS2003. It apparently
      // creates this functions even if they are not used, and if
      // this class is forward declared, these implicit functions will
      // cause compiler errors for missing calls to "ref".
      Scene& operator=( const Scene& );
      Scene( const Scene& );

      ///ODE collision callback
      static void NearCallback(void *data, dGeomID o1, dGeomID o2);


      void UpdateViewSet();

      RefPtr<osg::Group> mSceneNode; ///<This will be our Scene
      dSpaceID mSpaceID;
      dWorldID mWorldID;
      osg::Vec3 mGravity;

      ///<The time (seconds) for the physics time step.
      ///<(default = 0.0, indicating to use the System deltaFrameTime )
      double mPhysicsStepSize;

      typedef std::vector< Transformable* > TransformableVector;
      TransformableVector mCollidableContents; ///<The physical contents of the scene

      dJointGroupID mContactJointGroupID; ///<The group that contains all contact joints
      dNearCallback *mUserNearCallback;   ///<The user-supplied collision callback func
      void *mUserNearCallbackData; ///< pointer to user-supplied data

      typedef std::vector< RefPtr<Light> > LightVector;
      LightVector mLights; ///<Contains all light associated with this scene

      typedef std::vector< RefPtr<DeltaDrawable> > DrawableList;
      DrawableList mAddedDrawables; ///<The list of Drawable directly added

      typedef std::list<osg::observer_ptr<View> > ViewSet;
      ViewSet mViewSet;

      Mode mRenderMode;
      Face mRenderFace;

      ParticleSystemFreezer mFreezer;

      dtCore::RefPtr<dtCore::DatabasePager> mPager;
   };
}


#endif // DELTA_SCENE
