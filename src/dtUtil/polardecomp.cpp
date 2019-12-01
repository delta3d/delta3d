#include <prefix/dtutilprefix.h>
#include <dtUtil/polardecomp.h>
#include <dtUtil/matrixutil.h>
#include <cmath>

using namespace dtUtil;

////////////////////////////////////////////////////////////////////////////////
// Copy nxn matrix A to C using "gets" for assignment
void PolarDecomp::MatCopyMinusEqual(osg::Matrix& C, const osg::Matrix& A)
{
   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         C(i,j) -= A(i,j);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
// Assign nxn matrix C the element-wise combination of A and B using "op"
void PolarDecomp::MatBinOpEqualPlus(osg::Matrix& C, const float g1, const osg::Matrix& A, const float g2, const osg::Matrix& B)
{
   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         C(i, j) = g1 * A(i, j) + g2 * B(i, j);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
// Set MadjT to transpose of inverse of M times determinant of M
void PolarDecomp::AdjointTranspose(const osg::Matrix& M, osg::Matrix& MadjT)
{
   MatrixUtil::SetRow(MadjT, MatrixUtil::GetRow3(M, 1)^MatrixUtil::GetRow3(M, 2), 0);
   MatrixUtil::SetRow(MadjT, MatrixUtil::GetRow3(M, 2)^MatrixUtil::GetRow3(M, 0), 1);
   MatrixUtil::SetRow(MadjT, MatrixUtil::GetRow3(M, 0)^MatrixUtil::GetRow3(M, 1), 2);
}

////////////////////////////////////////////////////////////////////////////////
float PolarDecomp::NormInf(const osg::Matrix& M)
{
   return MatNorm(M, 0);
}

////////////////////////////////////////////////////////////////////////////////
float PolarDecomp::NormOne(const osg::Matrix& M)
{
   return MatNorm(M, 1);
}

////////////////////////////////////////////////////////////////////////////////
// Return index of column of M containing maximum abs entry, or -1 if M=0
int PolarDecomp::FindMaxCol(const osg::Matrix& M)
{
   float abs;

   float max = 0.0;
   int col = -1;

   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         abs = M(i,j);

         if (abs < 0.0)
         {
            abs = -abs;
         }

         if (abs > max)
         {
            max = abs;
            col = j;
         }
      }
   }

   return col;
}

////////////////////////////////////////////////////////////////////////////////
// Setup u for Household reflection to zero all v components but first
void PolarDecomp::MakeReflector(const osg::Vec3& v, osg::Vec3& u)
{
   float s = sqrt(v * v);

   u[0] = v[0];
   u[1] = v[1];
   u[2] = v[2] + ((v[2] < 0.0) ? -s : s);

   s = sqrt(2.0 / (u * u));

   u[0] = u[0] * s;
   u[1] = u[1] * s;
   u[2] = u[2] * s;
}

