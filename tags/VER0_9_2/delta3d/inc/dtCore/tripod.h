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

#ifndef DELTA_TRIPOD
#define DELTA_TRIPOD

#include "dtCore/transformable.h"
#include "dtCore/refptr.h"
#include "dtCore/camera.h"
#include "dtUtil/deprecationmgr.h"

#include <osg/Vec3>
#include "sg.h"

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
   class DT_EXPORT Tripod : public Transformable
   {
   public:

      DECLARE_MANAGEMENT_LAYER(Tripod)
      
      ///Contructor which takes in a optional Camera and Transformable
      Tripod(Camera *cam = NULL, Transformable *trans = NULL);
      virtual ~Tripod();

      ///Supply a Camera to connect to this Tripod
      void SetCamera( Camera *cam );
      void SetCamera( const std::string& camName );

      ///Get the currently connected Camera
      RefPtr<Camera> GetCamera() const {return mCamera;}

      ///Attach this Tripod to a Transformable
      void SetAttachToTransformable(Transformable *trans);
      void SetAttachToTransformable(const std::string& transName);

      ///Get the currently connected Transformable
      RefPtr<Transformable> GetAttachedTransformable() const {return mParent;}
      
      ///Set the Tripod's offset from the parent Transformable
      void SetOffset(float x, float y, float z, float h, float p, float r)
      { SetOffset( osg::Vec3(x,y,z), osg::Vec3(h,p,r) ); }
      
      void SetOffset(const osg::Vec3& newPos, const osg::Vec3& newHPR) {mPosition = newPos; mHPR = newHPR; }
      //DEPRECATED
      void SetOffset(sgCoord *coord)
      {
         DEPRECATE("void SetOffset(sgCoord *coord)", "void SetOffset(const osg::Vec3& newPos, const osg::Vec3& newHPR)")
         SetOffset(osg::Vec3(coord->xyz[0], coord->xyz[1], coord->xyz[2]), osg::Vec3(coord->hpr[0], coord->hpr[1], coord->hpr[2]));
      }

      ///Get the Tripod's currently used offset coordinates
      void GetOffset(osg::Vec3& pos_in, osg::Vec3& hpr_in) const{pos_in = mPosition; hpr_in = mHPR;}
      //DEPRECATED
      void GetOffset(sgCoord *coord) 
      {
         DEPRECATE("void GetOffset(sgCoord *coord)", "void GetOffset(osg::Vec3& pos_in, osg::Vec3& hpr_in) const")
         sgSetCoord(coord, mPosition[0], mPosition[1], mPosition[2], mHPR[0], mHPR[1], mHPR[2]);
      }

      ///Set the scaling factors for each degree of freedom (0.0 - 1.0)
      void SetScale(float x, float y, float z, float h, float p, float r);
      
      void SetScale(const osg::Vec3& xyz, const osg::Vec3& hpr){mXYZScale = xyz; mHPRScale = hpr;}
      //DEPRECATED
      void SetScale(sgVec3 xyz, sgVec3 hpr)
      {
         DEPRECATE("void SetScale(sgVec3 xyz, sgVec3 hpr)", "void SetScale(const osg::Vec3& xyz, const osg::Vec3& hpr)")
         SetScale(osg::Vec3(xyz[0], xyz[1], xyz[2]), osg::Vec3(hpr[0], hpr[1], hpr[2]));
      }

      ///Get the currently used scaling factors
      void GetScale(osg::Vec3& xyz, osg::Vec3& hpr){xyz = mXYZScale; hpr = mHPRScale;}
      //DEPRECATED
      void GetScale(sgVec3 xyz, sgVec3 hpr)
      {
         DEPRECATE("void GetScale(sgVec3 xyz, sgVec3 hpr)", "void GetScale(osg::Vec3 xyz, osg::Vec3 hpr)")
         xyz[0] = mXYZScale[0]; xyz[1] = mXYZScale[1]; xyz[2] = mXYZScale[2];
         hpr[0] = mHPRScale[0]; hpr[1] = mHPRScale[1]; hpr[2] = mHPRScale[2];
      }

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
      ///Override to receive messages
      virtual void OnMessage(MessageData *data);

      ///Override to implement your own method
      virtual void Update(double deltaFrameTime);

      RefPtr<Camera> mCamera; ///<pointer to the Camera to control
      RefPtr<Transformable> mParent; ///<pointer to the parent Transformable
      osg::Vec3 mPosition; ///<The position
      osg::Vec3 mHPR;///<Heading, Pitch, and Roll
      TetherMode mTetherMode; ///<The tethering mode
      osg::Vec3 mXYZScale; ///<The scale factors for x,y,z
      osg::Vec3 mHPRScale; ///<The scale factors for h,p,r
      RefPtr<Transformable> mLookAtTarget; ///<the look-at target
   };
   
}

#endif // DELTA_TRIPOD
