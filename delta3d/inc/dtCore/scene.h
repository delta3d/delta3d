/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#include <Producer/Camera>
#include <ode/ode.h>
#include <osg/Vec3>
#include "dtCore/base.h"
#include "dtCore/stats.h"
#include "dtCore/light.h"

#include "dtUtil/deprecationmgr.h"

namespace dtCore
{         
   //forward declaration
   class DeltaDrawable;
   class Transformable;
   class Physical;

   /*
   Deprecated 3/23/04: dtCore::_SceneHandler

   This was removed to support multi-window, multi-camera
   funtionality. Since more than one camera+window can be
   used each dtCore::Camera must have its own
   Producer::SceneHandler.

   So use dtCore::Camera::_SceneHandler instead.
   */
   
   class DT_EXPORT Scene : public Base  
   {
      DECLARE_MANAGEMENT_LAYER(Scene)

   public:

      enum Face 
      {
         FRONT,
         BACK,
         FRONT_AND_BACK
      };

      enum Mode 
      {
         POINT = GL_POINT,
         LINE = GL_LINE,
         FILL = GL_FILL
      };

      Scene(std::string name = "scene", bool useSceneLight = true);
      virtual ~Scene();

      ///Get a pointer to the internal scene node
      osg::Group  *GetSceneNode(void) {return mSceneNode.get();}
      
      ///Add a DeltaDrawable to the Scene to be viewed.
      void AddDrawable( DeltaDrawable* drawable );

      ///Remove a DeltaDrawable from the Scene
      void RemoveDrawable( DeltaDrawable *drawable );

      ///Get a handle to the DeltaDrawable with the supplied index number
      DeltaDrawable* GetDrawable(unsigned int i) {return mAddedDrawables[i].get(); }

      //Set the State for rendering.  Wireframe, Fill polygons, or vertex points.
      void SetRenderState( Face face, Mode mode );
      const std::pair<Face,Mode> GetRenderState() const { return std::make_pair( mRenderFace, mRenderMode ); }
     
      ///Get the height of terrain at a given x,y
      float GetHeightOfTerrain( const float *x, const float *y);
      
      ///Get the ODE space ID
      dSpaceID GetSpaceID() const;
      
      ///Get the ODE world ID
      dWorldID GetWorldID() const;
      
      ///Set the gravity vector
      void SetGravity(osg::Vec3 gravity);
      void SetGravity(float x, float y, float z);
      
      ///Get the gravity vector
      void GetGravity(osg::Vec3 vec);
      void GetGravity(float* x, float* y, float* z);
      
      ///Performs collision detection and updates physics
      virtual void OnMessage(MessageData *data);
      
      ///The user data associated with "collision" messages
      struct DT_EXPORT CollisionData
      {
         Physical* mBodies[2]; ///<The bodies colliding
         osg::Vec3 mLocation; ///<The collision location
         osg::Vec3 mNormal; ///<The collision normal
         float mDepth; ///<The penetration depth
      };
      
      ///Supply a user-defined collision callback to replace the internal one
      void SetUserCollisionCallback( dNearCallback *func, void *data=NULL );

      /** Get the step size of the physics.  The physics will 
      *  be updated numerous times per frame based on this number.  For example,
      *  if the delta frame rate is 33ms and the step size is 2ms, the physics
      *  will be updated 16 times.
      *  @return the step size in seconds
      *  @see SetPhysicsStepSize()
      */
      inline double GetPhysicsStepSize( void ) const { return mPhysicsStepSize; }

      /// @see GetPhysicsStepSize()
      inline void SetPhysicsStepSize( const double stepSize = 0.0 ){ mPhysicsStepSize = stepSize; };
      
      ///Deprecated 3/23/05
      void SetNextStatisticsType() 
      {
         DEPRECATE("void Scene::SetNextStatisticsType()",
                   "void Camera::SetNextStatisticsType()")
      }

      ///Deprecated 3/23/05
      void SetStatisticsType(osgUtil::Statistics::statsType type) 
      {
         DEPRECATE("void Scene::SetStatisticsType(osgUtil::Statistics::statsType type)",
                   "void Camera::SetStatisticsType(osgUtil::Statistics::statsType type)")    
      }

      /// Register a Physical with the Scene
      void RegisterPhysical( Physical *physical);

		/// UnRegister a Physical with the Scene
		void UnRegisterPhysical( Physical *physical);

      inline Light* GetLight( const int number ) const { return mLights[ number ]; }
      Light* GetLight( const std::string name ) const;

      inline void RegisterLight( Light* light )
      { 
         mLights[ light->GetNumber() ] = light; //add to internal array of lights
      }

      inline void UnRegisterLight( Light* light )
      { 
         mLights[ light->GetNumber() ] = NULL; //add to internal array of lights
      }

      ///Use the internal scene light
      void UseSceneLight( bool lightState = true );

      ///Get the index number of the supplied drawable
      inline unsigned int GetDrawableIndex( const DeltaDrawable* drawable ) const
      {
         for (unsigned int childNum=0;childNum<mAddedDrawables.size();++childNum)
         {
            if (mAddedDrawables[childNum]==drawable) return childNum;
         }
         return mAddedDrawables.size(); // node not found.
      }
   
      ///Get the number of Drawables which have been directly added to the Scene
      int GetNumberOfAddedDrawable(void) const {return mAddedDrawables.size();}     

    private:
      
      ///ODE collision callback
      static void NearCallback(void *data, dGeomID o1, dGeomID o2);
      
      RefPtr<osg::Group> mSceneNode; ///<This will be our root scene node
      dSpaceID mSpaceID;
      dWorldID mWorldID;
      osg::Vec3 mGravity;
      
      // The time (seconds) for the physics time step. 
      // (default = 0.0, indicating to use the System deltaFrameTime )
      double mPhysicsStepSize;
      
      std::vector<Physical*> mPhysicalContents; ///<The physical contents of the scene
      
      dJointGroupID mContactJointGroupID; ///<The group that contains all contact joints
      dNearCallback *mUserNearCallback;   ///<The user-supplied collision callback func
      void *mUserNearCallbackData; ///< pointer to user-supplied data

      Light* mLights[ MAX_LIGHTS ]; // contains all light associated with this scene

      typedef std::vector< RefPtr<DeltaDrawable> > DrawableList;

      DrawableList mAddedDrawables; ///<The list of Drawable directly added

      Mode mRenderMode;
      Face mRenderFace;

   };   
};


#endif // DELTA_SCENE