////////////////////////////////////////////////////////////////////////////////
// Apply Householder reflection represented by u to column vectors of M
void PolarDecomp::ReflectCols(osg::Matrix& M, const osg::Vec3& u)
{
   for (int i = 0; i < 3; ++i)
   {
      float s = u[0] * M(0,i) + u[1] * M(1,i) + u[2] * M(2,i);

      for (int j = 0; j < 3; ++j)
      {
         M(j,i) -= u[j] * s;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
// Apply Householder reflection represented by u to row vectors of M
void PolarDecomp::ReflectRows(osg::Matrix& M, const osg::Vec3& u)
{
   for (int i = 0; i < 3; ++i)
   {
      float s = u * MatrixUtil::GetRow3(M,i);

      for (int j = 0; j < 3; ++j)
      {
         M(i,j) -= u[j] * s;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
// Compute either the 1 or infinity norm of M, depending on tpose
float PolarDecomp::MatNorm(const osg::Matrix& M, const int tpose)
{
   float sum;

   float max = 0.0;

   for (int i = 0; i < 3; ++i)
   {
      if (tpose)
      {
         sum = std::abs(M(0, i)) + std::abs(M(1, i)) + std::abs(M(2, i));
      }
      else
      {
         sum = std::abs(M(i, 0))+std::abs(M(i, 1))+std::abs(M(i, 2));
      }

      if (max < sum)
      {
         max = sum;
      }
   }

   return max;
}

////////////////////////////////////////////////////////////////////////////////
// Find orthogonal factor Q of rank 1 (or less) M
void PolarDecomp::DoRank1(osg::Matrix& M, osg::Matrix& Q)
{
   Q.makeIdentity();

   // If rank(M) is 1, we should find a non-zero column in M
   int col = FindMaxCol(M);

   if (col < 0)
   {
      return; // Rank is 0
   }

   osg::Vec3 v1 = MatrixUtil::GetRow3(M, col);

   MakeReflector(v1, v1);
   ReflectCols(M, v1);

   osg::Vec3 v2 = MatrixUtil::GetRow3(M, 2);

   MakeReflector(v2, v2);
   ReflectRows(M, v2);

   float s = M(2,2);

   if (s < 0.0)
   {
      Q(2,2) = -1.0;
   }

   ReflectCols(Q, v1);
   ReflectRows(Q, v2);
}

////////////////////////////////////////////////////////////////////////////////
// Find orthogonal factor Q of rank 2 (or less) M using adjoint transpose
void PolarDecomp::DoRank2(osg::Matrix& M, const osg::Matrix& MadjT, osg::Matrix& Q)
{
   // If rank(M) is 2, we should find a non-zero column in MadjT
   int col = FindMaxCol(MadjT);

   if (col < 0)
   {
      DoRank1(M, Q);
      return;
   } // Rank < 2

   osg::Vec3 v1 = MatrixUtil::GetRow3(MadjT, col);

   MakeReflector(v1, v1);
   ReflectCols(M, v1);

   osg::Vec3 v2 = MatrixUtil::GetRow3(M, 0) ^ MatrixUtil::GetRow3(M, 1);

   MakeReflector(v2, v2);
   ReflectRows(M, v2);

   float w = M(0, 0);
   float x = M(0, 1);
   float y = M(1, 0);
   float z = M(1, 1);

   float c, s, d;

   if (w * z > x * y)
   {
      c = z + w;
      s = y - x;
      d = sqrt(c * c + s * s);
      c = c / d;
      s = s / d;

      Q(0, 0) = Q(1, 1) = c;
      Q(0, 1) = -(Q(1, 0) = s);
   }
   else
   {
      c = z - w;
      s = y + x;
      d = sqrt(c * c + s * s);
      c = c / d;
      s = s / d;

      Q(0, 0) = -(Q(1, 1) = c);
      Q(0, 1) = Q(1, 0) = s;
   }

   Q(0, 2) = Q(2, 0) = Q(1, 2) = Q(2, 1) = 0.0;
   Q(2, 2) = 1.0;

   ReflectCols(Q, v1);
   ReflectRows(Q, v2);
}

////////////////////////////////////////////////////////////////////////////////
float PolarDecomp::Decompose(const osg::Matrix& M, osg::Matrix& Q, osg::Matrix& S, osg::Vec3& T)
{
   const float TOL = 1.0e-6f;

   // return and remove translation
   T = MatrixUtil::GetRow3(M, 3);
   osg::Matrix noTransM = M;

   osg::Vec4 newCol = osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
   MatrixUtil::SetRow(noTransM, newCol, 3);

   osg::Matrix Mk;
   MatrixUtil::Transpose(Mk, noTransM);

   float M_one = NormOne(Mk);
   float M_inf = NormInf(Mk);

   osg::Matrix MadjTk, Ek;
   float det, MadjT_one, MadjT_inf, E_one, gamma, g1, g2;

   do
   {
      AdjointTranspose(Mk, MadjTk);

      det = MatrixUtil::GetRow3(Mk, 0) * MatrixUtil::GetRow3(MadjTk, 0);

      if (det == 0.0)
      {
         DoRank2(Mk, MadjTk, Mk);
         break;
      }

      MadjT_one = NormOne(MadjTk);
      MadjT_inf = NormInf(MadjTk);

      gamma = sqrt(sqrt((MadjT_one * MadjT_inf) / (M_one * M_inf)) / std::abs(det));
      g1 = gamma * 0.5;
      g2 = 0.5 / (gamma * det);

      Ek = Mk;

      MatBinOpEqualPlus(Mk, g1, Mk, g2, MadjTk);

      MatCopyMinusEqual(Ek, Mk);

      E_one = NormOne(Ek);

      M_one = NormOne(Mk);
      M_inf = NormInf(Mk);

   } while (E_one > (M_one * TOL));

   MatrixUtil::Transpose(Q, Mk);

   S = Mk * noTransM;

   for (int i = 0; i < 3; ++i)
   {
      for (int j = i; j < 3; ++j)
      {
         S(i, j) = S(j, i) = 0.5 * (S(i, j) + S(j, i));
      }
   }

   return det;
}
