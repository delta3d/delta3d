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

#ifndef DELTA_TRANSFORM
#define DELTA_TRANSFORM




#include "dtCore/export.h"
#include "sg.h"

namespace dtCore
{
   ///A class that represents a position and attitude in 3D space
   
   class DT_EXPORT Transform
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
      
      void SetLookAt(float posX, float posY, float posZ,
                     float lookAtX, float lookAtY, float lookAtZ,
                     float upVecX, float upVecY, float upVecZ);
                     
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

      float GetTranslationX();
      float GetTranslationY();
      float GetTranslationZ();
      
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



#endif // DELTA_TRANSFORM