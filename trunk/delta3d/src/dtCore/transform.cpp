#include "dtCore/transform.h"
#include "dtCore/notify.h"
#include "dtUtil/matrixutil.h"
#include "dtUtil/polardecomp.h"

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

   Set( xyz, hpr, scale );
}

void Transform::Set( const osg::Vec3& xyz, const osg::Vec3& hpr, const osg::Vec3& scale )
{
   SetTranslation( xyz );
   SetRotation( hpr );
   SetScale( scale );
}

void Transform::Set( const osg::Matrix& mat )
{
   osg::Matrix rotation, scale;
   osg::Vec3 translation;

   dtUtil::PolarDecomp::Decompose( mat, rotation, scale, translation );

   mTranslation.set( translation );
   SetRotation( rotation );
   mScale.set( scale(0,0), scale(1,1), scale(2,2) );   

}

//DEPRECRATED: Use Set( const osg::Matrix& mat )
void Transform::Set( sgMat4 mat )
{
   osg::Matrix tempMatrix;

   for( int i = 0; i< 4; i++ )
      for( int j = 0; j < 4; j++ )
         tempMatrix(i,j) = mat[i][j];

   Set( tempMatrix );
}

//DEPRECRATED: Use Set( float tx, float ty, float tz, float h, float p, float r, float sx, float sy, float sz )
void Transform::Set( float x, float y, float z, float h, float p, float r )
{
   Set( x, y, z, h, p, r, mScale[0], mScale[1], mScale[2] );
}

//DEPRECRATED: Use Set( const osg::Vec3& xyz, const osg::Vec3& hpr, const osg::Vec3& scale )
void Transform::Set( sgVec3 xyz, sgVec3 hpr )
{
   Set( osg::Vec3( xyz[0], xyz[1], xyz[2] ), osg::Vec3( hpr[0], hpr[1], hpr[2] ), mScale );
}

//DEPRECRATED: Use SetTranslation( const osg::Vec3& xyz )
void Transform::SetTranslation( sgVec3 xyz )
{ 
   mTranslation.set( xyz[0], xyz[1], xyz[2] );
}

void Transform::SetRotation( const osg::Matrix& rotation )
{
   osg::Vec3 hpr;

   dtUtil::MatrixUtil::MatrixToHpr( hpr, rotation );

   mRotation.set( hpr );
}

//DEPRECRATED: Use SetRotation( const osg::Vec3& hpr )
void Transform::SetRotation( sgVec3 hpr )
{
   mRotation.set( hpr[0], hpr[1], hpr[2] );
}

//DEPRECRATED: Use SetRotation( const osg::Matrix& rot )
void Transform::SetRotation( sgMat4 rot )
{
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

   h = mRotation[0];
   p = mRotation[1];
   r = mRotation[2];

   sx = mScale[0];
   sy = mScale[1];
   sz = mScale[2];
}

void Transform::Get( osg::Vec3& xyz, osg::Vec3& hpr, osg::Vec3& scale ) const
{
   xyz.set( mTranslation );
   hpr.set( mRotation );
   scale.set( mScale );
}

void Transform::Get( osg::Matrix& matrix ) const
{
   osg::Matrix rotation;
   dtUtil::MatrixUtil::HprToMatrix( rotation, mRotation );

   osg::Matrix fullMatrix = rotation * osg::Matrix::scale( mScale ) * osg::Matrix::translate( mTranslation );

   matrix.set(fullMatrix);
}

//DEPRECRATED: Use Get( osg::Vec3& xyz, osg::Vec3& hpr, osg::Vec3& scale )
void Transform::Get( sgVec3 xyz, sgVec3 hpr )
{
   for( int i = 0; i< 3; i++ )
   {
      xyz[i] = mTranslation[i];
      hpr[i] = mRotation[i];
   }
}     

//DEPRECRATED: Use Get( osg::Matrix& matrix ) 
void Transform::Get( sgMat4 mat )
{
   osg::Matrix fullMatrix;
   Get( fullMatrix );

   for( int i = 0; i< 4; i++ )
      for( int j = 0; j < 4; j++ )
         mat[i][j] = fullMatrix(i,j);
}     

//DEPRECRATED: Use Get( float& x, float& y, float& z, float& h, float& p, float& r)
void Transform::Get( float *x, float *y, float *z, float *h, float *p, float *r )
{
   float sx, sy, sz;
   Get( *x, *y, *z, *h, *p, *r, sx, sy, sz );
}

void Transform::GetTranslation( float& tx, float& ty, float& tz ) const
{
   tx = mTranslation[0];
   ty = mTranslation[1];
   tz = mTranslation[2];
}

//DEPRECRATED: Use GetTranslation( osg::Vec3& translation ) 
void Transform::GetTranslation( sgVec3 xyz )
{ 
   for( int i = 0; i < 3; i++ )
      xyz[i] = mTranslation[i];
}

//DEPRECRATED: Use GetTranslation( float& x, float& y, float& z )
void Transform::GetTranslation( float *x, float *y, float *z )
{
   GetTranslation( *x, *y, *z );
}

void Transform::GetRotation( float& h, float& p, float& r ) const
{
   float tx, ty, tz, sx, sy, sz;
   Get( tx, ty, tz, h, p, r, sx, sy, sz );
}

void Transform::GetRotation( osg::Matrix& rotation ) const
{
   dtUtil::MatrixUtil::HprToMatrix( rotation, mRotation );
}

//DEPRECRATED: Use GetRotation( float& h, float& p, float& r )
void Transform::GetRotation( float *h, float *p, float *r )
{
   GetRotation( *h, *p, *r );
}

//DEPRECRATED: Use GetRotation( osg::Vec3& rotation )
void Transform::GetRotation( sgVec3 hpr )
{ 
   for( int i = 0; i < 3; i++ )
      hpr[i] = mRotation[i];
}

//DEPRECRATED: Use GetRotation( osg::Matrix& matrix )
void Transform::GetRotation( sgMat4 rot )
{
   osg::Matrix rotation;
   GetRotation( rotation );

   for( int i = 0; i< 4; i++ )
      for( int j = 0; j < 4; j++ )
         rot[i][j] = rotation(i,j);
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

//DEPPRECATED: Use SetLookAt( const osg::Vec3& xyz, const osg::Vec3& lookAtXYZ, const osg::Vec3& upVec )
void Transform::SetLookAt( sgVec3 xyz, sgVec3 lookAtXYZ, sgVec3 upVec )
{
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
