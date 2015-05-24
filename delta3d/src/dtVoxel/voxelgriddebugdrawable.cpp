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

#include <dtVoxel/voxelgriddebugdrawable.h>
#include <osg/ShapeDrawable>

namespace dtVoxel
{

   VoxelGridDebugDrawable::VoxelGridDebugDrawable()
   {
   }

   VoxelGridDebugDrawable::~VoxelGridDebugDrawable()
   {
   }

   osg::Node* VoxelGridDebugDrawable::GetOSGNode()
   {
      return mSceneRoot.get();
   }

   const osg::Node* VoxelGridDebugDrawable::GetOSGNode() const
   {
      return mSceneRoot.get();
   }

   void VoxelGridDebugDrawable::CreateDebugDrawable(dtVoxel::VoxelGrid& grid)
   {
      mSceneRoot = new osg::Group();

      mGridSize = grid.GetWSDimensions();
      mBlockSize = grid.GetBlockDimensions();
      mCellSize = grid.GetCellDimensions();

      osg::Vec3f dim;
      dim[0] = std::floor(mBlockSize[0] / mCellSize[0]);
      dim[1] = std::floor(mBlockSize[1] / mCellSize[1]);
      dim[2] = std::floor(mBlockSize[2] / mCellSize[2]);

      int numBlocks = grid.GetNumBlocks();

      for (int blockCount = 0; blockCount < numBlocks; ++blockCount)
      {
         osg::Vec3 offset = grid.GetGridOffset();

         dtVoxel::VoxelBlock* curBlock = grid.GetBlockFromIndex(blockCount);
         if (curBlock->IsAllocated())
         {
            for (unsigned int x = 0; x < dim[0]; ++x)
            {
               for (unsigned int y = 0; y < dim[1]; ++y)
               {
                  for (unsigned int z = 0; z < dim[2]; ++z)
                  {
                     dtVoxel::VoxelCell* curCell = curBlock->GetCellFromIndex(x, y, z);
                     if (curCell->IsAllocated())
                     {
                        osg::Vec3 pos = curCell->GetOffset();

                        osg::Box* box = new osg::Box(pos, mCellSize.x(), mCellSize.y(), mCellSize.z());
                        osg::ShapeDrawable* sd = new osg::ShapeDrawable(box);
                        osg::Geode* sdg = new osg::Geode();
                        sdg->addDrawable(sd);
                        mSceneRoot->addChild(sdg);
                     }
                  }
               }
            }
         }

      }

   }


} /* namespace dtVoxel */
