
#include "transform.h"

using namespace dtCore;


Transform::Transform(float x, float y, float z, float h, float p, float r)
{
   Set(x,y,z,h,p,r);
}


Transform::Transform(const Transform& that)
{
    sgCopyMat4(mTransform, that.mTransform);
}

Transform::~Transform()
{
}

void Transform::Set( float x, float y, float z, float h, float p, float r)
{
   sgMakeCoordMat4(mTransform, x, y, z, h, p, r);
}

void Transform::SetTranslation( float x, float y, float z)
{
   sgVec3 xyz = {x,y,z};
   SetTranslation(xyz);
}

void Transform::SetRotation( sgVec3 hpr )
{
   sgMakeCoordMat4(mTransform, mTransform[3], hpr );
}

void Transform::SetRotation( float h, float p, float r)
{
   sgVec3 hpr = {h, p, r};
   SetRotation(hpr);
}

void Transform::SetRotation( sgMat4 rot)
{
   sgVec3 xyz = {mTransform[3][0], mTransform[3][1], mTransform[3][2] };
   sgCopyMat4(mTransform, rot);
   SetTranslation(xyz);
}



void Transform::Get( float *x, float *y, float *z, float *h, float *p, float *r)
{
   sgCoord pos;
   sgSetCoord( &pos, mTransform);
   *x = pos.xyz[0];
   *y = pos.xyz[1];
   *z = pos.xyz[2];
   *h = pos.hpr[0];
   *p = pos.hpr[1];
   *r = pos.hpr[2];
}


void Transform::GetTranslation(float *x, float *y, float *z) 
{
   *x = mTransform[3][0];
   *y = mTransform[3][1];
   *z = mTransform[3][2];
}

float Transform::GetTranslationX() 
{
   return mTransform[3][0];
}

float Transform::GetTranslationY() 
{
   return mTransform[3][1];
}

float Transform::GetTranslationZ() 
{
   return mTransform[3][2];
}

void Transform::GetRotation(float *h, float *p, float *r )
{
   float x,y,z;
   Get(&x, &y, &z, h, p, r);
}

void Transform::GetRotation( sgMat4 rot )
{
   Get(rot);
   sgSetVec3(rot[3], 0.f, 0.f, 0.f);
}

bool Transform::EpsilonEquals(const Transform* transform, float epsilon)
{
   for(int i=0;i<4;i++)
   {
      for(int j=0;j<4;j++)
      {
         if( sgdAbs( mTransform[i][j] - transform->mTransform[i][j] ) > epsilon)
         {
            return false;
         }
      }
   }

   return true;
}

void Transform::SetLookAt( sgVec3 xyz, sgVec3 lookAtXYZ, sgVec3 upVec )
{
    //use sgMakeLookAtMat4() method
    sgMakeLookAtMat4( mTransform, xyz, lookAtXYZ, upVec );
}

void Transform::SetLookAt(float posX, float posY, float posZ,
                          float lookAtX, float lookAtY, float lookAtZ,
                          float upVecX, float upVecY, float upVecZ)
{
   sgVec3 xyz = { posX, posY, posZ },
          lookAt = { lookAtX, lookAtY, lookAtZ },
          upVec = { upVecX, upVecY, upVecZ };
          
   SetLookAt(xyz, lookAt, upVec);
}

Transform & Transform::operator=(const Transform & rhs)
{
   if (this == &rhs) return *this;

   sgCopyMat4(mTransform, rhs.mTransform);
   return *this;
}

bool Transform::operator==(const Transform & rhs)
{
   if (this == &rhs) return true;

   return   EpsilonEquals( &rhs, 0.0f );
}
