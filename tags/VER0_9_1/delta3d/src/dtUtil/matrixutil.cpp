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

   //implementation converted from plib's sg.cxx
   //PLIB - A Suite of Portable Game Libraries
   //Copyright (C) 1998,2002  Steve Baker
   //For further information visit http://plib.sourceforge.net


   double ch, sh, cp, sp, cr, sr, srsp, crsp, srcp ;


   if ( hpr[0] == 0.0f )
   {
      ch = 1.0 ;
      sh = 0.0 ;
   }
   else
   {
      sh = sinf(osg::DegreesToRadians(hpr[0]));
      ch = cosf(osg::DegreesToRadians(hpr[0]));
   }

   if ( hpr[1] == 0.0f )
   {
      cp = 1.0 ;
      sp = 0.0 ;
   }
   else
   {
      sp = sinf(osg::DegreesToRadians(hpr[1]));
      cp = cosf(osg::DegreesToRadians(hpr[1]));
   }

   if ( hpr[2] == 0.0f )
   {
      cr   = 1.0 ;
      sr   = 0.0 ;
      srsp = 0.0 ;
      srcp = 0.0 ;
      crsp = sp ;
   }
   else
   {
      sr   = sinf(osg::DegreesToRadians(hpr[2]));
      cr   = cosf(osg::DegreesToRadians(hpr[2]));
      srsp = sr * sp ;
      crsp = cr * sp ;
      srcp = sr * cp ;
   }

   rotation(0, 0) = (  ch * cr - sh * srsp ) ;
   rotation(1, 0) = ( -sh * cp ) ;
   rotation(2, 0) = (  sr * ch + sh * crsp ) ;

   rotation(0, 1) = ( cr * sh + srsp * ch ) ;
   rotation(1, 1) = ( ch * cp ) ;
   rotation(2, 1) = ( sr * sh - crsp * ch ) ;

   rotation(0, 2) = ( -srcp ) ;
   rotation(1, 2) = (  sp ) ;
   rotation(2, 2) = (  cr * cp ) ;

   rotation(0, 3) =  0.0;
   rotation(1, 3) =  0.0;
   rotation(2, 3) =  0.0;
	rotation(3, 3) =  1.0;

}

void MatrixUtil::PositionAndHprToMatrix( osg::Matrix& rotation, const osg::Vec3& xyz, const osg::Vec3& hpr )
{
   HprToMatrix(rotation, hpr);

   rotation(3, 0) = xyz[0];
   rotation(3, 1) = xyz[1];
   rotation(3, 2) = xyz[2];
}


void MatrixUtil::MatrixToHpr( osg::Vec3& hpr, const osg::Matrix& rotation )
{

   //implementation converted from plib's sg.cxx
   //PLIB - A Suite of Portable Game Libraries
   //Copyright (C) 1998,2002  Steve Baker
   //For further information visit http://plib.sourceforge.net

   osg::Matrix mat;

   osg::Vec3 col1(rotation(0, 0), rotation(0, 1), rotation(0, 2));
   double s = col1.length();

   if ( s <= 0.00001 )
   {
      hpr.set(0.0f, 0.0f, 0.0f);
      return ;
   }


   double oneOverS = 1.0f / s;
   for( int i = 0; i < 3; i++ )
      for( int j = 0; j < 3; j++ )
         mat(i, j) = rotation(i, j) * oneOverS;


   hpr[1] = osg::RadiansToDegrees(asin(ClampUnity(mat(1, 2))));

   double cp = cos(osg::DegreesToRadians(hpr[1]));

   if ( cp > -0.00001 && cp < 0.00001 )
   {
      double cr = ClampUnity(mat(0,1));
      double sr = ClampUnity(-mat(2,1));

      hpr[0] = 0.0f;
      hpr[2] = osg::RadiansToDegrees(atan2(sr,cr));
   }
   else
   {
      cp = 1.0 / cp ;
      double sr = ClampUnity(-mat(0,2) * cp);
      double cr = ClampUnity(mat(2,2) * cp);
      double sh = ClampUnity(-mat(1,0) * cp);
      double ch = ClampUnity(mat(1,1) * cp);

      if ( (sh == 0.0f && ch == 0.0f) || (sr == 0.0f && cr == 0.0f) )
      {
         cr = ClampUnity(mat(0,1));
         sr = ClampUnity(-mat(2,1));

         hpr[0] = 0.0f;
      }
      else
      {
        hpr[0] = osg::RadiansToDegrees(atan2(sh, ch));
      }

      hpr[2] = osg::RadiansToDegrees(atan2(sr, cr));
   }


}

float MatrixUtil::ClampUnity(const float x)
{
   if ( x >  1.0f ) return  1.0f;
   if ( x < -1.0f ) return -1.0f;
   return x ;
}

void MatrixUtil::MatrixToHprAndPosition( osg::Vec3& xyz, osg::Vec3& hpr, const osg::Matrix& rotation )
{

   MatrixToHpr(hpr, rotation);
   xyz[0] = rotation(3, 0);
   xyz[1] = rotation(3, 1);
   xyz[2] = rotation(3, 2);

}

void MatrixUtil::TransformVec3(osg::Vec3& xyz, const osg::Matrix& transformMat)
{
   TransformVec3(xyz, xyz, transformMat);
}

void MatrixUtil::TransformVec3(osg::Vec3& vec_in, const osg::Vec3& xyz, const osg::Matrix& transformMat)
{

   vec_in = osg::Matrix::transform3x3(xyz, transformMat);
   vec_in[0] += transformMat(3,0);
   vec_in[1] += transformMat(3,1);
   vec_in[2] += transformMat(3,2);

}