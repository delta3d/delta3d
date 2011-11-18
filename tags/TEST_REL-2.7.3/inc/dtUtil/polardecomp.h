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

/*  Algorithm by Ken Shoemake, 1993 */

#ifndef DELTA_POLAR_DECOMP
#define DELTA_POLAR_DECOMP

#include <osg/Matrix>
#include <osg/Vec3>
#include <dtUtil/export.h>

namespace dtUtil
{
   /**
   * PolarDecomp is a class that will take a 4x4 Matrix and break it up into the following components
   * Rotation, Scale, and Translation
   */
   class DT_UTIL_EXPORT PolarDecomp
   {
   public:
      /// Find Polar Decomposition of Matrix M: Q=Rotation, S=Scale/Stretch, T=Translation
      static float Decompose(const osg::Matrix& M, osg::Matrix& Q, osg::Matrix& S, osg::Vec3& T);

   private:

      /// Copy nxn matrix A to C using "gets" for assignment
      static void MatCopyMinusEqual(osg::Matrix& C, const osg::Matrix& A);

      /// Assign nxn matrix C the element-wise combination of A and B using "op"
      static void MatBinOpEqualPlus(osg::Matrix& C, const float g1, const osg::Matrix& A, const float g2, const osg::Matrix& B);

      /// Set MadjT to transpose of inverse of M times determinant of M
      static void AdjointTranspose(const osg::Matrix& M, osg::Matrix& MadjT);

      static float NormInf(const osg::Matrix& M);
      static float NormOne(const osg::Matrix& M);

      /// Return index of column of M containing maximum abs entry, or -1 if M=0
      static int FindMaxCol(const osg::Matrix& M);

      /// Setup u for Household reflection to zero all v components but first
      static void MakeReflector(const osg::Vec3& v, osg::Vec3& u);

      /// Apply Householder reflection represented by u to column vectors of M
      static void ReflectCols(osg::Matrix& M, const osg::Vec3& u);

      /// Apply Householder reflection represented by u to row vectors of M
      static void ReflectRows(osg::Matrix& M, const osg::Vec3& u);

      // Compute either the 1 or infinity norm of M, depending on tpose
      static float MatNorm(const osg::Matrix& M, const int tpose);

      /// Find orthogonal factor Q of rank 1 (or less) M
      static void DoRank1(osg::Matrix& M, osg::Matrix& Q);

      /// Find orthogonal factor Q of rank 2 (or less) M using adjoint transpose
      static void DoRank2(osg::Matrix& M, const osg::Matrix& MadjT, osg::Matrix& Q);
   };
}
#endif // DELTA_POLAR_DECOMP
