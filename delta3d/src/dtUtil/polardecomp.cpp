#include "dtUtil/polardecomp.h"

using namespace dtUtil;
using namespace osg;

void PolarDecomp::Print( Matrixf& matrix )
{
   for( int i = 0; i < 4; i++ )
   {
      for( int j = 0; j < 4; j++ )
      {
         std::cout << "[" << matrix(j,i) << "]";
      }
      std::cout << std::endl;
   }
}        

void PolarDecomp::Print( Vec3d& vec )
{
   for( int i = 0; i < 3; i++ )
   {
      std::cout << "[" << vec[i] << "]";
   }
   std::cout << std::endl;
}

/** Copy nxn matrix A to C using "gets" for assignment **/
void PolarDecomp::MatCopyMinusEqual( Matrixf& C, Matrixf& A )
{
   int i,j; 

   for( i = 0; i < 3; i++ ) 
      for( j = 0; j < 3; j++ )
         C(i,j) -= A(i,j);
}

//mat_binop(Mk,=,g1*Mk,+,g2*MadjTk,3);

/** Assign nxn matrix C the element-wise combination of A and B using "op" **/
void PolarDecomp::MatBinOpEqualPlus( osg::Matrixf& C, float g1, osg::Matrixf& A, float g2, osg::Matrixf& B )
{
   int i,j; 

   for( i = 0; i < 3; i++ ) 
      for( j = 0; j < 3; j++ )
         C(i,j) = g1*A(i,j) + g2*B(i,j);
}

void PolarDecomp::Transpose( Matrixf& dest, const Matrixf& src )
{
   for( int i = 0; i < 4; i++ )
      for( int j = 0; j < 4; j++ )
         dest(i,j) = src(j,i);
}

void PolarDecomp::SetRow( Matrixf& matrix, const Vec3f& vec, const int row )
{
   for( int i = 0; i < 3; i++ )
      matrix(row,i) = vec[i];
}

void PolarDecomp::SetColumn( Matrixf& matrix, const Vec3f& vec, const int column )
{
   for( int i = 0; i < 3; i++ )
      matrix(i,column) = vec[i];
}

/** Set MadjT to transpose of inverse of M times determinant of M **/
void PolarDecomp::AdjointTranspose( Matrixf& M, Matrixf& MadjT )
{
   SetRow( MadjT, GetRow(M,1)^GetRow(M,2), 0);
   SetRow( MadjT, GetRow(M,2)^GetRow(M,0), 1);
   SetRow( MadjT, GetRow(M,0)^GetRow(M,1), 2);
}

float PolarDecomp::NormInf( Matrixf& M )
{
   return MatNorm( M, 0 );
}
float PolarDecomp::NormOne( Matrixf& M ) 
{
   return MatNorm( M, 1 );
}

