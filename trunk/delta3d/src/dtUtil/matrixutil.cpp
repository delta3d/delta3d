#include "dtUtil/matrixutil.h"

using namespace dtUtil;
using namespace osg;

void MatrixUtil::Print( const Matrixf& matrix )
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

void MatrixUtil::Transpose( Matrixf& dest, const Matrixf& src )
{
   for( int i = 0; i < 4; i++ )
      for( int j = 0; j < 4; j++ )
         dest(i,j) = src(j,i);
}

void MatrixUtil::SetColumn3( Matrixf& matrix, const Vec3f& vec, const int column )
{
   for( int i = 0; i < 3; i++ )
      matrix(column,i) = vec[i];
}

void MatrixUtil::SetRow3( Matrixf& matrix, const Vec3f& vec, const int row )
{
   for( int i = 0; i < 3; i++ )
      matrix(i,row) = vec[i];
}

void MatrixUtil::SetColumn4( Matrixf& matrix, const Vec4f& vec, const int column )
{
   for( int i = 0; i < 4; i++ )
      matrix(column,i) = vec[i];
}

void MatrixUtil::SetRow4( Matrixf& matrix, const Vec4f& vec, const int row )
{
   for( int i = 0; i < 4; i++ )
      matrix(i,row) = vec[i];
}
