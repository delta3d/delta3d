/*
 * aabbintersector.h
 *
 *  Created on: May 13, 2015
 *      Author: david
 */
#ifndef DTVOXEL_AABBINTERSECTOR_H_
#define DTVOXEL_AABBINTERSECTOR_H_

#include <openvdb/math/Math.h>
#include <openvdb/math/Stencils.h>
#include <openvdb/Grid.h>
#include <openvdb/Types.h>
#include <boost/utility.hpp>
#include <boost/type_traits/is_floating_point.hpp>

namespace dtVoxel
{
   template<typename GridT,
   int NodeLevel = GridT::TreeType::RootNodeType::ChildNodeType::LEVEL>
   class AABBIntersector
   {
   public:
      typedef GridT                         GridType;
      typedef typename GridT::ValueType     ValueT;
      typedef typename GridT::TreeType      TreeT;
      typedef typename TreeT::Ptr           TreePtr;
      typedef typename TreeT::RootNodeType  RootT;


      BOOST_STATIC_ASSERT( NodeLevel >= -1 && NodeLevel < int(TreeT::DEPTH)-1);

      /// @brief Constructor
      /// @param grid level set grid to intersect rays against.
      /// @param isoValue optional iso-value for the ray-intersection.
      AABBIntersector(typename GridType::Ptr grid)
      : mGrid(grid)
      {
//         if (grid->getGridClass() != openvdb::GRID_LEVEL_SET) {
//            OPENVDB_THROW(openvdb::RuntimeError,
//                  "LevelSetRayIntersector only supports level sets!"
//                  "\nUse Grid::setGridClass(openvdb::GRID_LEVEL_SET)");
//         }
      }

      void Intersect()
      {
         mHitGrid = typename GridType::Ptr(new GridType);
         mHitGrid->setTransform(mGrid->transformPtr());
         //mHitGrid->fill(mCollideBox, true, true);
         //mHitGrid->topologyIntersection(*mGrid);
         //mHitGrid->pruneGrid();
         typename GridType::ConstAccessor ca = mGrid->getConstAccessor();
         typename GridType::Accessor acc = mHitGrid->getAccessor();
         for (int i = mCollideBox.min().x(); i < mCollideBox.max().x() + 1; ++i)
         {
            for (int j = mCollideBox.min().y(); j < mCollideBox.max().y() + 1; ++j)
            {
               for (int k = mCollideBox.min().z(); k < mCollideBox.max().z() + 1; ++k)
               {
                  openvdb::math::Coord coord(i, j, k);
                  if (ca.isValueOn(coord))
                  {
                     acc.setValue(coord, ca.getValue(coord));
                  }
               }
            }
         }
      }

      typename GridType::Ptr GetGrid() const { return mGrid; }

      inline void SetIndexBB(const openvdb::math::CoordBBox& box)
      {
         mCollideBox = box;
      }

      /// @brief Return @c false the ray misses the bbox of the grid.
      /// @param wRay Ray represented in world space.
      /// @warning Call this method before the ray traversal starts.
      template <typename BBoxType>
      inline void SetWorldBB(const BBoxType& worldBoundingBox)
      {
         mCollideBox = openvdb::math::CoordBBox(openvdb::math::Coord::round(mGrid->worldToIndex(worldBoundingBox.min())), openvdb::math::Coord::round(mGrid->worldToIndex(worldBoundingBox.max())));
      }

      typename GridType::Ptr GetHits()
      {
         return mHitGrid;
      }


   private:


      typename GridType::Ptr         mHitGrid;
      const typename GridT::Ptr    mGrid;
      openvdb::math::CoordBBox mCollideBox;

   };// LevelSetRayIntersector



}

#endif /* DTVOXEL_AABBINTERSECTOR_H_ */
