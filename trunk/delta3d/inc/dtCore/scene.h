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

   
   /**
   *  Scene: This class encapsulates the root of the delta scene graph
   *
   *
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

      Scene(const std::string& name = "scene", bool useSceneLight = true);
      virtual ~Scene();

      ///Get a pointer to the internal scene node
      osg::Group *GetSceneNode() {return mSceneNode.get();}
      
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
      float GetHeightOfTerrain( float x, float y );
      
      ///Get the ODE space ID
      dSpaceID GetSpaceID() const;
      
      ///Get the ODE world ID
      dWorldID GetWorldID() const;
      
      ///Set the gravity vector
      void SetGravity(const osg::Vec3& gravity);
      ///Set the gravity vector
      void SetGravity(float x, float y, float z) { SetGravity( osg::Vec3(x,y,z) ); }

     
      ///Get the gravity vector
      void GetGravity(osg::Vec3& vec) const { vec = mGravity; }
      ///Get the gravity vector
      void GetGravity(float* x, float* y, float* z) const { *x = mGravity[0]; *y = mGravity[1]; *z = mGravity[2]; }

      
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
      void SetUserCollisionCallback( dNearCallback *func, void *data=0 );

      /** Get the step size of the physics.  The physics will 
      *  be updated numerous times per frame based on this number.  For example,
      *  if the delta frame rate is 33ms and the step size is 2ms, the physics
      *  will be updated 16 times.
      *  @return the step size in seconds
      *  @see SetPhysicsStepSize()
      */
      inline double GetPhysicsStepSize() const { return mPhysicsStepSize; }

      /// @see GetPhysicsStepSize()
      inline void SetPhysicsStepSize( double stepSize = 0.0 ){ mPhysicsStepSize = stepSize; };
      

      /// Register a Physical with the Scene
      void RegisterPhysical( Physical *physical );

		/// UnRegister a Physical with the Scene
		void UnRegisterPhysical( Physical *physical );

      ///returns a pointer to the light specified by the param number
      inline Light* GetLight( const int number ) const { return mLights[ number ]; }
      ///returns a pointer to the light specified by the current string
      Light* GetLight( const std::string& name ) const;

      ///registers a light using the light number
      inline void RegisterLight( Light* light )
      { 
         mLights[ light->GetNumber() ] = light; //add to internal array of lights
      }

      ///unreferences the current light, by number, Note: does not erase
      inline void UnRegisterLight( Light* light )
      { 
         mLights[ light->GetNumber() ] = 0; 
      }

      ///Use the internal scene light
      void UseSceneLight( bool lightState = true );

      ///Get the index number of the supplied drawable
      unsigned int GetDrawableIndex( const DeltaDrawable* drawable ) const
      {
         for (unsigned int childNum=0;childNum<mAddedDrawables.size();++childNum)
         {
            if (mAddedDrawables[childNum]==drawable) return childNum;
         }
         return mAddedDrawables.size(); // node not found.
      }
   
      ///Get the number of Drawables which have been directly added to the Scene
      int GetNumberOfAddedDrawable() const {return mAddedDrawables.size();}     

      /**
      * Enables paging when called ONLY AFTER a page-able
      * node has been added to the scene
      * @note all settings must be made before this call
      */
      void EnablePaging();
      
      /**
      *  Disables Paging, after enabled
      *  called on scene cleanup
      */
      void DisablePaging();


      /**
      *  Set's Slice time allocated for scene cleanup
      *  default 0.0025
      *  @param allocated cleanup time in seconds
      */
      void SetPagingCleanup(double pCleanup){mCleanupTime = pCleanup;}

      /**
      * Sets target frame rate for database pager, default 60
      * @param target framerate for paging thread in Frames / Sec
      */
      void SetTargetFrameRate(double pTargetFR){mTargetFrameRate = pTargetFR;}

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

      bool mPagingEnabled;
      Timer_t mStartTick;
      unsigned int mFrameNum;
      double mCleanupTime;
      double mTargetFrameRate;
   };   
};


#endif // DELTA_SCENE
