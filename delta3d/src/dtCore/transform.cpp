#include "dtCore/transform.h"
#include "dtCore/notify.h"
#include "dtUtil/matrixutil.h"
#include "dtUtil/polardecomp.h"
#include "dtUtil/deprecationmgr.h"

using namespace dtCore;

Transform::Transform( float tx, float ty, float tz, float h, float p, float r, float sx, float sy, float sz )
{
   Set( tx, ty, tz, h, p, r, sx, sy, sz );
}

Transform::Transform( const Transform& that )
{
   SetTranslation( that.mTranslation );
   SetRotation( that.mRotation );
   SetScale( that.mScale );
}

Transform::~Transform()
{
}

void Transform::Set( float tx, float ty, float tz, float h, float p, float r, float sx, float sy, float sz )
{
   osg::Vec3 xyz( tx, ty, tz );
   osg::Vec3 hpr( h, p, r );
   osg::Vec3 scale( sx, sy, sz );

   osg::Matrix rotation;
   dtUtil::MatrixUtil::HprToMatrix( rotation, hpr );

   Set( xyz, rotation, scale );
}

void Transform::Set( const osg::Vec3& xyz, const osg::Matrix& rotation, const osg::Vec3& scale )
{
   SetTranslation( xyz );
   SetRotation( rotation );
   SetScale( scale );
}

void Transform::Set(const osg::Vec3& xyz, const osg::Vec3& hpr, const osg::Vec3& scale)
{
   osg::Matrix rotation;
   dtUtil::MatrixUtil::HprToMatrix( rotation, hpr );

   Set( xyz, rotation, scale );
}

void Transform::Set( const osg::Matrix& mat )
{
   osg::Matrix rotation, scale;
   osg::Vec3 translation;

   dtUtil::PolarDecomp::Decompose( mat, rotation, scale, translation );

   SetTranslation( translation );
   SetRotation( rotation );
   mScale.set( scale(0,0), scale(1,1), scale(2,2) );   

}

void Transform::Set( sgMat4 mat )
{
   DEPRECATE(  "void Transform::Set( sgMat4 mat )",
               "void Transform::Set( const osg::Matrix& mat )" )

   osg::Matrix tempMatrix;

   for( int i = 0; i< 4; i++ )
      for( int j = 0; j < 4; j++ )
         tempMatrix(i,j) = mat[i][j];

   Set( tempMatrix );
}

void Transform::Set( float x, float y, float z, float h, float p, float r )
{
   DEPRECATE(  "void Transform::Set( float x, float y, float z, float h, float p, float r )",
               "void Transform::Set( float tx, float ty, float tz, float h, float p, float r, float sx, float sy, float sz )" )

   Set( x, y, z, h, p, r, mScale[0], mScale[1], mScale[2] );
}

void Transform::Set( sgVec3 xyz, sgVec3 hpr )
{
   DEPRECATE(  "void Transform::Set( sgVec3 xyz, sgVec3 hpr )",
               "void Transform::Set( const osg::Vec3& xyz, const osg::Matrix& rotation, const osg::Vec3& scale )" )

   Set( xyz[0], xyz[1], xyz[2], hpr[0], hpr[1], hpr[2] , mScale[0], mScale[1], mScale[2] );
}

void Transform::SetTranslation( sgVec3 xyz )
{ 
   DEPRECATE(  "void Transform::SetTranslation( sgVec3 xyz )",
               "void Transform::SetTranslation( const osg::Vec3& xyz )" )

   mTranslation.set( xyz[0], xyz[1], xyz[2] );
}

void Transform::SetRotation( float h, float p, float r )
{
   SetRotation( osg::Vec3( h, p, r ) );
}

void Transform::SetRotation( const osg::Vec3& hpr )
{
   osg::Matrix rotation;

   dtUtil::MatrixUtil::HprToMatrix( rotation, hpr );

   SetRotation( rotation );
}

void Transform::SetRotation( sgVec3 hpr )
{
   DEPRECATE(  "void Transform::SetRotation( sgVec3 hpr )",
               "void Transform::SetRotation( const osg::Vec3& hpr )" )

   SetRotation( hpr[0], hpr[1], hpr[2] );
}

