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

#ifndef DELTA_MATRIX_UTIL
#define DELTA_MATRIX_UTIL

#include <iostream>

#include "osg/Matrixf"
#include "dtCore/export.h"

namespace dtUtil
{

   class DT_EXPORT MatrixUtil
   {
   public:

      static void Print( const osg::Matrixf& matrix );  
      static void Print( const osg::Vec3f& vec );
      static void Print( const osg::Vec4f& vec );

      static void Transpose( osg::Matrixf& dest, const osg::Matrixf& src );

      static inline osg::Vec3f GetColumn3( const osg::Matrixf& matrix, const int column )
      {
         return osg::Vec3f( matrix(column,0), matrix(column,1), matrix(column,2) );
      }
      static inline osg::Vec3f GetRow3( const osg::Matrixf& matrix, const int row )
      {
         return osg::Vec3f( matrix(0,row), matrix(1,row), matrix(2,row) );
      }
      
      static inline osg::Vec4f GetColumn4( const osg::Matrixf& matrix, const int column )
      {
         return osg::Vec4f( matrix(column,0), matrix(column,1), matrix(column,2), matrix(column,3) );
      }
      static inline osg::Vec4f GetRow4( const osg::Matrixf& matrix, const int row )
      {
         return osg::Vec4f( matrix(0,row), matrix(1,row), matrix(2,row), matrix(3,row) );
      }
      
      static void SetColumn3( osg::Matrixf& matrix, const osg::Vec3f& vec, const int column );
      static void SetRow3( osg::Matrixf& matrix, const osg::Vec3f& vec, const int row );
      
      static void SetColumn4( osg::Matrixf& matrix, const osg::Vec4f& vec, const int column );
      static void SetRow4( osg::Matrixf& matrix, const osg::Vec4f& vec, const int row );
     

   };
}
#endif // DELTA_MATRIX_UTIL
