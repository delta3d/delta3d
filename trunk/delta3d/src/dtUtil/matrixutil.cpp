#include <prefix/dtutilprefix.h>
#include <dtUtil/matrixutil.h>
#include <iostream>

using namespace dtUtil;

void MatrixUtil::Print(const osg::Matrix& matrix)
{
   for (int i = 0; i < 4; ++i)
   {
      for (int j = 0; j < 4; ++j)
      {
         std::cout << "[" << matrix(i,j) << "]";
      }
      std::cout << std::endl;
   }
}


void MatrixUtil::Print(const osg::Vec3& vec)
{
   for (int i = 0; i < 3; ++i)
   {
      std::cout << "[" << vec[i] << "]";
   }
   std::cout << std::endl;
}


void MatrixUtil::Print(const osg::Vec4& vec)
{
   for (int i = 0; i < 4; ++i)
   {
      std::cout << "[" << vec[i] << "]";
   }
   std::cout << std::endl;
}


void MatrixUtil::Transpose(osg::Matrix& dest, const osg::Matrix& src)
{
   for (int i = 0; i < 4; ++i)
   {
      for (int j = 0; j < 4; ++j)
      {
         dest(i,j) = src(j,i);
      }
   }
}


osg::Vec3 MatrixUtil::GetRow3(const osg::Matrix& matrix, int row)
{
   return osg::Vec3(matrix(row,0), matrix(row, 1), matrix(row,2));
}


osg::Vec4 MatrixUtil::GetRow4(const osg::Matrix& matrix, int row)
{
   return osg::Vec4(matrix(row,0), matrix(row,1), matrix(row,2), matrix(row,3));
}


void MatrixUtil::SetRow(osg::Matrix& matrix, const osg::Vec3& vec, int row)
{
   for (int i = 0; i < 3; ++i)
   {
      matrix(row,i) = vec[i];
   }
}


void MatrixUtil::SetRow(osg::Matrix& matrix, const osg::Vec4& vec, int row)
{
   for (int i = 0; i < 4; ++i)
   {
      matrix(row, i) = vec[i];
   }
}


void MatrixUtil::HprToMatrix(osg::Matrix& rotation, const osg::Vec3& hpr, bool zeroTranslation)
{
   // implementation converted from plib's sg.cxx
   // PLIB - A Suite of Portable Game Libraries
   // Copyright (C) 1998,2002  Steve Baker
   // For further information visit http://plib.sourceforge.net


   double ch, sh, cp, sp, cr, sr, srsp, crsp, srcp;

   // this can't be smart for both 32 and 64 bit types.
   ///\todo find a preprocessor way to assign this constant different for the different precision types.
   const osg::Vec3::value_type magic_epsilon = (osg::Vec3::value_type)0.00001;

   if (osg::equivalent(hpr[0], (osg::Vec3::value_type)0.0, magic_epsilon))
   {
      ch = 1.0;
      sh = 0.0;
   }
   else
   {
      sh = sinf(osg::DegreesToRadians(hpr[0]));
      ch = cosf(osg::DegreesToRadians(hpr[0]));
   }

   if (osg::equivalent(hpr[1], (osg::Vec3::value_type)0.0, magic_epsilon))
   {
      cp = 1.0;
      sp = 0.0;
   }
   else
   {
      sp = sinf(osg::DegreesToRadians(hpr[1]));
      cp = cosf(osg::DegreesToRadians(hpr[1]));
   }

   if (osg::equivalent(hpr[2], (osg::Vec3::value_type)0.0, magic_epsilon))
   {
      cr   = 1.0;
      sr   = 0.0;
      srsp = 0.0;
      srcp = 0.0;
      crsp = sp;
   }
   else
   {
      sr   = sinf(osg::DegreesToRadians(hpr[2]));
      cr   = cosf(osg::DegreesToRadians(hpr[2]));
      srsp = sr * sp;
      crsp = cr * sp;
      srcp = sr * cp;
   }

   rotation(0, 0) =  ch * cr - sh * srsp;
   rotation(1, 0) = -sh * cp;
   rotation(2, 0) =  sr * ch + sh * crsp;

   rotation(0, 1) =  cr * sh + srsp * ch;
   rotation(1, 1) =  ch * cp;
   rotation(2, 1) =  sr * sh - crsp * ch;

   rotation(0, 2) = -srcp;
   rotation(1, 2) =  sp;
   rotation(2, 2) =  cr * cp;

   if (zeroTranslation)
   {
      rotation(3, 0) =  0.0;  // x trans
      rotation(3, 1) =  0.0;  // y trans
      rotation(3, 2) =  0.0;  // z trans
   }

   rotation(0, 3) =  0.0;
   rotation(1, 3) =  0.0;
   rotation(2, 3) =  0.0;
   rotation(3, 3) =  1.0;
}


