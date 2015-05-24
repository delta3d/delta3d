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
#include <dtUtil/log.h>
#include <iostream>

namespace dtVoxel
{

   VoxelGrid::VoxelGrid()
      : mBlocksX(0)
      , mBlocksY(0)
      , mBlocksZ(0)
      , mNumBlocks(0)
      , mGridOffset()
      , mWSDimensions()
      , mBlockDimensions()
      , mCellDimensions()
      , mInitialized(false)
      , mTextureResolution()
      , mRootNode(NULL)
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

      mBlocksX = int(std::floor(mWSDimensions[0] / mBlockDimensions[0]));
      mBlocksY = int(std::floor(mWSDimensions[1] / mBlockDimensions[1]));
      mBlocksZ = int(std::floor(mWSDimensions[2] / mBlockDimensions[2]));

      mNumBlocks = mBlocksX * mBlocksY * mBlocksZ;

      mInitialized = true;
   }

   void VoxelGrid::CreateGridFromActor(VoxelActor& voxelActor)
   {
      if (!mInitialized)
      {
         LOG_ERROR("VoxelGrid has not been initialized.");
         return;
      }

      mRootNode = new osg::Group();

      mBlocks = new VoxelBlock[mNumBlocks];

      dtCore::RefPtr<osg::Group> currentGroup = NULL;
      
      for (int z = 0; z < mBlocksZ; z++)
      {
         for (int y = 0; y < mBlocksY; y++)         
         {
            for (int x = 0; x < mBlocksX; x++)
            {
               VoxelBlock* curBlock = GetBlockFromIndex(x, y, z);

               osg::Vec3 offsetFrom = mGridOffset;
               offsetFrom[0] += x * mBlockDimensions[0];
               offsetFrom[1] += y * mBlockDimensions[1];
               offsetFrom[2] += z * mBlockDimensions[2];

               osg::Vec3 offsetTo = offsetFrom + mBlockDimensions;

               curBlock->Init(mBlockDimensions, offsetFrom, mCellDimensions);

               osg::BoundingBox bb(offsetFrom, offsetTo);
               openvdb::GridBase::Ptr vdbGrid = voxelActor.CollideWithAABB(bb);

               if (vdbGrid != NULL && !vdbGrid->empty())
               {
                  curBlock->Allocate(voxelActor, vdbGrid, mTextureResolution);

                  //todo- spatially subdivide
                  mRootNode->addChild(curBlock->GetOSGVolume());
               }
            }
         }
      }
   }

   VoxelBlock* VoxelGrid::GetBlockFromIndex(int x, int y, int z)
   {
      int index = (z * mBlocksY * mBlocksX) + (y * mBlocksX) + x;
      
      return &mBlocks[index];
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

   osg::Node* VoxelGrid::GetOSGNode()
   {
      return mRootNode.get();
   }

   const osg::Node* VoxelGrid::GetOSGNode() const
   {
      return mRootNode.get();
   }

} /* namespace dtVoxel */