void Transform::SetRotation( sgMat4 rot )
{
   DEPRECATE(  "void Transform::SetRotation( sgMat4 rot )",
               "void Transform::SetRotation( const osg::Matrix& rot )" )

   osg::Matrix tempMatrix;

   for( int i = 0; i< 3; i++ )
      for( int j = 0; j < 3; j++ )
         tempMatrix(i,j) = rot[i][j];

   tempMatrix(3,0) = 0.0f;
   tempMatrix(3,1) = 0.0f;
   tempMatrix(3,2) = 0.0f;
   tempMatrix(3,3) = 1.0f;

   SetRotation( tempMatrix );
}

void Transform::Get( float& tx, float& ty, float& tz, float& h, float& p, float& r, float &sx, float &sy, float &sz ) const
{
   tx = mTranslation[0];
   ty = mTranslation[1];
   tz = mTranslation[2];

   GetRotation( h, p, r );

   sx = mScale[0];
   sy = mScale[1];
   sz = mScale[2];
}

void Transform::Get( osg::Vec3& xyz, osg::Matrix& rotation, osg::Vec3& scale ) const
{
   xyz.set( mTranslation );
   rotation.set( mRotation );
   scale.set( mScale );
}

void Transform::Get( osg::Matrix& matrix ) const
{
   osg::Matrix fullMatrix = mRotation * osg::Matrix::scale( mScale ) * osg::Matrix::translate( mTranslation );

   matrix.set(fullMatrix);
}

void Transform::Get( osg::Vec3& xyz, osg::Vec3& hpr, osg::Vec3& scale ) const
{
   xyz.set(mTranslation);
   GetRotation(hpr);
   scale.set(mScale);
}

void Transform::Get( sgVec3 xyz, sgVec3 hpr )
{
   DEPRECATE(  "void Transform::Get( sgVec3 xyz, sgVec3 hpr )",
               "void Transform::Get( osg::Vec3& xyz, osg::Vec3& hpr, osg::Vec3& scale )" )

   osg::Vec3 osgHpr;
   GetRotation( osgHpr );

   for( int i = 0; i< 3; i++ )
   {
      xyz[i] = mTranslation[i];
      hpr[i] = osgHpr[i];
   }
}     

void Transform::Get( sgMat4 mat )
{
   DEPRECATE(  "void Transform::Get( sgMat4 mat )",
               "void Transform::Get( osg::Matrix& matrix ) " )

   osg::Matrix fullMatrix;
   Get( fullMatrix );

   for( int i = 0; i< 4; i++ )
      for( int j = 0; j < 4; j++ )
         mat[i][j] = fullMatrix(i,j);
}     

void Transform::Get( float *x, float *y, float *z, float *h, float *p, float *r )
{
   DEPRECATE(  "void Transform::Get( float *x, float *y, float *z, float *h, float *p, float *r )",
               "void Transform::Get( float& tx, float&t y, float& tz, float& h, float& p, float& r, float& sx, float& sy, float &sz )" )

   float sx, sy, sz;
   Get( *x, *y, *z, *h, *p, *r, sx, sy, sz );
}

void Transform::GetTranslation( float& tx, float& ty, float& tz ) const
{
   tx = mTranslation[0];
   ty = mTranslation[1];
   tz = mTranslation[2];
}


void Transform::GetTranslation( sgVec3 xyz )
{ 
   DEPRECATE(  "void Transform::GetTranslation( sgVec3 xyz )",
               "void Transform::GetTranslation( osg::Vec3& translation ) " )

   for( int i = 0; i < 3; i++ )
      xyz[i] = mTranslation[i];
}

void Transform::GetTranslation( float *x, float *y, float *z )
{
   DEPRECATE(  "void Transform::GetTranslation( float *x, float *y, float *z )",
               "void Transform::GetTranslation( float& x, float& y, float& z )" )

   GetTranslation( *x, *y, *z );
}

void Transform::GetRotation( float& h, float& p, float& r ) const
{
   osg::Vec3 hpr;
   dtUtil::MatrixUtil::MatrixToHpr( hpr, mRotation );

   h = hpr[0];
   p = hpr[1];
   r = hpr[2];
}

void Transform::GetRotation( osg::Vec3& hpr ) const
{
   dtUtil::MatrixUtil::MatrixToHpr( hpr, mRotation );
}

void Transform::GetRotation( float *h, float *p, float *r )
{
   DEPRECATE(  "void Transform::GetRotation( float *h, float *p, float *r )",
               "void Transform::GetRotation( float& h, float& p, float& r )" )

   GetRotation( *h, *p, *r );
}

