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

      void Print( osg::Matrixf& matrix );  
      void Print( osg::Vec3d& vec );

      /** Copy nxn matrix A to C using "gets" for assignment **/
      void MatCopyMinusEqual( osg::Matrixf& C, osg::Matrixf& A );

      /** Assign nxn matrix C the element-wise combination of A and B using "op" **/
      void MatBinOpEqualPlus( osg::Matrixf& C, float g1, osg::Matrixf& A, float g2, osg::Matrixf& B );

      void Transpose( osg::Matrixf& dest, const osg::Matrixf& src );

      inline osg::Vec3f GetRow( osg::Matrixf& matrix, int row )
      {
         return osg::Vec3f( matrix(row,0), matrix(row,1), matrix(row,2) );
      }

      inline osg::Vec3f GetColumn( osg::Matrixf& matrix, int column )
      {
         return osg::Vec3f( matrix(0,column), matrix(1,column), matrix(2,column) );
      }

      void SetRow( osg::Matrixf& matrix, const osg::Vec3f& vec, const int row );

      void SetColumn( osg::Matrixf& matrix, const osg::Vec3f& vec, const int column );

      /** Set MadjT to transpose of inverse of M times determinant of M **/
      void AdjointTranspose( osg::Matrixf& M, osg::Matrixf& MadjT );

      float NormInf( osg::Matrixf& M );
      float NormOne( osg::Matrixf& M );

      /** Return index of column of M containing maximum abs entry, or -1 if M=0 **/
      int FindMaxCol( osg::Matrixf& M );

      /** Setup u for Household reflection to zero all v components but first **/
      void MakeReflector( osg::Vec3f& v, osg::Vec3f& u );

      /** Apply Householder reflection represented by u to column vectors of M **/
      void ReflectCols( osg::Matrixf& M, osg::Vec3f& u );

      /** Apply Householder reflection represented by u to row vectors of M **/
      void ReflectRows( osg::Matrixf& M, osg::Vec3f& u );

      /** Compute either the 1 or infinity norm of M, depending on tpose **/
      float MatNorm( osg::Matrixf& M, int tpose );

      /** Find orthogonal factor Q of rank 1 (or less) M **/
      void DoRank1( osg::Matrixf& M, osg::Matrixf& Q );

      /** Find orthogonal factor Q of rank 2 (or less) M using adjoint transpose **/
      void DoRank2( osg::Matrixf& M, osg::Matrixf& MadjT, osg::Matrixf& Q );

      float Decompose( osg::Matrixf& M, osg::Matrixf& Q, osg::Matrixf& S );

      void Test();

   };
}
#endif // DELTA_POLAR_DECOMP
