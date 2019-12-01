/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, LLC
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
 */

#ifndef DTVOXEL_MARCHING_CUBES_H_
#define DTVOXEL_MARCHING_CUBES_H_

#include <osg/Vec3>
#include <dtVoxel/export.h>

namespace dtVoxel
{
   //This implementation of Marching Cubes is a modified version of the algorithm
   //from http://paulbourke.net/geometry/polygonise/

   typedef struct {
      osg::Vec3 p[8];
      float val[8];
   } GRIDCELL;

   typedef struct {
      osg::Vec3 p[3];         /* Vertices */
      osg::Vec3 n[3];         /* Normal   */
   } TRIANGLE;


   /*-------------------------------------------------------------------------
   Given a grid cell and an isolevel, calculate the triangular
   facets requied to represent the isosurface through the cell.
   Return the number of triangular facets, the array "triangles"
   will be loaded up with the vertices at most 5 triangular facets.
   0 will be returned if the grid cell is either totally above
   of totally below the isolevel.
   */
   DT_VOXEL_EXPORT int PolygonizeCube(GRIDCELL g, float iso, TRIANGLE *tri, osg::Vec3* vertArray);


} /* namespace dtVoxel */

#endif /* DTVOXEL_MARCHING_CUBES_H_ */