/** Return index of column of M containing maximum abs entry, or -1 if M=0 **/
int PolarDecomp::FindMaxCol( Matrixf& M )
{
   float abs, max;
   int i, j, col;

   max = 0.0;
   col = -1;

   for( i = 0; i < 3; i++ )
   {
      for( j = 0; j < 3; j++ )
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
void PolarDecomp::MakeReflector( Vec3f& v, Vec3f& u )
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
void PolarDecomp::ReflectCols( Matrixf& M, Vec3f& u )
{
   int i, j;

   for( i = 0; i < 3; i++ ) 
   {
      float s = u[0]*M(0,i) + u[1]*M(1,i) + u[2]*M(2,i);

      for( j = 0; j < 3; j++ ) 
         M(j,i) -= u[j]*s;
   }
}

/** Apply Householder reflection represented by u to row vectors of M **/
void PolarDecomp::ReflectRows( Matrixf& M, Vec3f& u )
{
   int i, j;

   for( i = 0; i < 3; i++ )
   {
      float s = u * GetRow(M,i); 

      for( j = 0; j < 3; j++ ) 
         M(i,j) -= u[j]*s;
   }
}

/** Compute either the 1 or infinity norm of M, depending on tpose **/
float PolarDecomp::MatNorm( Matrixf& M, int tpose )
{
   int i;
   float sum, max;

   max = 0.0;

   for( i = 0; i < 3; i++ ) {
      if(tpose) 
         sum = fabs( M(0,i) )+fabs( M(1,i) )+fabs( M(2,i) );
      else
         sum = fabs( M(i,0) )+fabs( M(i,1) )+fabs( M(i,2) );

      if(max < sum) 
         max = sum;
   }

   return max;
}

/** Find orthogonal factor Q of rank 1 (or less) M **/
void PolarDecomp::DoRank1( Matrixf& M, Matrixf& Q )
{
   Vec3f v1, v2; 
   float s;
   int col;

   Q.makeIdentity();

   /* If rank(M) is 1, we should find a non-zero column in M */
   col = FindMaxCol(M);

   if( col < 0 ) 
      return; /* Rank is 0 */

   v1[0] = M(0,col); 
   v1[1] = M(1,col); 
   v1[2] = M(2,col);

   MakeReflector(v1, v1); 
   ReflectCols(M, v1);

   v2[0] = M(2,0); 
   v2[1] = M(2,1); 
   v2[2] = M(2,2);

   MakeReflector(v2, v2); 
   ReflectRows(M, v2);

   s = M(2,2);

   if( s < 0.0 ) 
      Q(2,2) = -1.0;

   ReflectCols(Q, v1);
   ReflectRows(Q, v2);
}

/** Find orthogonal factor Q of rank 2 (or less) M using adjoint transpose **/
void PolarDecomp::DoRank2( Matrixf& M, Matrixf& MadjT, Matrixf& Q )
{
   Vec3f v1, v2;
   float w, x, y, z, c, s, d;
   int col;

   /* If rank(M) is 2, we should find a non-zero column in MadjT */
   col = FindMaxCol(MadjT);

   if( col < 0 )
   {
      DoRank1(M, Q); 
      return;
   } /* Rank<2 */

   v1[0] = MadjT(0,col); 
   v1[1] = MadjT(1,col); 
   v1[2] = MadjT(2,col);

   MakeReflector(v1, v1); 
   ReflectCols(M, v1);

   v2 = GetRow(M,0) ^ GetRow(M,1);

   MakeReflector(v2, v2); 
   ReflectRows(M, v2);

   w = M(0,0); 
   x = M(0,1); 
   y = M(1,0); 
   z = M(1,1);

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


float PolarDecomp::Decompose( Matrixf& M, Matrixf& Q, Matrixf& S )
{
   #define TOL 1.0e-6

   Matrixf Mk, MadjTk, Ek;
   float det, M_one, M_inf, MadjT_one, MadjT_inf, E_one, gamma, g1, g2;
   int i, j;

   Transpose( Mk, M );

   M_one = NormOne( Mk );  
   M_inf = NormInf( Mk );

   do
   {
      AdjointTranspose( Mk, MadjTk );

      det = GetRow(Mk,0) * GetRow(MadjTk,0);

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

   Transpose( Q, Mk );

   // Q: set last row/column to 0s, 1 in corner?

   S = Mk * M;	 

   // S: set last row/column to 0s, 1 in corner?

   for( i = 0; i < 3; i++ ) 
      for( j = i; j < 3; j++ )
         S(i,j) = S(j,i) = 0.5*( S(i,j) + S(j,i) );

   return( det );
}


void PolarDecomp::Test()
{

   Matrixf iden = Matrixf();

   Matrixf scale = Matrixf();
   scale.makeScale( 3.0f, 3.0f, 3.0f );

   Matrixf rotation = Matrixf();
   rotation.makeRotate( 90.0f, 1.0f, 0.0f, 0.0f );

   Matrixf translation = Matrixf();
   translation.makeTranslate( 2.0f, 2.0f, 2.0f );

   //Matrixf comp = iden * scale;
   //Matrixf comp = iden * rotation;
   //Matrixf comp = iden * translation;
   //Matrixf comp = iden * scale * rotation;
   Matrixf comp = iden * scale * rotation * translation;

   std::cout << "Composition:" << std::endl;
   Print( comp );
   std::cout << std::endl;

   Vec3d scaleVec = comp.getScale();
   Print( scaleVec );
}
