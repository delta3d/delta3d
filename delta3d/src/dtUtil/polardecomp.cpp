#include "dtUtil/polardecomp.h"
#include "dtUtil/matrixutil.h"

using namespace dtUtil;
using namespace osg;

/** Copy nxn matrix A to C using "gets" for assignment **/
void PolarDecomp::MatCopyMinusEqual( Matrixf& C, const Matrixf& A )
{
   for( int i = 0; i < 3; i++ ) 
      for( int j = 0; j < 3; j++ )
         C(i,j) -= A(i,j);
}

/** Assign nxn matrix C the element-wise combination of A and B using "op" **/
void PolarDecomp::MatBinOpEqualPlus( osg::Matrixf& C, const float g1, const osg::Matrixf& A, const float g2, const osg::Matrixf& B )
{
   for( int i = 0; i < 3; i++ ) 
      for( int j = 0; j < 3; j++ )
         C(i,j) = g1*A(i,j) + g2*B(i,j);
}

/** Set MadjT to transpose of inverse of M times determinant of M **/
void PolarDecomp::AdjointTranspose( const Matrixf& M, Matrixf& MadjT )
{
   MatrixUtil::SetColumn3( MadjT, MatrixUtil::GetColumn3(M,1)^MatrixUtil::GetColumn3(M,2), 0);
   MatrixUtil::SetColumn3( MadjT, MatrixUtil::GetColumn3(M,2)^MatrixUtil::GetColumn3(M,0), 1);
   MatrixUtil::SetColumn3( MadjT, MatrixUtil::GetColumn3(M,0)^MatrixUtil::GetColumn3(M,1), 2);
}

float PolarDecomp::NormInf( const Matrixf& M )
{
   return MatNorm( M, 0 );
}
float PolarDecomp::NormOne( const Matrixf& M ) 
{
   return MatNorm( M, 1 );
}

/** Return index of column of M containing maximum abs entry, or -1 if M=0 **/
int PolarDecomp::FindMaxCol( const Matrixf& M )
{
   float abs;
   
   float max = 0.0;
   int col = -1;

   for( int i = 0; i < 3; i++ )
   {
      for( int j = 0; j < 3; j++ )
      {
         abs = M(i,j); 

         if ( abs < 0.0 ) 
            abs = -abs;

         if ( abs > max )
         {  
            max = abs; 
            col = j;
         }
      }
   }

   return col;
}

/** Setup u for Household reflection to zero all v components but first **/
void PolarDecomp::MakeReflector( const Vec3f& v, Vec3f& u )
{
   float s = sqrt( v*v );

   u[0] = v[0]; 
   u[1] = v[1];
   u[2] = v[2] + ( ( v[2] < 0.0 ) ? -s : s );

   s = sqrt( 2.0 / (u*u) );

   u[0] = u[0]*s; 
   u[1] = u[1]*s; 
   u[2] = u[2]*s;
}

/** Apply Householder reflection represented by u to column vectors of M **/
void PolarDecomp::ReflectCols( Matrixf& M, const Vec3f& u )
{
   for( int i = 0; i < 3; i++ ) 
   {
      float s = u[0]*M(0,i) + u[1]*M(1,i) + u[2]*M(2,i);

      for( int j = 0; j < 3; j++ ) 
         M(j,i) -= u[j]*s;
   }
}

/** Apply Householder reflection represented by u to row vectors of M **/
void PolarDecomp::ReflectRows( Matrixf& M, const Vec3f& u )
{
   for( int i = 0; i < 3; i++ )
   {
      float s = u * MatrixUtil::GetColumn3(M,i); 

      for( int j = 0; j < 3; j++ ) 
         M(i,j) -= u[j]*s;
   }
}

/** Compute either the 1 or infinity norm of M, depending on tpose **/
float PolarDecomp::MatNorm( const Matrixf& M, const int tpose )
{
   float sum;

   float max = 0.0;

   for( int i = 0; i < 3; i++ ) {
      if( tpose ) 
         sum = fabs( M(0,i) )+fabs( M(1,i) )+fabs( M(2,i) );
      else
         sum = fabs( M(i,0) )+fabs( M(i,1) )+fabs( M(i,2) );

      if( max < sum ) 
         max = sum;
   }

   return max;
}

