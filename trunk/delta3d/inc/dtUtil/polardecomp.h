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

/*  Algorithm by Ken Shoemake, 1993 */

#ifndef DELTA_POLAR_DECOMP
#define DELTA_POLAR_DECOMP

#include <iostream>

#include "osg/Matrixf"
#include "dtCore/export.h"

namespace dtUtil
{

   class DT_EXPORT PolarDecomp
   {
   public:
      // Find Polar Decomposition of Matrix M: Q=Rotation, S=Scale/Stretch, T=Translation
      static float Decompose( const osg::Matrixf& M, osg::Matrixf& Q, osg::Matrixf& S, osg::Vec4f& T );

   private:

      // Copy nxn matrix A to C using "gets" for assignment
      static void MatCopyMinusEqual( osg::Matrixf& C, const osg::Matrixf& A );

      // Assign nxn matrix C the element-wise combination of A and B using "op"
      static void MatBinOpEqualPlus( osg::Matrixf& C, const float g1, const osg::Matrixf& A, const float g2, const osg::Matrixf& B );

      // Set MadjT to transpose of inverse of M times determinant of M
      static void AdjointTranspose( const osg::Matrixf& M, osg::Matrixf& MadjT );

      static float NormInf( const osg::Matrixf& M );
      static float NormOne( const osg::Matrixf& M );

      // Return index of column of M containing maximum abs entry, or -1 if M=0
      static int FindMaxCol( const osg::Matrixf& M );

      // Setup u for Household reflection to zero all v components but first
      static void MakeReflector( const osg::Vec3f& v, osg::Vec3f& u );

      // Apply Householder reflection represented by u to column vectors of M
      static void ReflectCols( osg::Matrixf& M, const osg::Vec3f& u );

      // Apply Householder reflection represented by u to row vectors of M
      static void ReflectRows( osg::Matrixf& M, const osg::Vec3f& u );

      // Compute either the 1 or infinity norm of M, depending on tpose
      static float MatNorm( const osg::Matrixf& M, const int tpose );

      // Find orthogonal factor Q of rank 1 (or less) M
      static void DoRank1( osg::Matrixf& M, osg::Matrixf& Q );

      // Find orthogonal factor Q of rank 2 (or less) M using adjoint transpose
      static void DoRank2( osg::Matrixf& M, const osg::Matrixf& MadjT, osg::Matrixf& Q );

   };
}
#endif // DELTA_POLAR_DECOMP
