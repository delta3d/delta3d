#pragma once
#include "transformable.h"
#include "camera.h"

namespace dtCore
{
   ///A Tripod is used to attach a Camera to a Transformable

   /** The Tripod is useful for attaching a Camera to another
    *  Transformable.  The Tripod has a built-in offset which allows the Camera
    *  to "stand-off" the parent Transformable.  The user may also supply a scale
    *  valued for the x,y,z, and h,p,r coordinates.  These scale values allow 
    *  the Camera to "lag" behind.
    *  Also, the Tripod may be instructed to always point toward another
    *  Transformable.
    */
   class DT_EXPORT Tripod : public Base
   {
   public:

      ///Contructor which takes in a optional Camera and Transformable
      Tripod(Camera *cam = NULL, Transformable *trans = NULL);
      virtual ~Tripod(void);

      ///Supply a Camera to connect to this Tripod
      void SetCamera( Camera *cam);
      void SetCamera( const std::string camName);

      ///Get the currently connected Camera
      osg::ref_ptr<Camera> GetCamera(void)const {return mCamera;}

      ///Attach this Tripod to a Transformable
      void SetAttachToTransformable(Transformable *trans);
      void SetAttachToTransformable(std::string transName);

      ///Get the currently connected Transformable
      osg::ref_ptr<Transformable> GetAttachedTransformable(void) const {return mParent;}
      
      ///Set the Tripod's offset from the parent Transformable
      void SetOffset(float x, float y, float z, float h, float p, float r);
      void SetOffset(sgCoord *coord) {sgCopyCoord(&mOffsetCoord, coord);}

      ///Get the Tripod's currently used offset coordinates
      void GetOffset(sgCoord *coord) {sgCopyCoord(coord, &mOffsetCoord);}

      ///Set the scaling factors for each degree of freedom (0.0 - 1.0)
      void SetScale(float x, float y, float z, float h, float p, float r);
      void SetScale(sgVec3 xyz, sgVec3 hpr);

      ///Get the currently used scaling factors
      void GetScale(sgVec3 xyz, sgVec3 hpr);

      enum TetherMode
      {
         TETHER_PARENT_REL,  ///<offsets are in the Parent's coordinate system
         TETHER_WORLD_REL ///<offsets are in the world coordinate system
      };

      ///Set the mode of how the Tripod connects to the parent
      void SetTetherMode( TetherMode mode) {mTetherMode = mode;}
      TetherMode GetTetherMode(void)const {return mTetherMode;}

      ///Point the Tripod at this Transformable
      void SetLookAtTarget(Transformable *target);

   protected:
      ///Override to receive messages
      virtual void OnMessage(MessageData *data);

      ///Override to implement your own method
      virtual void Update(const double deltaFrameTime);

      osg::ref_ptr<Camera> mCamera; ///<pointer to the Camera to control
      osg::ref_ptr<Transformable> mParent; ///<pointer to the parent Transformable
      sgCoord mOffsetCoord; ///<The offset values
      TetherMode mTetherMode; ///<The tethering mode
      sgVec3 mXYZScale; ///<The scale factors for x,y,z
      sgVec3 mHPRScale; ///<The scale factors for h,p,r
      osg::ref_ptr<Transformable> mLookAtTarget; ///<the look-at target
   };
   
}
