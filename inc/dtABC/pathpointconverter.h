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

#ifndef __PATH_POINT_CONVERTER_H__
#define __PATH_POINT_CONVERTER_H__

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <dtCore/transformable.h>
#include <dtUtil/matrixutil.h>
#include "pathpoint.h"

namespace dtABC
{

/***
 * This class extends transformable and adds an implicit
 * conversion to a PathPoint, to simplify manipulation with BezierNodes
 * and BezierControlPoints
 */
class PathPointConverter: public dtCore::Transformable
{
public:
   PathPointConverter(){}

   PathPoint GetPathPoint() const
   {
      const osg::MatrixTransform* mat = GetMatrixNode();
      return PathPoint(dtUtil::MatrixUtil::GetRow3(mat->getMatrix(), 3), mat->getMatrix());
   }

protected:
   /*virtual*/ ~PathPointConverter(){}


};


} // namespace dtABC

#endif // __PATH_POINT_CONVERTER_H__

