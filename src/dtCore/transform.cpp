#include <prefix/dtcoreprefix.h>
#include <dtCore/transform.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/polardecomp.h>
#include <dtUtil/mathdefines.h>
#include <cmath>

namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////
   Transform::Transform(float tx, float ty, float tz, float h, float p, float r)
   {
      Set(tx, ty, tz, h, p, r);
   }

   /////////////////////////////////////////////////////////////////////////////
   Transform::Transform(const Transform& that)
   {
      mTransform = that.mTransform;
   }

   /////////////////////////////////////////////////////////////////////////////
   Transform::~Transform()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::GetRow(unsigned index, osg::Vec3& row) const
   {
      row = dtUtil::MatrixUtil::GetRow3(mTransform, int(index));
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::SetRow(unsigned index, const osg::Vec3& row)
   {
      dtUtil::MatrixUtil::SetRow(mTransform, row, int(index));
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::GetRow(unsigned index, osg::Vec4& row) const
   {
      row = dtUtil::MatrixUtil::GetRow4(mTransform, int(index));
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::SetRow(unsigned index, const osg::Vec4& row)
   {
      dtUtil::MatrixUtil::SetRow(mTransform, row, int(index));
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Set(float tx, float ty, float tz, float h, float p, float r)
   {
      osg::Vec3 xyz(tx, ty, tz);
      osg::Vec3 hpr(h, p, r);

      Set(xyz, hpr);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Set(const osg::Vec3& xyz, const osg::Matrix& rotation)
   {
      mTransform.set(rotation);
      mTransform.setTrans(xyz);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Set(const osg::Vec3& xyz, const osg::Quat& quat)
   {
      SetTranslation(xyz);
      SetRotation(quat);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Set(const osg::Vec3& xyz, const osg::Vec3& hpr)
   {
      osg::Matrix rotation;
      dtUtil::MatrixUtil::HprToMatrix(rotation, hpr);

      Set(xyz, rotation);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Set(const osg::Matrix& mat)
   {
      //osg::Vec3 x_vec( mat(0, 0), mat(0, 1), mat(0, 2) );
      //osg::Vec3 y_vec( mat(1, 0), mat(1, 1), mat(1, 2) );
      //osg::Vec3 z_vec( mat(2, 0), mat(2, 1), mat(2, 2) );
      //osg::Vec3 scale(x_vec.length(), y_vec.length(), z_vec.length());

      //mRotation.set(
      //                  mat(0, 0) / scale[0], mat(0, 1) / scale[0], mat(0, 2) / scale[0],  0.0f,
      //                  mat(1, 0) / scale[1], mat(1, 1) / scale[1], mat(1, 2) / scale[1],  0.0f,
      //                  mat(2, 0) / scale[2], mat(2, 1) / scale[2], mat(2, 2) / scale[2],  0.0f,
      //                  0.0f,                 0.0f,                 0.0f,                  1.0f
      //              );

      //mTranslation.set(mat(3, 0), mat(3, 1), mat(3, 2));
      //mScale.set(scale);

       //osg::Matrix rotation, scale;
       //osg::Vec3 translation;

       //dtUtil::PolarDecomp::Decompose( mat, rotation, scale, translation );

       //SetTranslation( translation );
       //SetRotation( rotation );
       //mScale.set( scale(0,0), scale(1,1), scale(2,2) );
      mTransform = mat;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::SetRotation(const osg::Vec3& hpr)
   {
      osg::Matrix rotation;
      dtUtil::MatrixUtil::HprToMatrix(rotation, hpr);
      SetRotation(rotation);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::SetRotation(const osg::Matrix& rotation)
   {
      osg::Vec3d trans;
      trans = mTransform.getTrans();
      mTransform.set(rotation);
      mTransform.setTrans(trans);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Get(float& tx, float& ty, float& tz, float& h, float& p, float& r) const
   {
      GetTranslation(tx, ty, tz);
      GetRotation(h, p, r);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Get(osg::Vec3& xyz, osg::Matrix& rotation) const
   {
      GetTranslation(xyz);
      GetRotation(rotation);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Get(osg::Matrix& matrix) const
   {
      matrix = mTransform;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Get(osg::Vec3& xyz, osg::Quat& quat) const
   {
      GetTranslation(xyz);
      GetRotation(quat);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Get(osg::Vec3& xyz, osg::Vec3& hpr) const
   {
      GetTranslation(xyz);
      GetRotation(hpr);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::GetTranslation(float& tx, float& ty, float& tz) const
   {
      osg::Vec3f vec3;
      GetTranslation(vec3);
      tx = vec3.x();
      ty = vec3.y();
      tz = vec3.z();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Move(const osg::Vec3f& distance)
   {
      mTransform.setTrans(mTransform.getTrans() + distance);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Move(const osg::Vec3d& distance)
   {
      mTransform.setTrans(mTransform.getTrans() + distance);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::GetRotation(float& h, float& p, float& r) const
   {
      osg::Vec3 hpr;
      dtUtil::MatrixUtil::MatrixToHpr(hpr, mTransform);

      h = hpr[0];
      p = hpr[1];
      r = hpr[2];
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::GetRotation(osg::Vec3& hpr) const
   {
      dtUtil::MatrixUtil::MatrixToHpr(hpr, mTransform);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::GetRotation(osg::Matrix& rotation) const
   {
      rotation.set(mTransform);
      rotation.setTrans(0.0, 0.0, 0.0);
   }

   /////////////////////////////////////////////////////////////////////////////
   template <typename VecType>
   static void Rescale(const VecType& scale, osg::Matrix& toScale)
   {
      osg::Matrix rotation, matScale;
      osg::Vec3 translation;

      dtUtil::PolarDecomp::Decompose(toScale, rotation, matScale, translation);
      matScale.makeScale(scale);
      rotation.setTrans(translation);
      toScale = rotation * matScale;
   }

   /////////////////////////////////////////////////////////////////////////////
   template <typename VecType>
   static void CalcScale(VecType& scale, const osg::Matrix& transform)
   {
      osg::Matrix rotation, matScale;
      osg::Vec3 translation;
      dtUtil::PolarDecomp::Decompose(transform, rotation, matScale, translation);

      scale[0] = matScale(0,0);
      scale[1] = matScale(1,1);
      scale[2] = matScale(2,2);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::CalcScale(osg::Vec3f& scale) const
   {
      dtCore::CalcScale(scale, mTransform);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::CalcScale(osg::Vec3d& scale) const
   {
      dtCore::CalcScale(scale, mTransform);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Rescale(const osg::Vec3d& scale)
   {
      dtCore::Rescale(scale, mTransform);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Rescale(const osg::Vec3f& scale)
   {
      dtCore::Rescale(scale, mTransform);
   }

   /////////////////////////////////////////////////////////////////////////////
   double Transform::CalcDistance(const dtCore::Transform& xform) const
   {
      return sqrt(CalcDistanceSquared(xform));
   }

   /////////////////////////////////////////////////////////////////////////////
   double Transform::CalcDistanceSquared(const dtCore::Transform& xform) const
   {
      osg::Vec3 positionOne, positionTwo;
      GetTranslation(positionOne);
      xform.GetTranslation(positionTwo);

      return (((positionOne[0]-positionTwo[0]) * (positionOne[0]-positionTwo[0])) +
              ((positionOne[1]-positionTwo[1]) * (positionOne[1]-positionTwo[1])) +
              ((positionOne[2]-positionTwo[2]) * (positionOne[2]-positionTwo[2])));
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Set(const osg::Vec3& xyz, const osg::Vec3& lookAtXYZ, const osg::Vec3& upVec)
   {
      osg::Vec3 x,y,z;

      y = lookAtXYZ - xyz;
      z = upVec;
      x = y ^ z;
      z = x ^ y;

      x.normalize();
      y.normalize();
      z.normalize();

      dtUtil::MatrixUtil::SetRow(mTransform, x, 0);
      dtUtil::MatrixUtil::SetRow(mTransform, y, 1);
      dtUtil::MatrixUtil::SetRow(mTransform, z, 2);
      dtUtil::MatrixUtil::SetRow(mTransform, xyz, 3);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::Set(float posX, float posY, float posZ,
                       float lookAtX, float lookAtY, float lookAtZ,
                       float upVecX, float upVecY, float upVecZ)
   {
      osg::Vec3 xyz(posX, posY, posZ);
      osg::Vec3 lookAt(lookAtX, lookAtY, lookAtZ);
      osg::Vec3 upVec(upVecX, upVecY, upVecZ);

      Set(xyz, lookAt, upVec);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Transform::GetOrientation(osg::Vec3& right, osg::Vec3& up, osg::Vec3& forward) const
   {
      right   = GetRightVector();
      up      = GetUpVector();
      forward = GetForwardVector();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Transform::GetRightVector() const
   {
      return dtUtil::MatrixUtil::GetRow3(mTransform, 0);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Transform::GetUpVector() const
   {
      return dtUtil::MatrixUtil::GetRow3(mTransform, 2);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Transform::GetForwardVector() const
   {
      return dtUtil::MatrixUtil::GetRow3(mTransform, 1);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Transform::EpsilonEquals(const Transform& transform, float epsilon) const
   {
      for(int i = 0; i < 4; i++)
      {
         for(int j = 0; j < 4; j++)
         {
            if(!dtUtil::Equivalent(mTransform(i,j), transform.mTransform(i,j), osg::Matrix::value_type(epsilon)))
            {
               return false;
            }
         }
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   Transform& Transform::operator=(const Transform& rhs)
   {
      if (this == &rhs) return *this;

      mTransform = rhs.mTransform;

      return *this;
   }

   /////////////////////////////////////////////////////////////////////////////////////////////
   bool Transform::operator==(const Transform& rhs)
   {
      if (this == &rhs) return true;

      return EpsilonEquals(rhs, 0.0f);
   }

   /////////////////////////////////////////////////////////////////////////////
}
