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

#ifndef DELTA_TRANSFORM
#define DELTA_TRANSFORM

#include <dtCore/export.h>
#include <osg/Matrix>
#include <osg/Vec3>
#include <osg/Vec4>
#include <iosfwd>

namespace dtUtil
{
   class DataStream;
}

namespace dtCore
{
   ////A class that represents a position and attitude in 3D space

   class DT_CORE_EXPORT Transform
   {
   public:
      typedef osg::Matrix::value_type value_type;
      typedef osg::Vec3::value_type vec_value_type;

      Transform(vec_value_type tx = 0.0f, vec_value_type ty = 0.0f, vec_value_type tz = 0.0f,
            vec_value_type h = 0.0f, vec_value_type p = 0.0f, vec_value_type r = 0.0f);
      Transform(const osg::Matrix& initialMatrix);
      Transform(const Transform& that);
      ~Transform();

      /// @return true if the transform is an identity matrix.
      bool IsIdentity() const { return mTransform.isIdentity(); }
      bool Invert() { return mTransform.invert(mTransform); }
      osg::Vec3f Multiply(const osg::Vec3f& vec) { return mTransform.preMult(vec); }
      osg::Vec3d Multiply(const osg::Vec3d& vec) { return mTransform.preMult(vec); }
      /**
       *  Multiply the transform by this one and store this result in result Out
       *  If transform is a normal transformation matrix, this will take this matrix
       *  and transform it into the space of xform, and store the result in resultOut.
       */
      void Multiply(const dtCore::Transform& xform, dtCore::Transform& resultOut)
      {
         resultOut.mTransform.mult(this->mTransform, xform.mTransform);
      }

      /// Overwrites this transform with an identity matrix
      void MakeIdentity() { mTransform.makeIdentity(); }
      /// Overwrites this transform with a scale matrix with the given values.
      void MakeScale(const osg::Vec3d& scaleVec) { mTransform.makeScale(scaleVec); }
      /// Overwrites this transform with a scale matrix with the given values.
      void MakeScale(const osg::Vec3f& scaleVec) { mTransform.makeScale(scaleVec); }
      /// Overwrites this transform with a scale matrix with the given values.
      void MakeScale(value_type x, value_type y, value_type z)
         { mTransform.makeScale(x,y,z); }

      void GetRow(unsigned index, osg::Vec3& row) const;
      void SetRow(unsigned index, const osg::Vec3& row);

      void GetRow(unsigned index, osg::Vec4& row) const;
      void SetRow(unsigned index, const osg::Vec4& row);

      ///Set both translation and rotation methods
      void Set(vec_value_type tx, vec_value_type ty, vec_value_type tz,
            vec_value_type h, vec_value_type p, vec_value_type r);
      ///Set both translation and rotation methods
      void Set(const osg::Vec3& xyz, const osg::Vec3& hprRotation);
      ///Set both translation and rotation methods
      void Set(const osg::Vec3& xyz, const osg::Quat& quat);
      ///Set both translation and rotation methods
      void Set(const osg::Vec3& xyz, const osg::Matrix& rotation);
      ///Set both translation and rotation methods
      void Set(const osg::Matrix& mat);

      ///Set both translation and rotation using target(lookat) and position
      void Set(const osg::Vec3& xyz, const osg::Vec3& lookAtXYZ, const osg::Vec3& upVec);

      ///Set both translation and rotation using target(lookat) and position
      void Set(vec_value_type posX, vec_value_type posY, vec_value_type posZ,
            vec_value_type lookAtX, vec_value_type lookAtY, vec_value_type lookAtZ,
            vec_value_type upVecX, vec_value_type upVecY, vec_value_type upVecZ);

      ///Set only translation methods
      void SetTranslation(vec_value_type tx, vec_value_type ty, vec_value_type tz) { SetTranslation(osg::Vec3(tx, ty, tz)); }
      ///Set only translation methods
      void SetTranslation(const osg::Vec3f& xyz) { mTransform.setTrans( xyz ); }
      ///Set only translation methods
      void SetTranslation(const osg::Vec3d& xyz) { mTransform.setTrans( xyz ); }

      /// adjusts the translation by the given vector
      void Move(const osg::Vec3f& distance);
      /// adjusts the translation by the given vector
      void Move(const osg::Vec3d& distance);

      /**
       * Set the rotation using heading, pitch, roll (in degrees)
       * This will wipe out any scale assigned to the matrix.
       */
      void SetRotation(vec_value_type h, vec_value_type p, vec_value_type r) { SetRotation(osg::Vec3(h, p, r)); }

      /**
       * Set the rotation using heading, pitch, roll (in degrees)
       * This will wipe out any scale assigned to the matrix.
       */
      void SetRotation(const osg::Vec3& hpr);

      /**
       * Set the rotation using a quaternion
       * This will wipe out any scale assigned to the matrix.
       */
      void SetRotation(const osg::Quat& quat) { mTransform.setRotate(quat); }

      /**
       * Set the rotation using a matrix.  Only the 3x3 portion will be used.
       */
      void SetRotation(const osg::Matrix& rotation);

