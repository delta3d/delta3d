#include "dtUtil/matrixutil.h"
#include "sg.h"

using namespace dtUtil;
using namespace osg;

void MatrixUtil::Print( const Matrix& matrix )
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

void MatrixUtil::Print( const Vec3& vec )
{
   for( int i = 0; i < 3; i++ )
   {
      std::cout << "[" << vec[i] << "]";
   }
   std::cout << std::endl;
}

void MatrixUtil::Print( const Vec4& vec )
{
   for( int i = 0; i < 4; i++ )
   {
      std::cout << "[" << vec[i] << "]";
   }
   std::cout << std::endl;
}

void MatrixUtil::Transpose( Matrix& dest, const Matrix& src )
{
   for( int i = 0; i < 4; i++ )
      for( int j = 0; j < 4; j++ )
         dest(i,j) = src(j,i);
}

Vec3 MatrixUtil::GetColumn3( const Matrix& matrix, int column )
{
   return Vec3f( matrix(column,0), matrix(column,1), matrix(column,2) );
}

Vec4 MatrixUtil::GetColumn4( const Matrix& matrix, int column )
{
   return Vec4f( matrix(column,0), matrix(column,1), matrix(column,2), matrix(column,3) );
}

Vec3 MatrixUtil::GetRow3( const Matrix& matrix, int row )
{
   return Vec3f( matrix(0,row), matrix(1,row), matrix(2,row) );
}

Vec4 MatrixUtil::GetRow4( const Matrix& matrix, int row )
{
   return Vec4f( matrix(0,row), matrix(1,row), matrix(2,row), matrix(3,row) );
}

void MatrixUtil::SetColumn( Matrix& matrix, const Vec3& vec, int column )
{
   for( int i = 0; i < 3; i++ )
      matrix(column,i) = vec[i];
}

void MatrixUtil::SetColumn( Matrix& matrix, const Vec4& vec, int column )
{
   for( int i = 0; i < 4; i++ )
      matrix(column,i) = vec[i];
}

void MatrixUtil::SetRow( Matrix& matrix, const Vec3& vec, int row )
{
   for( int i = 0; i < 3; i++ )
      matrix(i,row) = vec[i];
}

void MatrixUtil::SetRow( Matrix& matrix, const Vec4& vec, int row )
{
   for( int i = 0; i < 4; i++ )
      matrix(i,row) = vec[i];
}

void MatrixUtil::HprToMatrix( osg::Matrix& rotation, const osg::Vec3& hpr )
{
   sgMat4 mat;

   sgVec3 sgHpr = { hpr[0], hpr[1], hpr[2] };

   sgMakeCoordMat4( mat, mat[3], sgHpr );

   for( int i = 0; i < 3; i++ )
      for( int j = 0; j < 3; j++ )
         rotation(i,j) = mat[i][j];
   
   /*
   //Converted from PLIB's SG library under the LGPL: http://plib.sourceforge.net/

   osg::Vec3 hprRadians( hpr );

   for( int i = 0; i < 3; i++ )
      hprRadians[i] = osg::DegreesToRadians( hprRadians[i] );

   float ch, sh, cp, sp, cr, sr, srsp, crsp, srcp ;

   if ( hprRadians[0] == 0.0f )
   {
      ch = 1.0f;
      sh = 0.0f;
   }
   else
   {
      sh = osg::RadiansToDegrees( sin( hprRadians[0] ) );
      ch = osg::RadiansToDegrees( cos( hprRadians[0] ) );
   }

   if ( hprRadians[1] == 0.0f )
   {
      cp = 1.0f;
      sp = 0.0f;
   }
   else
   {
      sp = osg::RadiansToDegrees( sin( hprRadians[1] ) );
      cp = osg::RadiansToDegrees( cos( hprRadians[1] ) );
   }

   if ( hprRadians[2] == 0.0f )
   {
      cr   = 1.0f;
      sr   = 0.0f;
      srsp = 0.0f;
      srcp = 0.0f;
      crsp = sp;
   }
   else
   {
      sr   = osg::RadiansToDegrees( sin( hprRadians[2] ) );
      cr   = osg::RadiansToDegrees( cos( hprRadians[2] ) );
      srsp = sr * sp;
      crsp = cr * sp;
      srcp = sr * cp;
   }

   rotation(0,0) = ch * cr - sh * srsp;
   rotation(1,0) = -sh * cp;
   rotation(2,0) = sr * ch + sh * crsp;
   rotation(3,0) = 0.0f;

   rotation(0,1) = cr * sh + srsp * ch;
   rotation(1,1) = ch * cp;
   rotation(2,1) = sr * sh - crsp * ch;
   rotation(3,1) = 0.0f;;

   rotation(0,2) = -srcp;
   rotation(1,2) = sp;
   rotation(2,2) = cr * cp;
   rotation(3,2) = 0.0f;

   rotation(0,3) = 0.0f ;
   rotation(1,3) = 0.0f ;
   rotation(2,3) = 0.0f ;
   rotation(3,3) = 1.0f ;
   */
}

