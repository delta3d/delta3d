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

#include "osg/Matrix"
#include "dtCore/export.h"

namespace dtUtil
{

   class DT_EXPORT MatrixUtil
   {
   public:

      static void Print( const osg::Matrix& matrix );  
      static void Print( const osg::Vec3f& vec );
      static void Print( const osg::Vec4f& vec );

      static void Transpose( osg::Matrix& dest, const osg::Matrix& src );

      static osg::Vec3f GetColumn3( const osg::Matrix& matrix, const int column );
      static osg::Vec4f GetColumn4( const osg::Matrix& matrix, const int column );

      static osg::Vec3f GetRow3( const osg::Matrix& matrix, const int row );      
      static osg::Vec4f GetRow4( const osg::Matrix& matrix, const int row );
      
      static void SetColumn( osg::Matrix& matrix, const osg::Vec3f& vec, const int column );
      static void SetColumn( osg::Matrix& matrix, const osg::Vec4f& vec, const int column );

      static void SetRow( osg::Matrix& matrix, const osg::Vec3f& vec, const int row );      
      static void SetRow( osg::Matrix& matrix, const osg::Vec4f& vec, const int row );
     
   };
}
#endif // DELTA_MATRIX_UTIL
