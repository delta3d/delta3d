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
#include <osg/FrameStamp>
#include <osgUtil/SceneView>

#include <ode/ode.h>
#include "sg.h"
#include "dtCore/base.h"
#include "dtCore/object.h"
#include "dtCore/physical.h"
#include "dtCore/stats.h"


//NOTE: EVIL! fix w/ static const int
const int MAX_LIGHT_NUMBER = 8;

namespace dtCore
{         
   class Light; //forward declaration

   class DT_EXPORT _SceneHandler : public Producer::Camera::SceneHandler
   {
   public:
   	_SceneHandler(bool useSceneLight=true);
   	virtual ~_SceneHandler();

      osgUtil::SceneView* GetSceneView() { return mSceneView.get(); }

      virtual void clear(Producer::Camera& cam);

      void ClearImplementation( Producer::Camera& cam);

      /** 
      *  Prepare the scene by sorting, and
      *  ordering for optimal rendering
      */
      virtual void cull( Producer::Camera &cam);

      void CullImplementation( Producer::Camera &cam );

      /** 
      *  The draw() method must be implemented by
      *  the derived class for rendering the scene
      */
      virtual void draw( Producer::Camera &cam);

      void DrawImplementation( Producer::Camera &cam );

      Stats *mStats; ///<The statistics display

   protected:
   	osg::ref_ptr<osgUtil::SceneView> mSceneView;
      ulClock mClock;
   private:
     osg::ref_ptr<osg::FrameStamp> mFrameStamp;
   };
   
   
   class DT_EXPORT Scene : public Base  
   {
      DECLARE_MANAGEMENT_LAYER(Scene)

   public:

      Scene(std::string name = "scene", bool useSceneLight = true);
      virtual ~Scene();
      _SceneHandler *GetSceneHandler(void) {return mSceneHandler.get();}
      osg::Group  *GetSceneNode(void) {return mSceneNode.get();}
      
      ///Add a DeltaDrawable to the Scene to be viewed.
      void AddDrawable( DeltaDrawable* drawable );

      ///Remove a DeltaDrawable from the Scene
      void RemoveDrawable( DeltaDrawable *drawable );
     
      ///Get the height of terrain at a given x,y
      float GetHeightOfTerrain( const float *x, const float *y);
      
      ///Get the ODE space ID
      dSpaceID GetSpaceID() const;
      
      ///Get the ODE world ID
      dWorldID GetWorldID() const;
      
      ///Set the gravity vector
      void SetGravity(sgVec3 gravity);
      void SetGravity(float x, float y, float z);
      
      ///Get the gravity vector
      void GetGravity(sgVec3 vec);
      void GetGravity(float* x, float* y, float* z);
      
      ///Performs collision detection and updates physics
      virtual void OnMessage(MessageData *data);
      
      ///The user data associated with "collision" messages
      struct DT_EXPORT CollisionData
      {
         Physical* mBodies[2]; ///<The bodies colliding
         sgVec3 mLocation; ///<The collision location
         sgVec3 mNormal; ///<The collision normal
         float mDepth; ///<The penetration depth
      };
      
      ///Supply a user-defined collision callback to replace the internal one
      void SetUserCollisionCallback( dNearCallback *func, void *data=NULL );

      double GetPhysicsStepSize( void ){ return mPhysicsStepSize; }
      void SetPhysicsStepSize( double stepSize = 0.0 );
      
      ///Display the next statistics mode
      void SetNextStatisticsType() {mSceneHandler->mStats->SelectNextType();}

      //Display the supplied statistics type5
      void SetStatisticsType(osgUtil::Statistics::statsType type) 
      {
         mSceneHandler->mStats->SelectType(type);
      }

      // Register a Physical with the Scene
      void RegisterPhysical( Physical *physical);

		// UnRegister a Physical with the Scene
		void UnRegisterPhysical( Physical *physical);

      // Register a Light with the Scene
      void RegisterLight( Light* light );
      
      // Removes a light from the scene
      void UnRegisterLight( Light* light );      

      inline Light* GetLight( int number ) const { return mLights[ number ]; }
      Light* GetLight( const std::string name ) const;

      void UseSceneLight( bool lightState = true );

   private:
      
      ///ODE collision callback
      static void NearCallback(void *data, dGeomID o1, dGeomID o2);
      
      osg::ref_ptr<_SceneHandler> mSceneHandler;
      osg::ref_ptr<osg::Group> mSceneNode; ///<This will be our root scene node
      dSpaceID mSpaceID;
      dWorldID mWorldID;
      sgVec3 mGravity;
      
      // The time (seconds) for the physics time step. 
      // (default = 0.0, indicating to use the System deltaFrameTime )
      double mPhysicsStepSize;
      
      std::vector<Physical*> mPhysicalContents; ///<The physical contents of the scene
      
      dJointGroupID mContactJointGroupID; ///<The group that contains all contact joints
      dNearCallback *mUserNearCallback;   ///<The user-supplied collision callback func
      void *mUserNearCallbackData; ///< pointer to user-supplied data

      //static 
      osg::Group* mLightGroup; // single light group for all scene lights
      Light* mLights[ MAX_LIGHT_NUMBER ]; // contains all light associated with this scene
   };
   
};


#endif // DELTA_SCENE
