#ifndef DELTA_SCENE
#define DELTA_SCENE

 // scene.h: interface for the Scene class.
//
//////////////////////////////////////////////////////////////////////



#include <Producer/Camera>
#include <osg/FrameStamp>
#include <osgUtil/SceneView>

//extern "C"
//{
   #include <ode/ode.h>
//}

#include "sg.h"
#include "base.h"
#include "object.h"
#include "physical.h"
#include "stats.h"

namespace dtCore
{
   class DT_EXPORT _SceneHandler : public Producer::Camera::SceneHandler
   {
   public:
   	_SceneHandler();
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
      ///The fixed time (seconds) for the physics
      ///time step. (default = 0.01)
      static double PHYSICS_STEPSIZE;

      Scene(std::string name = "scene");
      virtual ~Scene();
      _SceneHandler *GetSceneHandler(void) {return mSceneHandler.get();}
      osg::Group  *GetSceneNode(void) {return mSceneNode.get();}
      
      ///Add a Drawable to the Scene to be viewed.
      void AddDrawable( Drawable *drawable );

      ///Remove a Drawable from the Scene
      void RemoveDrawable( Drawable *drawable );
      
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
      
      ///Display the next statistics mode
      void SetNextStatisticsType() {mSceneHandler->mStats->SelectNextType();}

      //Display the supplied statistics type5
      void SetStatisticsType(osgUtil::Statistics::statsType type) 
      {
         mSceneHandler->mStats->SelectType(type);
      }

   private:
      
      ///ODE collision callback
      static void NearCallback(void *data, dGeomID o1, dGeomID o2);
      
      osg::ref_ptr<_SceneHandler> mSceneHandler;
      osg::ref_ptr <osg::Group> mSceneNode; ///<This will be our root scene node
      dSpaceID mSpaceID;
      dWorldID mWorldID;
      sgVec3 mGravity;
      
      std::vector<Physical*> mPhysicalContents; ///<The physical contents of the scene
      
      dJointGroupID mContactJointGroupID; ///<The group that contains all contact joints
      dNearCallback *mUserNearCallback;   ///<The user-supplied collision callback func
      void *mUserNearCallbackData; ///< pointer to user-supplied data
   };
   
};


#endif // DELTA_SCENE
