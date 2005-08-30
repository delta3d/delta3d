#ifndef __NOISE_UTILITY_H__
#define __NOISE_UTILITY_H__


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
* @author Bradley Anderegg
*/


#include <osg/Vec2f>
#include <osg/Vec3f>

#include <osg/Vec2d>
#include <osg/Vec3d>

#include "noise1.h"
#include "noise2.h"
#include "noise3.h"

#include "SeamlessNoise.h"

#include "fractal.h"


namespace dtUtil
{

typedef Noise1<float, float> Noise1f;
typedef Noise1<double, double> Noise1d;

typedef Noise2<float, osg::Vec2f> Noise2f;
typedef Noise2<double, osg::Vec2d> Noise2d;

typedef Noise3<float, osg::Vec3f> Noise3f;
typedef Noise3<double, osg::Vec3d> Noise3d;

typedef SeamlessNoise<float, float, 1, Noise1f> SeamlessNoise1f;
typedef SeamlessNoise<double, double, 1, Noise1f> SeamlessNoise1d;

typedef SeamlessNoise<float, osg::Vec2f, 2, Noise2f> SeamlessNoise2f;
typedef SeamlessNoise<double, osg::Vec2d, 2, Noise2d> SeamlessNoise2d;

typedef SeamlessNoise<float, osg::Vec3f, 3, Noise3f > SeamlessNoise3f;
typedef SeamlessNoise<double, osg::Vec3d, 3, Noise3d> SeamlessNoise3d;

typedef Fractal<float, float, Noise1f> Fractal1f;
typedef Fractal<double, double, Noise1d> Fractal1d;

typedef Fractal<float, osg::Vec2f, Noise2f> Fractal2f;
typedef Fractal<double, osg::Vec2d, Noise2d> Fractal2d;

typedef Fractal<float, osg::Vec3f, Noise3f> Fractal3f;
typedef Fractal<double, osg::Vec3d, Noise3d> Fractal3d;


typedef Fractal<float, float, SeamlessNoise1f> SeamlessFractal1f;
typedef Fractal<double, double, SeamlessNoise1d> SeamlessFractal1d;

typedef Fractal<float, osg::Vec2f, SeamlessNoise2f> SeamlessFractal2f;
typedef Fractal<double, osg::Vec2d, SeamlessNoise2d> SeamlessFractal2d;

typedef Fractal<float, osg::Vec3f, SeamlessNoise3f> SeamlessFractal3f;
typedef Fractal<double, osg::Vec3d, SeamlessNoise3d> SeamlessFractal3d;



}//dtUtil


#endif