void MatrixUtil::MatrixToHpr( osg::Vec3& hpr, const osg::Matrix& rotation )
{
   
   sgMat4 mat;

   for( int i = 0; i < 4; i++ )
      for( int j = 0; j < 4; j++ )
         mat[i][j] = rotation(i,j);

   sgCoord pos;
   sgSetCoord( &pos, mat );

   hpr[0] = pos.hpr[0];
   hpr[1] = pos.hpr[1];
   hpr[2] = pos.hpr[2];
   
   
   //Converted from PLIB's SG library: http://plib.sourceforge.net/

   /*
   osg::Matrix mat;

   float s = GetColumn3( mat, 0 ).length();
   
   if ( s <= 0.00001 )
   {
      hpr.set( 0.0f, 0.0f, 0.0f );
      return ;
   }

   for( int i = 0; i < 4; i++ )
      for( int j = 0; j < 4; j++ )
         mat(i,j) = rotation(i,j) / s;

   hpr[1] = osg::RadiansToDegrees( asin( osg::DegreesToRadians( osg::clampTo( float(mat(1,2)), -1.0f, 1.0f ) ) ) );

   float cp = osg::RadiansToDegrees( cos( osg::DegreesToRadians( hpr[1] ) ) );

   if ( cp > -0.00001 && cp < 0.00001 )
   {
      float cr = osg::clampTo( float(mat(0,1)), -1.0f, 1.0f  ); 
      float sr = osg::clampTo( float(-mat(2,1)), -1.0f, 1.0f  );

      hpr[0] = 0.0f ;
      hpr[2] = osg::RadiansToDegrees( atan2( osg::DegreesToRadians( sr ), osg::DegreesToRadians( cr ) ) );
   }
   else
   {
      cp = 1.0f / cp ;
      float sr = osg::clampTo( float(-mat(0,2)) * cp, -1.0f, 1.0f  );
      float cr = osg::clampTo( float(mat(2,2)) * cp, -1.0f, 1.0f  );
      float sh = osg::clampTo( float(-mat(1,0)) * cp, -1.0f, 1.0f  );
      float ch = osg::clampTo( float(mat(1,1)) * cp, -1.0f, 1.0f  );

      if ( (sh == 0.0f && ch == 0.0f) || (sr == 0.0f && cr == 0.0f) )
      {
         cr = osg::clampTo( float(mat(0,1)), -1.0f, 1.0f );
         sr = osg::clampTo( float(-mat(2,1)), -1.0f, 1.0f  );

         hpr[0] = 0.0f ;
      }
      else
      {
         hpr[0] = osg::RadiansToDegrees( atan2( osg::DegreesToRadians( sh ), osg::DegreesToRadians( ch ) ) );
      }

      hpr[2] = osg::RadiansToDegrees( atan2( osg::DegreesToRadians( sr ), osg::DegreesToRadians( cr ) ) );
   }
   */
  
}