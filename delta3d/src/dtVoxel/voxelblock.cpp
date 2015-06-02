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

#include <dtVoxel/voxelblock.h>
#include <dtUtil/log.h>
#include <osgUtil/Simplifier>
#include <osgUtil/Optimizer>
#include <tbb/parallel_for.h>
#include <iostream>

namespace dtVoxel
{

   VoxelBlock::VoxelBlock()
      : mIsAllocated(false)      
      , mIsDirty(false)
      , mIsEmpty(true)
      , mNumCells(0)
      , mCells(NULL)
   {
   }

   VoxelBlock::~VoxelBlock()
   {
   }


   void VoxelBlock::Init(const osg::Vec3& ws_dimensions, const osg::Vec3& ws_offset, const osg::Vec3f& cellSize)
   {
      //static int count = 0;
      //std::cout << "Creating Voxel Block " << count++ << std::endl;

      mWSDimensions = ws_dimensions;
      mOffset = ws_offset;
      mWSCellDimensions = cellSize;

      mVolume = new osg::Group();
   }
   
   void VoxelBlock::DeAllocate()
   {
      //std::cout << "DeAllocating Voxel Block" << std::endl;

      mVolume->removeChildren(0, mVolume->getNumChildren());

      for (int i = 0; i < mNumCells; ++i)
      {
         mCells[i].DeAllocate();
      }

      delete[] mCells;

      mIsAllocated = false;
   }

   bool VoxelBlock::IsAllocated() const
   {
      return mIsAllocated;
   }

   void VoxelBlock::SetDirty(bool b)
   {
      mIsDirty = b;
   }

   bool VoxelBlock::GetDirty() const
   {
      return mIsDirty;
   }

   void VoxelBlock::SetEmpty(bool b)
   {
      mIsEmpty = b;
   }

   bool VoxelBlock::GetEmpty() const
   {
      return mIsEmpty;
   }

   const osg::Vec3& VoxelBlock::GetOffset()
   {
      return mOffset;
   }

   void VoxelBlock::RegenerateAABB(VoxelActor& voxelActor, const osg::BoundingBox& bb, const osg::Vec3i& textureResolution)
   {
      osg::Vec3 newDim;
      newDim[0] = bb.xMax() - bb.xMin();
      newDim[1] = bb.yMax() - bb.yMin();
      newDim[2] = bb.zMax() - bb.zMin();

      int blocksX = int(std::floor(newDim.x() / mWSCellDimensions.x()));
      int blocksY = int(std::floor(newDim.y() / mWSCellDimensions.y()));
      int blocksZ = int(std::floor(newDim.z() / mWSCellDimensions.z()));

      //std::cout << "Blocks X " << blocksX << " Blocks Y " << blocksY << " Blocks Z " << blocksZ << std::endl;

      osg::Vec3 bboffset = bb._min - mOffset;
      int startX = int(std::floor(bboffset.x() / mWSDimensions.x()));
      int startY = int(std::floor(bboffset.y() / mWSDimensions.y()));
      int startZ = int(std::floor(bboffset.z() / mWSDimensions.z()));

      //std::cout << "start X " << startX << " start Y " << startY << " start Z " << startZ << std::endl;

      int endX = startX + blocksX;
      int endY = startY + blocksY;
      int endZ = startZ + blocksZ;

      for (int z = startZ; z < endZ; z++)
      {
         for (int y = startY; y < endY; y++)
         {
            for (int x = startX; x < endX; x++)
            {
               VoxelCell* vc = GetCellFromIndex(x, y, z);
               if (vc != nullptr)
               {
                  osg::Vec3 pos(x * mWSCellDimensions[0], y * mWSCellDimensions[1], z * mWSCellDimensions[2]);
                  osg::Vec3 offsetFrom = pos + mOffset;
                  osg::Vec3 offsetTo = offsetFrom + mWSCellDimensions;

                  osg::Matrix transform;
                  transform.makeScale(1.0f, 1.0f, 1.0f); //compute offset
                  transform.setTrans(offsetFrom);

                  //todo using local grid should be faster
                  osg::BoundingBox bb(offsetFrom, offsetTo);
                  openvdb::GridBase::Ptr vdbGrid = voxelActor.CollideWithAABB(bb);

                  if (vc->IsAllocated())
                  {
                     mVolume->removeChild(vc->GetOSGNode());
                     vc->DeAllocate();
                  }

                  if (vdbGrid != NULL && !vdbGrid->empty())
                  {                    
                     vc->CreateMesh(voxelActor, vdbGrid, transform, mWSCellDimensions, textureResolution);
                     mVolume->addChild(vc->GetOSGNode());
                  }
               }
               else
               {
                  LOG_ERROR("Error calculating VoxelCell AABB region");
               }
            }
         }
      }

      SetDirty(false);
   }

