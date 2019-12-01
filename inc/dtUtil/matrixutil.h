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

#ifndef DELTA_MATRIX_UTIL
#define DELTA_MATRIX_UTIL

#include <osg/Matrix>
#include <osg/Vec3>
#include <osg/Vec4>
#include <dtUtil/export.h>

namespace dtUtil
{
   /**
   * MatrixUtil is a utility class for operating on an osg::Matrix
   *
   */

   class DT_UTIL_EXPORT MatrixUtil
   {
   public:

      ///Prints a matrix
      static void Print( const osg::Matrix& matrix );
      ///Prints a vector
      static void Print( const osg::Vec3& vec );
      ///Prints a vector
      static void Print( const osg::Vec4& vec );

      ///clamps a float from -1 to 1
      static float ClampUnity( float x );

      ///transposes a matrix
      //@param dest: matrix to store result
      //@param src: matrix to transpose
      static void Transpose( osg::Matrix& dest, const osg::Matrix& src );

      /*
         NOTE:  We are ROW MAJOR so a column doesnt mean anything to us
         unfortunately the old implementation of Get/Set Row and Get/Set Column
         were reversed
         -I have commented out Get/Set Column to enforce ppl to change that call
         to Get/Set Row
      */

      //static osg::Vec3 GetColumn3( const osg::Matrix& matrix, int column );
      //static osg::Vec4 GetColumn4( const osg::Matrix& matrix, int column );

      static osg::Vec3 GetRow3( const osg::Matrix& matrix, int row );
      static osg::Vec4 GetRow4( const osg::Matrix& matrix, int row );

      //static void SetColumn( osg::Matrix& matrix, const osg::Vec3& vec, int column );
      //static void SetColumn( osg::Matrix& matrix, const osg::Vec4& vec, int column );

      static void SetRow( osg::Matrix& matrix, const osg::Vec3& vec, int row );
      static void SetRow( osg::Matrix& matrix, const osg::Vec4& vec, int row );

      /**
      * Translates Euler angles Heading, pitch, and roll to an osg::Matrix
      * @param rotation: the matrix whose rotation will be filled
      * @param hpr: the current Heading, pitch, roll to translate to a matrix
      *             (values are in degrees)
      * @param zeroTranslation optional parameter, if set to false, the translation in the matrix will be left as is.
      */
      static void HprToMatrix( osg::Matrix& rotation, const osg::Vec3& hpr, bool zeroTranslation = true);

      /**
      * Translates Euler angles Heading, pitch, and roll to an osg::Matrix
      * in addition fills in the translation of the matrix
      * @param rotation: the matrix whose rotation will be filled
      * @param xyz: the translation to be added to the matrix
      * @param hpr: the current Heading, pitch, roll to translate to a matrix
      */
      static void PositionAndHprToMatrix( osg::Matrix& rotation, const osg::Vec3& xyz, const osg::Vec3& hpr );

      /**
      * Translates the rotation part of an osg::Matrix to Euler Angles
      * @param hpr: the vector to fill with the Euler Angles
      * @param rotation: the current rotation matrix
      */
      static void MatrixToHpr( osg::Vec3& hpr, const osg::Matrix& rotation );

      /**
      * Translates the rotation part of an osg::Matrix to Euler Angles
      * As well as the translation to a vector
      * @param xyz: the vector to fill with the translation part of the matrix
      * @param hpr: the vector to fill with the Euler Angles
      * @param rotation: the current rotation matrix
      */
      static void MatrixToHprAndPosition( osg::Vec3& xyz, osg::Vec3& hpr, const osg::Matrix& rotation );

      /**
      * This function transforms a point by a 4x4 matrix and stores the result back in the point
      *
      * @param xyz: the vector which will be transformed and have the result stored back in to
      * @param transformMat: the 4x4 matrix which will be used to rotate and translate the point
      */
      static void TransformVec3(osg::Vec3& xyz, const osg::Matrix& transformMat);

      /**
      * This function transforms a point by a 4x4 matrix and stores the result into another vector
      *
      * @param vec_in: the vector to store the result of the transform
      * @param xyz: the vector which will be transformed
      * @param transformMat: the 4x4 matrix which will be used to rotate and translate the point
      */
      static void TransformVec3(osg::Vec3& vec_in, const osg::Vec3& xyz, const osg::Matrix& transformMat);

   };
}
#endif // DELTA_MATRIX_UTIL
