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

#ifndef DELTA_TRIPOD
#define DELTA_TRIPOD

#include "dtCore/base.h"
#include "dtCore/camera.h"

#include <osg/Vec3>


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
    *
    *  Note: To have a Camera keep a fixed, parent-relative distance from the parent,
    *  you can simply use the functionality in Transformable and not use the Tripod.
    *  \code
    *    myParent->AddChild( myCamera );
    *    Transform offset(x,y,z,h,p,r);
    *    myCamera->SetTransform( &offset, Transformable::REL_CS );
    *  \endcode
    */
   class DT_CORE_EXPORT Tripod : public Base
   {
   public:

      DECLARE_MANAGEMENT_LAYER(Tripod)
      
      ///Contructor which takes in a optional Camera and Transformable
      Tripod(Camera *cam = NULL, Transformable *parent = NULL);

      ///Supply a Camera to connect to this Tripod
      void SetCamera( Camera *cam );

      ///set camera by name
      void SetCamera( const std::string& camName );

      ///Get the currently connected Camera
      RefPtr<Camera> GetCamera() const {return mCamera;}

      ///Attach this Tripod to a Transformable
      void SetAttachToTransformable(Transformable *parent);
      ///Attach to transformable by name
      void SetAttachToTransformable(const std::string& parentName);

      ///Get the currently connected Transformable
      RefPtr<Transformable> GetAttachedTransformable() const {return mParentTrans;}
      
      ///Set the Tripod's offset from the parent Transformable
      void SetOffset(float x, float y, float z, float h, float p, float r)
      { SetOffset( osg::Vec3(x,y,z), osg::Vec3(h,p,r) ); }
      
      ///Set the Tripod's offset from the parent Transformable
      void SetOffset(const osg::Vec3& newPos, const osg::Vec3& newHPR) {mPosition = newPos; mHPR = newHPR; }

      ///Get the Tripod's currently used offset coordinates
      void GetOffset(osg::Vec3& pos_in, osg::Vec3& hpr_in) const{pos_in = mPosition; hpr_in = mHPR;}

      ///Set the scaling factors for each degree of freedom (0.0 - 1.0)
      void SetScale(float x, float y, float z, float h, float p, float r);
      
       ///Set the scaling factors for each degree of freedom (0.0 - 1.0)
      void SetScale(const osg::Vec3& xyz, const osg::Vec3& hpr){mXYZScale = xyz; mHPRScale = hpr;}

      ///Get the currently used scaling factors
      void GetScale(osg::Vec3& xyz, osg::Vec3& hpr){xyz = mXYZScale; hpr = mHPRScale;}

      enum TetherMode
      {
         TETHER_PARENT_REL,  ///<offsets are in the Parent's coordinate system
         TETHER_WORLD_REL ///<offsets are in the world coordinate system
      };

      ///Set the mode of how the Tripod connects to the parent
      void SetTetherMode( TetherMode mode) {mTetherMode = mode;}
      TetherMode GetTetherMode()const {return mTetherMode;}

      ///Point the Tripod at this Transformable
      void SetLookAtTarget(Transformable *target);

   protected:

      virtual ~Tripod();

      ///Override to receive messages
      virtual void OnMessage(MessageData *data);

      ///Override to implement your own method
      virtual void Update(double deltaFrameTime);

      RefPtr<Camera> mCamera; ///<pointer to the Camera to control
      RefPtr<Transformable> mParentTrans; ///<pointer to the parent Transformable
      osg::Vec3 mPosition; ///<The position
      osg::Vec3 mHPR;///<Heading, Pitch, and Roll
      TetherMode mTetherMode; ///<The tethering mode
      osg::Vec3 mXYZScale; ///<The scale factors for x,y,z
      osg::Vec3 mHPRScale; ///<The scale factors for h,p,r
      RefPtr<Transformable> mLookAtTarget; ///<the look-at target
   };

}

#endif // DELTA_TRIPOD