void MatrixUtil::PositionAndHprToMatrix(osg::Matrix& rotation, const osg::Vec3& xyz, const osg::Vec3& hpr)
{
   HprToMatrix(rotation, hpr);

   rotation(3, 0) = xyz[0];
   rotation(3, 1) = xyz[1];
   rotation(3, 2) = xyz[2];
}


void MatrixUtil::MatrixToHpr(osg::Vec3& hpr, const osg::Matrix& rotation)
{
   // implementation converted from plib's sg.cxx
   // PLIB - A Suite of Portable Game Libraries
   // Copyright (C) 1998,2002  Steve Baker
   // For further information visit http://plib.sourceforge.net

   osg::Matrix mat;

   osg::Vec3 col1(rotation(0, 0), rotation(0, 1), rotation(0, 2));
   double s = col1.length();

   const double magic_epsilon = 0.00001;
   if (s <= magic_epsilon)
   {
      hpr.set(0.0f, 0.0f, 0.0f);
      return;
   }


   double oneOverS = 1.0f / s;
   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         mat(i, j) = rotation(i, j) * oneOverS;
      }
   }


   double sin_pitch = ClampUnity(mat(1, 2));
   double pitch = asin(sin_pitch);
   hpr[1] = osg::RadiansToDegrees(pitch);

   double cp = cos(pitch);

   if (cp > -magic_epsilon && cp < magic_epsilon)
   {
      double cr = ClampUnity(-mat(2,1));
      double sr = ClampUnity(mat(0,1));

      if (hpr[1] < 0.0f)
      {
         hpr[0] = 180.f;
      }
      else
      {
         hpr[0] = 0.0f;
      }

      hpr[2] = osg::RadiansToDegrees(atan2(sr,cr));
   }
   else
   {
      double one_over_cp = 1.0 / cp;
      double sr = ClampUnity(-mat(0,2) * one_over_cp);
      double cr = ClampUnity( mat(2,2) * one_over_cp);
      double sh = ClampUnity(-mat(1,0) * one_over_cp);
      double ch = ClampUnity( mat(1,1) * one_over_cp);

      if ((osg::equivalent(sh,0.0,magic_epsilon) && osg::equivalent(ch,0.0,magic_epsilon)) ||
          (osg::equivalent(sr,0.0,magic_epsilon) && osg::equivalent(cr,0.0,magic_epsilon)) )
      {
         cr = ClampUnity(-mat(2,1));
         sr = ClampUnity(mat(0,1));;

         hpr[0] = 0.0f;
      }
      else
      {
        hpr[0] = osg::RadiansToDegrees(atan2(sh, ch));
      }

      hpr[2] = osg::RadiansToDegrees(atan2(sr, cr));
   }
}


float MatrixUtil::ClampUnity(float x)
{
   if (x >  1.0f) { return  1.0f; }
   if (x < -1.0f) { return -1.0f; }
   return x;
}


void MatrixUtil::MatrixToHprAndPosition(osg::Vec3& xyz, osg::Vec3& hpr, const osg::Matrix& rotation)
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