   void VoxelBlock::Allocate(VoxelActor& voxelActor, const osg::Vec3i& textureResolution)
   {
      //std::cout << "Allocating Voxel Block" << std::endl;

      osg::Vec3 gridDim(
       1.0 +  int(std::floor(mWSDimensions[0] / mWSCellDimensions[0])),
       1.0 + int(std::floor(mWSDimensions[1] / mWSCellDimensions[1])),
       1.0 + int(std::floor(mWSDimensions[2] / mWSCellDimensions[2])));

      mNumCells = gridDim[0] * gridDim[1] * gridDim[2];
      //todo catch out of memory exception here
      mCells = new VoxelCell[mNumCells];
     

      //VoxelCell* cellToInit = &mCells[0];
      for (unsigned int z = 0; z < gridDim[2]; ++z)
      {
         for (unsigned int y = 0; y < gridDim[1]; ++y)
         {
            for (unsigned int x = 0; x < gridDim[0]; ++x)
            {
               osg::Vec3 pos(x * mWSCellDimensions[0], y * mWSCellDimensions[1], z * mWSCellDimensions[2]);
               osg::Vec3 offsetFrom = pos + mOffset;
               osg::Vec3 offsetTo = offsetFrom + mWSCellDimensions;

               osg::Matrix transform;
               transform.makeScale(1.0f, 1.0f, 1.0f); //compute offset
               transform.setTrans(offsetFrom);

               //todo using local grid should be faster
               osg::BoundingBox bb(offsetFrom, offsetTo);
               openvdb::GridBase::Ptr vdbGrid = voxelActor.CollideWithAABB(bb);

               if (vdbGrid != NULL && !vdbGrid->empty())
               {
                  //keeping a pointer and incrementing it should be much faster for a large contiguous dataset
                  int cellIndex = (z * gridDim[1] * gridDim[0]) + (y * gridDim[0]) + x;

                  //cellToInit->Init(transform, mWSCellDimensions, textureResolution);
                  mCells[cellIndex].CreateMesh(voxelActor, vdbGrid, transform, mWSCellDimensions, textureResolution);

                  //todo- should these be spatialized?
                  mVolume->addChild(mCells[cellIndex].GetOSGNode());
               }

               //++cellToInit;
            }
         }
      }

      //simplify mesh
      //dtCore::RefPtr<osgUtil::Simplifier> simplifier = new osgUtil::Simplifier();
      //simplifier->setMaximumLength(1000.0f);
      //simplifier->setDoTriStrip(true);
      //mVolume->accept(*simplifier);

      //spatialize
      osgUtil::Optimizer opt;
      opt.optimize(mVolume, osgUtil::Optimizer::SPATIALIZE_GROUPS);

      mIsAllocated = true;
   }

   //void VoxelBlock::AllocateCell(const osg::Vec3& pos, const osg::Vec3i& textureResolution)
   //{
   //   VoxelCell* cell = GetCellFromPos(pos);
   //   if (cell != NULL)
   //   {
   //      //todo avoid allocation here
   //      cell->AllocateImage(textureResolution[0], textureResolution[1], textureResolution[2]);
   //   }
   //   else
   //   {
   //      LOG_ERROR("Cell position out of range.");
   //   }

   //}


   VoxelCell* VoxelBlock::GetCellFromIndex(int x, int y, int z)
   {
      VoxelCell* cell = NULL;

      osg::Vec3 gridDim(
         int(std::floor(mWSDimensions[0] / mWSCellDimensions[0])),
         int(std::floor(mWSDimensions[1] / mWSCellDimensions[1])),
         int(std::floor(mWSDimensions[2] / mWSCellDimensions[2])));

      int numCells = gridDim[0] * gridDim[1] * gridDim[2];
      int cellIndex = (z * gridDim[1] * gridDim[0]) + (y * gridDim[0]) + x;

      if (cellIndex >= 0 && cellIndex < numCells)
      {
         cell = &mCells[cellIndex];
      }

      return cell;
   }

   VoxelCell* VoxelBlock::GetCellFromPos(const osg::Vec3& pos)
   {
      VoxelCell* cell = NULL;

      osg::Vec3 localPos = pos - mOffset;

      int indexX = int(std::floor(localPos[0] / mWSCellDimensions[0]));
      int indexY = int(std::floor(localPos[1] / mWSCellDimensions[1]));
      int indexZ = int(std::floor(localPos[2] / mWSCellDimensions[2]));

      osg::Vec3 gridDim(
         int(std::floor(mWSDimensions[0] / mWSCellDimensions[0])),
         int(std::floor(mWSDimensions[1] / mWSCellDimensions[1])),
         int(std::floor(mWSDimensions[2] / mWSCellDimensions[2])));

      int numCells = gridDim[0] * gridDim[1] * gridDim[2];
      int cellIndex = (indexZ * gridDim[1] * gridDim[0]) + (indexY * gridDim[0]) + indexX;

      if (cellIndex > 0 && cellIndex < numCells)
      {
         cell = &mCells[cellIndex];
      }

      return cell;
   }

   osg::Group* VoxelBlock::GetOSGNode()
   {
      return mVolume.get();
   }

   const osg::Group* VoxelBlock::GetOSGNode() const
   {
      return mVolume.get();
   }

   const osg::Vec3& VoxelBlock::GetOffset() const
   {
      return mOffset;
   }

} /* namespace dtVoxel */