      ///Get translation and rotation methods
      void Get(vec_value_type& tx, vec_value_type& ty, vec_value_type& tz, vec_value_type& h, vec_value_type& p, vec_value_type& r) const;
      ///Get translation and rotation methods
      void Get(osg::Vec3& xyz, osg::Matrix& rotation) const;
      ///Get translation and rotation methods
      void Get(osg::Vec3& xyz, osg::Vec3& hpr) const;
      ///Get translation and rotation methods
      void Get(osg::Vec3& xyz, osg::Quat& quat) const;
      ///Get translation and rotation methods
      void Get(osg::Matrix& matrix) const;

      ///Get only translation methods
      void GetTranslation(osg::Vec3f& translation) const { translation.set(mTransform.getTrans()); }
      ///Get only translation methods
      void GetTranslation(osg::Vec3d& translation) const { translation.set(mTransform.getTrans()); }
      ///Get only translation methods
      void GetTranslation(vec_value_type& tx, vec_value_type& ty, vec_value_type& tz) const;
      ///Get only translation methods
      const osg::Vec3 GetTranslation() const { return mTransform.getTrans(); }

      ///Get only rotation methods
      void GetRotation(vec_value_type& h, vec_value_type& p, vec_value_type& r) const;
      ///Get only rotation methods
      void GetRotation(osg::Vec3& hpr) const;
      ///Get only rotation methods
      void GetRotation(osg::Quat& quat) const { quat = mTransform.getRotate(); }
      ///Get only rotation methods
      void GetRotation(osg::Matrix& rotation) const;
      ///Get only rotation methods
      osg::Vec3 GetRotation() const { osg::Vec3 hpr; GetRotation(hpr); return hpr; }

      ///gets the contents of the matrix as the basis vectors that make up it's rows
      void GetOrientation(osg::Vec3& right, osg::Vec3& up, osg::Vec3& forward) const;
      ///gets the vector that represents the right direction from this transform
      osg::Vec3 GetRightVector() const;
      ///gets the vector that represents the up direction from this transform
      osg::Vec3 GetUpVector() const;
      ///gets the vector that represents the forward direction from this transform
      osg::Vec3 GetForwardVector() const;

      /// calculates the scale using a polar decomposition.
      void CalcScale(osg::Vec3f& scale) const;
      /// calculates the scale using a polar decomposition.
      void CalcScale(osg::Vec3d& scale) const;

      /// Calculates the distance from this trannform to another
      value_type CalcDistance(const dtCore::Transform& xform) const;
      /// Calculates the distance squared from this transform to another
      value_type CalcDistanceSquared(const dtCore::Transform& xform) const;

      /**
       * Does a polar decomposition (SLOW) on the matrix to get the scale and the unscaled rotation matrix,
       * then rescales it based on the vector passed in.  This is not something you want to call often.
       * Note - this scale value will be wiped out if you call SetRotation(). Scale is really only
       * relevant to visible 3D objects - it was separated out to significantly improve performance.
       * To use a permanent scale on an object, you should have a separate matrix transform that holds
       * the scale value and make your mesh be a child of that. See GameMeshActor for an example.
       */
      void Rescale(const osg::Vec3d& scale);

      /**
       * Does a polar decomposition (SLOW) on the matrix to get the scale and the unscaled rotation matrix,
       * then rescales it based on the vector passed in.  This is not something you want to call often.
       * Note - this scale value will be wiped out if you call SetRotation(). Scale is really only
       * relevant to visible 3D objects - it was separated out to significantly improve performance.
       * To use a permanent scale on an object, you should have a separate matrix transform that holds
       * the scale value and make your mesh be a child of that. See GameMeshActor for an example.
       */
      void Rescale(const osg::Vec3f& scale);

      /** checks to see if the param transform is within epsilon of this transform
       *  slightly more sophisticated as using operator ==
       *  The epsilon will scale up and down based on the values of the floats using the passed id in epsilon
       *  as a base.
       */
      bool EpsilonEquals(const Transform& transform, value_type epsilon = 0.0001) const;

      /// @return true if the all the elements are valid floats, i.e. not NAN
      bool IsValid() const { return mTransform.valid(); };

      /// @return the refernce to the position in the transform matrix
      osg::Matrix::value_type& operator()(unsigned i, unsigned j) { return mTransform(i, j); }

      /// @return the value of the position in the transform matrix
      osg::Matrix::value_type operator()(unsigned i, unsigned j) const { return mTransform(i, j); }

      Transform & operator=(const Transform &);
      bool        operator==(const Transform &);

   private:

      osg::Matrix mTransform; ///<Internal storage
   };

   DT_CORE_EXPORT std::ostream& operator << (std::ostream& o, const Transform& xform);

   DT_CORE_EXPORT std::istream& operator >> (std::istream& i, Transform& xform);

   DT_CORE_EXPORT dtUtil::DataStream& operator << (dtUtil::DataStream& ds, const Transform& xform);

   DT_CORE_EXPORT dtUtil::DataStream& operator >> (dtUtil::DataStream& ds, Transform& xform);

}



#endif // DELTA_TRANSFORM
