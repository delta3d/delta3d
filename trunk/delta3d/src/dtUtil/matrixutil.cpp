#include "dtUtil/matrixutil.h"

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

void MatrixUtil::Print( const Vec3f& vec )
{
   for( int i = 0; i < 3; i++ )
   {
      std::cout << "[" << vec[i] << "]";
   }
   std::cout << std::endl;
}

void MatrixUtil::Print( const Vec4f& vec )
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

Vec3f MatrixUtil::GetColumn3( const Matrix& matrix, const int column )
{
   return Vec3f( matrix(column,0), matrix(column,1), matrix(column,2) );
}

Vec4f MatrixUtil::GetColumn4( const Matrix& matrix, const int column )
{
   return Vec4f( matrix(column,0), matrix(column,1), matrix(column,2), matrix(column,3) );
}

Vec3f MatrixUtil::GetRow3( const Matrix& matrix, const int row )
{
   return Vec3f( matrix(0,row), matrix(1,row), matrix(2,row) );
}

Vec4f MatrixUtil::GetRow4( const Matrix& matrix, const int row )
{
   return Vec4f( matrix(0,row), matrix(1,row), matrix(2,row), matrix(3,row) );
}

void MatrixUtil::SetColumn( Matrix& matrix, const Vec3f& vec, const int column )
{
   for( int i = 0; i < 3; i++ )
      matrix(column,i) = vec[i];
}

void MatrixUtil::SetColumn( Matrix& matrix, const Vec4f& vec, const int column )
{
   for( int i = 0; i < 4; i++ )
      matrix(column,i) = vec[i];
}

void MatrixUtil::SetRow( Matrix& matrix, const Vec3f& vec, const int row )
{
   for( int i = 0; i < 3; i++ )
      matrix(i,row) = vec[i];
}

void MatrixUtil::SetRow( Matrix& matrix, const Vec4f& vec, const int row )
{
   for( int i = 0; i < 4; i++ )
      matrix(i,row) = vec[i];
}