void Transform::GetRotation( sgVec3 hpr )
{ 
   DEPRECATE(  "void Transform::GetRotation( sgVec3 hpr )",
               "void Transform::GetRotation( osg::Vec3& rotation )" )

   osg::Vec3 osgHpr;
   dtUtil::MatrixUtil::MatrixToHpr( osgHpr, mRotation );
   for( int i = 0; i < 3; i++ )
      hpr[i] = osgHpr[i];
}

void Transform::GetRotation( sgMat4 rot )
{
   DEPRECATE(  "void Transform::GetRotation( sgMat4 rot )",
               "void Transform::GetRotation( osg::Matrix& matrix )" )

   for( int i = 0; i< 4; i++ )
      for( int j = 0; j < 4; j++ )
         rot[i][j] = mRotation(i,j);
}

void Transform::GetScale( float& sx, float& sy, float& sz ) const
{
   sx = mScale[0];
   sy = mScale[1];
   sz = mScale[2];
}

void Transform::SetLookAt( const osg::Vec3& xyz, const osg::Vec3& lookAtXYZ, const osg::Vec3& upVec )
{
   /*
   osg::Matrix lookAtMatrix = osg::Matrix::lookAt(xyz, lookAtXYZ, upVec);

   dtUtil::MatrixUtil::SetColumn( lookAtMatrix, osg::Vec4( 0.0f, 0.0f, 0.0f, 1.0f ), 3 ); 

   SetRotation( lookAtMatrix );
   SetTranslation( xyz );
   */

   sgMat4 sgMat;
   sgVec3 sgXyz, sgLookAtXyz, sgUpVec;

   for( int i = 0; i < 3; i++ )
   {
      sgXyz[i] = xyz[i];
      sgLookAtXyz[i] = lookAtXYZ[i];
      sgUpVec[i] = upVec[i];
   }

   sgMakeLookAtMat4( sgMat, sgXyz, sgLookAtXyz, sgUpVec );
   
   osg::Matrix lookAtMatrix;

   for( int i = 0; i< 3; i++ )
      for( int j = 0; j < 3; j++ )
         lookAtMatrix(i,j) = sgMat[i][j];

   SetRotation( lookAtMatrix );
   SetTranslation( xyz );

}

void Transform::SetLookAt( sgVec3 xyz, sgVec3 lookAtXYZ, sgVec3 upVec )
{
   DEPRECATE(  "void Transform::SetLookAt( const osg::Vec3& xyz, const osg::Vec3& lookAtXYZ, const osg::Vec3& upVec )",
               "void Transform::SetLookAt( sgVec3 xyz, sgVec3 lookAtXYZ, sgVec3 upVec )" )

   SetLookAt(  osg::Vec3( xyz[0], xyz[1], xyz[2] ), 
               osg::Vec3( lookAtXYZ[0], lookAtXYZ[1], lookAtXYZ[2] ), 
               osg::Vec3( upVec[0], upVec[1], upVec[2] ) );
}

void Transform::SetLookAt(float posX, float posY, float posZ,
                          float lookAtX, float lookAtY, float lookAtZ,
                          float upVecX, float upVecY, float upVecZ)
{
   osg::Vec3 xyz( posX, posY, posZ );
   osg::Vec3 lookAt( lookAtX, lookAtY, lookAtZ );
   osg::Vec3 upVec( upVecX, upVecY, upVecZ );

   SetLookAt( xyz, lookAt, upVec );
}

bool Transform::EpsilonEquals( const Transform* transform, float epsilon ) const
{
   osg::Matrix thisMatrix, thatMatrix;

   Get( thisMatrix );
   transform->Get( thatMatrix );

   for( int i = 0; i < 4; i++ )
      for( int j = 0; j < 4; j++ )
         if( fabs( thisMatrix(i,j) - thatMatrix(i,j) ) > epsilon)
            return false;

   return true;
}

Transform & Transform::operator=(const Transform & rhs)
{
   if (this == &rhs) return *this;

   SetTranslation( rhs.mTranslation );
   SetRotation( rhs.mRotation );
   SetScale( rhs.mScale );
   
   return *this;
}

bool Transform::operator==(const Transform & rhs)
{
   if (this == &rhs) return true;

   return EpsilonEquals( &rhs, 0.0f );
}
