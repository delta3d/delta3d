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

#ifndef DTVOXEL_VOXELGEOMETRY_H_
#define DTVOXEL_VOXELGEOMETRY_H_

#include <dtVoxel/export.h>
#include <dtPhysics/geometry.h>
#include <dtVoxel/aabbintersector.h>
#include <pal/palFactory.h>
#include <pal/palGeometry.h>
#include <openvdb/openvdb.h>
#include <bitset>
// #include <iostream>
#include <dtCore/camera.h>
#include <osg/io_utils>

namespace dtVoxel
{
   template<typename GridType>
   //typedef openvdb::BoolGrid GridType;
   class ColliderCallback : public palCustomGeometryCallback
   {
   public:
      typedef AABBIntersector<GridType> IntersectorType;
      typedef typename GridType::ConstPtr GridPtr;

      ColliderCallback(const palBoundingBox& shapeBoundingBox, GridPtr grid)
      : palCustomGeometryCallback(shapeBoundingBox)
      , mGrid(grid)
      {
      }

      ~ColliderCallback()  {}

      /**
       * Override this to return the triangles within the given axis aligned bounding box.
       */
      virtual void operator()(const palBoundingBox& bbBox, palTriangleCallback& callback)
      {
         static const int faces[] =
               {
                     0, 2, 1,
                     0, 3, 2, // -x
                     0, 1, 4,
                     6, 0, 4, // +z
                     4, 5, 6,
                     5, 7, 6, // +x
                     2, 3, 5,
                     5, 3, 7, // -z
                     0, 6, 3,
                     3, 6, 7, // +y
                     1, 2, 4,
                     4, 2, 5  // -y
               };
         //typedef typename GridType::ValueOnIter GridItr;
         openvdb::BBoxd worldBoundingBox(openvdb::Vec3d(bbBox.min.x,bbBox.min.y,bbBox.min.z), openvdb::Vec3d(bbBox.max.x,bbBox.max.y,bbBox.max.z));
         openvdb::CoordBBox collideBox;
         GridPtr grid = mGrid;
         if (worldBoundingBox.volume() < size_t(UINT32_MAX))
         {
            //std::cout << "max extent " << worldBoundingBox.maxExtent();
            collideBox = grid->transform().worldToIndexCellCentered(worldBoundingBox);
         }
         else // debug draw
         {
            //return;
            dtCore::Transform xform;
            dtCore::Camera::GetInstance(0)->GetTransform(xform);
            osg::Vec3d cameraPos = xform.GetTranslation();
            //std::cout << "camera pos " << cameraPos;
            openvdb::Vec3d cameraOvdb(cameraPos.x(), cameraPos.y(), cameraPos.z());
            openvdb::Vec3d min(cameraOvdb - openvdb::Vec3d(10.0, 10.0, 5.0));
            openvdb::Vec3d max(cameraOvdb + openvdb::Vec3d(10.0, 10.0, 5.0));

            collideBox = openvdb::CoordBBox(grid->transform().worldToIndexCellCentered(min), grid->transform().worldToIndexCellCentered(max));
         }

         const palBoundingBox& fullBoundingBox = GetBoundingBox();
         typename GridType::ConstAccessor ca = mGrid->getConstAccessor();

         //std::cout << " collision box: " << collideBox << std::endl;
         for (int i = collideBox.min().x(), iend = collideBox.max().x() + 1; i < iend; ++i)
         {
            for (int j = collideBox.min().y(), jend = collideBox.max().y() + 1; j < jend; ++j)
            {
               int partId = (fullBoundingBox.max.x - fullBoundingBox.min.x) * i + j;

               for (int k = collideBox.min().z(), kend = collideBox.max().z() + 1; k < kend; ++k)
               {
                  openvdb::math::Coord coord(i, j, k);
                  if (!ca.isValueOn(coord))
                     continue;

                  openvdb::BBoxd iBox(openvdb::Vec3d(double(coord.x()), double(coord.y()), double(coord.z())), 0);
                  iBox.expand(0.5f);
                  //std::cout << "Voxel in collision box: " << iBox << std::endl;
                  openvdb::BBoxd voxelBBox = grid->transform().indexToWorld(iBox);
                  std::bitset<6> activeNeighbors;
                  activeNeighbors[0] = ca.isValueOn(openvdb::Coord(coord.x()-1,coord.y(),coord.z()));
                  activeNeighbors[1] = ca.isValueOn(openvdb::Coord(coord.x(),coord.y(),coord.z()+1));
                  activeNeighbors[2] = ca.isValueOn(openvdb::Coord(coord.x()+1,coord.y(),coord.z()));
                  activeNeighbors[3] = ca.isValueOn(openvdb::Coord(coord.x(),coord.y(),coord.z()-1));
                  activeNeighbors[4] = ca.isValueOn(openvdb::Coord(coord.x(),coord.y()+1,coord.z()));
                  activeNeighbors[5] = ca.isValueOn(openvdb::Coord(coord.x(),coord.y()-1,coord.z()));
                  //std::cout << " Neighbors ";
                  //for (unsigned n = 0 ; n < 6 ; ++n) std::cout << activeNeighbors[n];
                  //std::cout << std::endl;
                  openvdb::math::Vec3<Float> min = voxelBBox.min();
                  openvdb::math::Vec3<Float> max = voxelBBox.max();
                  // std::cout << "voxel min max: " << min << max << std::endl;
                  Float cube_vertices[] =
                        {
                              min.x(),  max.y(),  max.z(),
                              min.x(),  min.y(),  max.z(),
                              min.x(),  min.y(),  min.z(),
                              min.x(),  max.y(),  min.z(),
                              max.x(),  min.y(),  max.z(),
                              max.x(),  min.y(),  min.z(),
                              max.x(),  max.y(),  max.z(),
                              max.x(),  max.y(),  min.z()
                        };
                  static int triCount = 12;
                  int baseTriIdx = (triCount * k);
                  for (int triIdx=0; triIdx < triCount; ++triIdx)
                  {
                     if (!activeNeighbors[triIdx/2])
                     {
                        palTriangle triangle;
                        //std::cout << "triangle ";
                        for (unsigned vertIdx = 0; vertIdx < 3; ++vertIdx)
                        {
                           triangle.vertices[vertIdx].x = cube_vertices[3 * faces[3*triIdx + vertIdx] + 0];
                           triangle.vertices[vertIdx].y = cube_vertices[3 * faces[3*triIdx + vertIdx] + 1];
                           triangle.vertices[vertIdx].z = cube_vertices[3 * faces[3*triIdx + vertIdx] + 2];
                           //std::cout << "[" << triangle.vertices[j].x << " " << triangle.vertices[j].y << " " << triangle.vertices[j].z << "]";
                        }
                        //std::cout << std::endl;
                        callback.ProcessTriangle(triangle, partId,  baseTriIdx + triIdx);
                     }
                  }
               }
            }
         }
      }

