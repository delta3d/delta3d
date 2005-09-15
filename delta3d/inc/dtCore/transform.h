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
#include "osg/Matrix"
#include "osg/Vec3"

namespace dtCore
{
   ////A class that represents a position and attitude in 3D space
   
   class DT_EXPORT Transform
   {
   public:
      
      Transform(  float tx = 0.0f, float ty = 0.0f, float tz = 0.0f, 
                  float h = 0.0f, float p = 0.0f, float r = 0.0f, 
                  float sx = 1.0f, float sy = 1.0f, float sz = 1.0f );
      Transform( const Transform& that );
      virtual ~Transform();

      ///Set both translation and rotation methods
      virtual void Set( float tx, float ty, float tz, 
                        float h, float p, float r, 
                        float sx, float sy, float sz ); 
      ///Set both translation and rotation methods
      virtual void Set( const osg::Vec3& xyz, const osg::Vec3& rotation, const osg::Vec3& scale );
      ///Set both translation and rotation methods
      virtual void Set( const osg::Vec3& xyz, const osg::Matrix& rotation, const osg::Vec3& scale );
      ///Set both translation and rotation methods
      virtual void Set( const osg::Matrix& mat );

      ///Set only translation methods
      virtual void SetTranslation( float tx, float ty, float tz ) { mTranslation.set( tx, ty, tz); }
      ///Set only translation methods
      virtual void SetTranslation( const osg::Vec3& xyz ) { mTranslation.set( xyz ); }
      
      //Set only rotation methods
      virtual void SetRotation( float h, float p, float r );
      //Set only rotation methods
      virtual void SetRotation( const osg::Vec3& hpr );
      //Set only rotation methods
      virtual void SetRotation( const osg::Matrix& rotation ) { mRotation.set( rotation ); }
      
      ///Set the uniform scale factor
      virtual void SetScale( float sx, float sy, float sz ) { mScale.set( sx, sy, sz ); }
      ///Set the uniform scale factor
      virtual void SetScale( const osg::Vec3& scale ) { mScale.set( scale ); }
      
      ///Get translation and rotation methods
      void Get( float& tx, float& ty, float& tz, float& h, float& p, float& r, float& sx, float& sy, float& sz ) const;
      ///Get translation and rotation methods
      void Get( osg::Vec3& xyz, osg::Matrix& rotation, osg::Vec3& scale ) const;
      ///Get translation and rotation methods
      void Get( osg::Vec3& xyz, osg::Vec3& hpr, osg::Vec3& scale  ) const;
      ///Get translation and rotation methods
      void Get( osg::Matrix& matrix ) const; 
      
      ///Get only translation methods
      void GetTranslation( osg::Vec3& translation ) const { translation.set( mTranslation ); }
      ///Get only translation methods
      void GetTranslation( float& tx, float& ty, float& tz ) const;

      float GetTranslationX() const { return mTranslation[0]; }
      float GetTranslationY() const { return mTranslation[0]; }
      float GetTranslationZ() const { return mTranslation[0]; }
    
      ///Get only rotation methods
      void GetRotation( float& h, float& p, float& r ) const;
      ///Get only rotation methods
      void GetRotation( osg::Vec3& hpr ) const;
      ///Get only rotation methods
      void GetRotation( osg::Matrix& rotation ) const { rotation.set( mRotation ); }

      void GetScale( float& sx, float& sy, float& sz ) const;
      void GetScale( osg::Vec3& scale ) const { scale.set( mScale ); }

      void SetLookAt( const osg::Vec3& xyz, const osg::Vec3& lookAtXyz, const osg::Vec3& upVec );      
      void SetLookAt(   float posX, float posY, float posZ,
                        float lookAtX, float lookAtY, float lookAtZ,
                        float upVecX, float upVecY, float upVecZ );

      bool EpsilonEquals( const Transform* transform, float epsilon = 0.0001f ) const;

      Transform & operator=(const Transform &);
      bool        operator==(const Transform &);

   protected:

      osg::Vec3 mTranslation; ///<Internal storage of translation
      osg::Matrix mRotation; ///<Internal storage of the rotation
      osg::Vec3 mScale; ///<Internal storage of scale
      
   };
};



#endif // DELTA_TRANSFORM
