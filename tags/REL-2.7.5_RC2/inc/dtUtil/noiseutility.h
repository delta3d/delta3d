#ifndef __NOISE_UTILITY_H__
#define __NOISE_UTILITY_H__


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
* Bradley Anderegg
*/

#include <cstdlib>

#include <osg/Vec2f>
#include <osg/Vec3f>

#include <osg/Vec2d>
#include <osg/Vec3d>

#include "noise1.h"
#include "noise2.h"
#include "noise3.h"

#include "seamlessnoise.h"

#include "fractal.h"



namespace dtUtil
{


   /**
   * NoiseUtility.h contains all necessary defines to use the Noise Library in
   * dtUtil
   *
   * The noise classes are templated to allow varying precision and dimensions
   * ex: for a noise class with floating point precision and two components use Noise2f
   *
   */




typedef Noise1<float, float> Noise1f;
typedef Noise1<double, double> Noise1d;

typedef Noise2<float, osg::Vec2f> Noise2f;
typedef Noise2<double, osg::Vec2d> Noise2d;

typedef Noise3<float, osg::Vec3f> Noise3f;
typedef Noise3<double, osg::Vec3d> Noise3d;

typedef Fractal<float, osg::Vec2f, Noise2f> Fractal2f;
typedef Fractal<double, osg::Vec2d, Noise2d> Fractal2d;

typedef Fractal<float, osg::Vec3f, Noise3f> Fractal3f;
typedef Fractal<double, osg::Vec3d, Noise3d> Fractal3d;

typedef Fractal<float, osg::Vec3f, SeamlessNoise> SeamlessFractal;



}//dtUtil


#endif