      GridPtr mGrid;
   };

   class DT_VOXEL_EXPORT VoxelGeometry : public dtPhysics::Geometry
   {
   public:

      template<typename GridTypePtr>
      static dtCore::RefPtr<VoxelGeometry> CreateVoxelGeometry(const dtCore::Transform& worldxform, float mass, GridTypePtr grid)
      {
         typedef typename GridTypePtr::element_type GridType;

         openvdb::CoordBBox bbox;
         bbox = grid->evalActiveVoxelBoundingBox();
         openvdb::Vec3d start = grid->indexToWorld(bbox.getStart());
         openvdb::Vec3d end = grid->indexToWorld(bbox.getEnd());
         //std::cout << "Bounding box of grid: " << start << "->" << end << std::endl;
         palBoundingBox palBB;
         palBB.min.Set(Float(start.x()), Float(start.y()), Float(start.z()));
         palBB.max.Set(Float(end.x()), Float(end.y()), Float(end.z()));
         ColliderCallback<GridType>* cc = new ColliderCallback<GridType>(palBB, grid);
         return CreateVoxelGeometryWithCallback(worldxform, mass, cc);
      }
      static dtCore::RefPtr<VoxelGeometry> CreateVoxelGeometryWithCallback(const dtCore::Transform& worldxform, float mass, palCustomGeometryCallback* callBack);

   protected:
      VoxelGeometry();
      virtual ~VoxelGeometry();
   };

   typedef dtCore::RefPtr<VoxelGeometry> VoxelGeometryPtr;
} /* namespace dtVoxel */

#endif /* DTVOXEL_VOXELGEOMETRY_H_ */
