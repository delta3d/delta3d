
#if !defined(AFX_TRANSFORM_H__C6D53150_DAEC_4E42_BF12_C588CB62C637__INCLUDED_)
#define AFX_TRANSFORM_H__C6D53150_DAEC_4E42_BF12_C588CB62C637__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sg.h"

namespace dtCore
{
   ///A class that represents a position and attitude in 3D space
   
   class Transform
   {
   public:
      
      Transform(float x=0, float y=0, float z=0, float h=0, float p=0, float r=0);
      Transform(const Transform& that);
      virtual ~Transform();

      //Set both translation and rotation methods
      virtual void Set (sgMat4 mat) {sgCopyMat4(mTransform, mat);}
      virtual void Set( float x, float y, float z, float h, float p, float r); 
      virtual void Set(sgVec3 xyz, sgVec3 hpr ) {sgMakeCoordMat4(mTransform, xyz, hpr);}

      void SetLookAt( sgVec3 xyz, sgVec3 lookAtXYZ, sgVec3 upVec );
      
      //Set only translation methods
      virtual void SetTranslation( float x, float y, float z);
      virtual void SetTranslation( sgVec3 xyz ) {sgCopyVec3(mTransform[3], xyz);}

      //Set only rotation methods
      virtual void SetRotation( sgVec3 hpr );
      virtual void SetRotation( float h, float p, float r);    
      virtual void SetRotation( sgMat4 rot);
      
      //Get translation and rotation methods
      void Get( sgVec3 xyz, sgVec3 hpr ) {Get(&xyz[0], &xyz[1], &xyz[2], &hpr[0], &hpr[1], &hpr[2]);}     
      void Get( sgMat4 mat) {sgCopyMat4(mat, mTransform); }     
      void Get( float *x, float *y, float *z, float *h, float *p, float *r);

      //Get only translation methods
      void GetTranslation( sgVec3 xyz ) { GetTranslation(&xyz[0], &xyz[1], &xyz[2]); }
      void GetTranslation(float *x, float *y, float *z);
      
      //Get only rotation methods
      void GetRotation( float *h, float *p, float *r);
      void GetRotation( sgVec3 hpr ) { GetRotation(&hpr[0], &hpr[1], &hpr[2] );}
      void GetRotation( sgMat4 rot );
      
      //Compares this transform to another within the given threshold
      bool EpsilonEquals(const Transform* transform, float epsilon = 0.0001f);

      Transform & operator=(const Transform &);
      bool        operator==(const Transform &);

   protected:
      sgMat4 mTransform;  ///<Internal storage of the position/rotation
   };
};


#endif // !defined(AFX_TRANSFORM_H__C6D53150_DAEC_4E42_BF12_C588CB62C637__INCLUDED_)