/** Find orthogonal factor Q of rank 1 (or less) M **/
void PolarDecomp::DoRank1( Matrixf& M, Matrixf& Q )
{
   Q.makeIdentity();

   /* If rank(M) is 1, we should find a non-zero column in M */
   int col = FindMaxCol(M);

   if( col < 0 ) 
      return; /* Rank is 0 */
      
   Vec3f v1 = MatrixUtil::GetRow3( M, col );

   MakeReflector(v1, v1); 
   ReflectCols(M, v1);

   Vec3f v2 = MatrixUtil::GetColumn3( M, 2 );
   
   MakeReflector(v2, v2); 
   ReflectRows(M, v2);

   float s = M(2,2);

   if( s < 0.0 ) 
      Q(2,2) = -1.0;

   ReflectCols(Q, v1);
   ReflectRows(Q, v2);
}

/** Find orthogonal factor Q of rank 2 (or less) M using adjoint transpose **/
void PolarDecomp::DoRank2( Matrixf& M, const Matrixf& MadjT, Matrixf& Q )
{
   /* If rank(M) is 2, we should find a non-zero column in MadjT */
   int col = FindMaxCol(MadjT);

   if( col < 0 )
   {
      DoRank1(M, Q); 
      return;
   } /* Rank<2 */

   Vec3f v1 = MatrixUtil::GetRow3( MadjT, col );
   
   MakeReflector(v1, v1); 
   ReflectCols(M, v1);

   Vec3f v2 = MatrixUtil::GetColumn3(M,0) ^ MatrixUtil::GetColumn3(M,1);

   MakeReflector(v2, v2); 
   ReflectRows(M, v2);

   float w = M(0,0); 
   float x = M(0,1); 
   float y = M(1,0); 
   float z = M(1,1);

   float c, s, d;
   
   if (w*z>x*y)
   {
      c = z+w; 
      s = y-x; 
      d = sqrt(c*c+s*s); 
      c = c/d; 
      s = s/d;

      Q(0,0) = Q(1,1) = c; 
      Q(0,1) = -( Q(1,0) = s );
   }
   else
   {
      c = z-w; 
      s = y+x; 
      d = sqrt(c*c+s*s); 
      c = c/d; 
      s = s/d;

      Q(0,0) = -( Q(1,1) = c ); 
      Q(0,1) = Q(1,0) = s;
   }

   Q(0,2) = Q(2,0) = Q(1,2) = Q(2,1) = 0.0; 
   Q(2,2) = 1.0;

   ReflectCols(Q, v1); 
   ReflectRows(Q, v2);
}


float PolarDecomp::Decompose( const osg::Matrixf& M, osg::Matrixf& Q, osg::Matrixf& S, osg::Vec4f& T )
{
   const float TOL = 1.0e-6;
   
   // return and remove translation
   T = MatrixUtil::GetColumn4( M, 3 );
   Matrixf noTransM = M;
   
   Vec4f newCol = Vec4f( 0.0f, 0.0f, 0.0f, 1.0f );
   MatrixUtil::SetColumn4( noTransM, newCol, 3 );
   //

   Matrixf Mk;
   MatrixUtil::Transpose( Mk, noTransM );

   float M_one = NormOne( Mk );  
   float M_inf = NormInf( Mk );
   
   Matrixf MadjTk, Ek;
   float det, MadjT_one, MadjT_inf, E_one, gamma, g1, g2;

   do
   {
      AdjointTranspose( Mk, MadjTk );

      det = MatrixUtil::GetColumn3(Mk,0) * MatrixUtil::GetColumn3(MadjTk,0);

      if ( det == 0.0 )
      {
         DoRank2(Mk, MadjTk, Mk); 
         break;
      }

      MadjT_one = NormOne( MadjTk ); 
      MadjT_inf = NormInf( MadjTk );

      gamma = sqrt( sqrt( (MadjT_one*MadjT_inf)/(M_one*M_inf) ) / fabs(det) );
      g1 = gamma*0.5;
      g2 = 0.5/(gamma*det);

      Ek = Mk;

      MatBinOpEqualPlus( Mk, g1, Mk, g2, MadjTk );

      MatCopyMinusEqual( Ek, Mk );

      E_one = NormOne(Ek);

      M_one = NormOne(Mk);  
      M_inf = NormInf(Mk);

   } while ( E_one > (M_one*TOL) );

   MatrixUtil::Transpose( Q, Mk );

   S = Mk * noTransM;

   for( int i = 0; i < 3; i++ ) 
      for( int j = i; j < 3; j++ )
         S(i,j) = S(j,i) = 0.5*( S(i,j) + S(j,i) );

   return( det );
}
