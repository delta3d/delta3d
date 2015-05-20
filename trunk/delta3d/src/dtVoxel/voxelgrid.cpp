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

#include <dtVoxel/voxelgrid.h>
#include <iostream>

namespace dtVoxel
{

   VoxelGrid::VoxelGrid()
      : mNumBlocks(0)
      , mBlocks(NULL)
   {
   }

   VoxelGrid::~VoxelGrid()
   {
   }

   void VoxelGrid::Init(const osg::Vec3& grid_offset, const osg::Vec3& dimensions, const osg::Vec3& block_dimensions, const osg::Vec3& cellDimensions, const osg::Vec3i& textureResolution)
   {
      mGridOffset = grid_offset;
      mWSDimensions = dimensions;
      mBlockDimensions = block_dimensions;
      mCellDimensions = cellDimensions;
      mTextureResolution = textureResolution;

      int blocksX = int(std::floor(mWSDimensions[0] / mBlockDimensions[0]));
      int blocksY = int(std::floor(mWSDimensions[1] / mBlockDimensions[1]));

      mNumBlocks = blocksX * blocksY;
   }

   void VoxelGrid::CreateVoxelGrid(osg::Node* sceneRoot)
   {
      mRootNode = new osg::Group();

      mBlocks = new VoxelBlock[mNumBlocks];

      int blocksX = int(std::floor(mWSDimensions[0] / mBlockDimensions[0]));
      int blocksY = int(std::floor(mWSDimensions[1] / mBlockDimensions[1]));

      dtCore::RefPtr<osg::Group> currentGroup = NULL;
      for (int x = 0; x < blocksX; x++)
      {
         for (int y = 0; y < blocksY; y++)
         {
            osg::Vec3 offset = mGridOffset;
            offset[0] += x * mBlockDimensions[0];
            offset[1] += y * mBlockDimensions[1];
            mBlocks[(x * blocksY) + y].Init(mBlockDimensions, offset, mCellDimensions);
            mBlocks[(x * blocksY) + y].Allocate(mTextureResolution);

            mRootNode->addChild(mBlocks[(x * blocksY) + y].GetOSGVolume());
         }
      }

      sceneRoot->asGroup()->addChild(mRootNode);
   }


   VoxelBlock* VoxelGrid::GetBlockFromIndex(int index)
   {
      return &mBlocks[index];
   }

   VoxelBlock* VoxelGrid::GetBlockFromPos(const osg::Vec3& pos)
   {
      VoxelBlock* block = NULL;
      osg::Vec3 offset = pos - mGridOffset;
      int cellX = int(std::floor(pos[0] / mWSDimensions[0]));
      int cellY = int(std::floor(pos[1] / mWSDimensions[1]));

      int blocksX = int(std::floor(mWSDimensions[0] / mBlockDimensions[0]));
      int blocksY = int(std::floor(mWSDimensions[1] / mBlockDimensions[1]));

      int index = (cellX * blocksY) + cellY;
      if (index > 0 && index < mNumBlocks)
      {
         block = &mBlocks[index];
      }

      return block;
   }

   const osg::Vec3i& VoxelGrid::GetTextureResolution() const
   {
      return mTextureResolution;
   }


} /* namespace dtVoxel */
