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
#include <dtCore/project.h>
#include <dtUtil/log.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/mathdefines.h>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
#include <osg/PagedLOD>
#include <iostream>
#include <fstream>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range3d.h>

#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>

const char START_END_CHAR = '!';
const int FILE_IDENT = 99834;
const std::string DATABASE_FILENAME("VoxelGridDatabase.osgb");

namespace dtVoxel
{

   VoxelGrid::VoxelGrid()
      : mBlocksX(0)
      , mBlocksY(0)
      , mBlocksZ(0)
      , mNumBlocks(0)
      , mViewDistance(1000.0f)
      , mGridOffset()
      , mWSDimensions()
      , mBlockDimensions()
      , mCellDimensions()
      , mInitialized(false)
      , mRes0()
      , mRes1()
      , mRes2()
      , mRes3()
      , mDist0(350.0f)
      , mDist1(500.0f)
      , mDist2(750.0f)
      , mAllocatedBounds()      
      , mStaticResolution()
      , mDynamicResolution()
      , mRootNode(new osg::Group())
      , mVoxelActor()
      , mFullPathToFileCache()
      , mBlockVisibility()
      , mDirtyCells()
      , mBlocks(nullptr)
   {
   }

   VoxelGrid::~VoxelGrid()
   {
   }

   void VoxelGrid::Init(const osg::Vec3& grid_offset, const osg::Vec3& dimensions, const osg::Vec3& block_dimensions, const osg::Vec3& cellDimensions, const osg::Vec3i& staticResolution, const osg::Vec3i& dynamicResolution)
   {
      mGridOffset = grid_offset;
      mWSDimensions = dimensions;
      mBlockDimensions = block_dimensions;
      mCellDimensions = cellDimensions;      
      mStaticResolution = staticResolution;
      mDynamicResolution = dynamicResolution;

      mBlocksX = int(std::floor(mWSDimensions[0] / mBlockDimensions[0]));
      mBlocksY = int(std::floor(mWSDimensions[1] / mBlockDimensions[1]));
      mBlocksZ = int(std::floor(mWSDimensions[2] / mBlockDimensions[2]));

      mNumBlocks = mBlocksX * mBlocksY * mBlocksZ;

      mBlocks = new VoxelBlock[mNumBlocks];


      mRes0 = mStaticResolution;

      mRes1.set(int(std::floor(float(mStaticResolution[0]) * 0.75f)),
         int(std::floor(float(mStaticResolution[1]) * 0.75f)),
         int(std::floor(float(mStaticResolution[2]) * 0.75f)));

      mRes2.set(int(std::floor(float(mRes1[0]) * 0.75f)),
         int(std::floor(float(mRes1[1]) * 0.75f)),
         int(std::floor(float(mRes1[2]) * 0.75f)));

      mRes3.set(1 + int(std::floor(float(mRes2[0]) * 0.75f)),
         1 + int(std::floor(float(mRes2[1]) * 0.75f)),
         1 + int(std::floor(float(mRes2[2]) * 0.75f)));



      mInitialized = true;
   }

   osg::BoundingBox VoxelGrid::ComputeWorldBounds(const osg::Vec3& pos)
   {
      osg::Vec3 bbMin;
      osg::Vec3 bbMax;

      osg::Vec3 clampMax = mGridOffset + mWSDimensions;
      
      
      for (int i = 0; i < 3; ++i)
      {
         bbMin[i] = pos[i] - mViewDistance;         
         dtUtil::Clamp(bbMin[i], mGridOffset[i], clampMax[i]);

         bbMax[i] = pos[i] + mViewDistance;
         dtUtil::Clamp(bbMax[i], mGridOffset[i], clampMax[i]);
      }

      return osg::BoundingBox(bbMin, bbMax);
   }

   void VoxelGrid::MarkDirtyAABB(const osg::BoundingBox& bb)
   {            
      //std::cout << "Mark Dirty AABB, offset " << mGridOffset << std::endl;
      osg::BoundingBox bounds(mGridOffset, mGridOffset + mWSDimensions);
      osg::BoundingBox dirtyBounds = bb.intersect(bounds);

      osg::Vec3i startIndex = GetIndexFromPos(dirtyBounds._min);
      osg::Vec3i endIndex = GetIndexFromPos(dirtyBounds._max);

      startIndex.x() = dtUtil::Max(startIndex.x(), 0);
      startIndex.y() = dtUtil::Max(startIndex.y(), 0);
      startIndex.z() = dtUtil::Max(startIndex.z(), 0);
      endIndex.x() = dtUtil::Min(endIndex.x(), mBlocksX);
      endIndex.y() = dtUtil::Min(endIndex.y(), mBlocksY);
      endIndex.z() = dtUtil::Min(endIndex.z(), mBlocksZ);
      //std::cout << std::endl << "start index " << startIndex.x() << ", " << startIndex.y() << ", " << startIndex.z() << std::endl;
      //std::cout << std::endl << "end index " << endIndex.x() << ", " << endIndex.y() << ", " << endIndex.z() << std::endl;


      for (int z = startIndex.z(); z <= endIndex.z(); z++)
      {
         for (int y = startIndex.y(); y <= endIndex.y(); y++)
         {
            for (int x = startIndex.x(); x <= endIndex.x(); x++)
            {
               VoxelBlock* curBlock = GetBlockFromIndex(x, y, z);

               if (curBlock != nullptr)
               {
                  //std::cout << "Collecting dirty cells" << std::endl;

                  curBlock->CollectDirtyCells(*mVoxelActor, bb, mDynamicResolution, mDirtyCells);
               }
            }
         }
      }
   }


   void VoxelGrid::UpdateGrid(const osg::Vec3& newCameraPos)
   {

      std::list<VoxelCellUpdateInfo>::iterator iter = mDirtyCells.begin();
      std::list<VoxelCellUpdateInfo>::iterator iterEnd = mDirtyCells.end();

      for (; iter != iterEnd;)
      {
         VoxelCellUpdateInfo& updateInfo = *iter;

         if (updateInfo.mStarted && updateInfo.mCell->CheckTaskStatus())
         {
            updateInfo.mBlock->RegenerateCell(*mVoxelActor, updateInfo.mCell, updateInfo.mNodeToUpdate.get(), updateInfo.mCellIndex, mDynamicResolution, mViewDistance);

            iter = mDirtyCells.erase(iter);
         }
         else
         {
            ++iter;
         }
      }

   }

   void VoxelGrid::BeginNewUpdates(const osg::Vec3& newCameraPos, unsigned maxCellsToUpdate, bool allowBackgroundThreading)
   {
      unsigned runCount = maxCellsToUpdate;
      for (auto iter = mDirtyCells.begin(); runCount > 0 && iter != mDirtyCells.end(); ++iter)
      {
         VoxelCellUpdateInfo& updateInfo = *iter;
         if (updateInfo.mCell->RunTask(allowBackgroundThreading))
         {
            updateInfo.mStarted = true;
            --runCount;
         }
      }
   }

   void VoxelGrid::CreateGridFromActor(const osg::Vec3& pos, VoxelActor& voxelActor)
   {
      if (!mInitialized)
      {
         LOG_ERROR("VoxelGrid has not been initialized.");
         return;
      }

      mOffset = pos;
      mAllocatedBounds = ComputeWorldBounds(pos);
      
      mVoxelActor = &voxelActor;
      
      //std::cout << std::endl << "Creating Voxel Grid with " << mNumBlocks << " Voxel Blocks" << std::endl;

      GenerateCacheString();

      
      //int blockCount = 0;
      OpenThreads::Atomic blockCount;
      tbb::parallel_for(tbb::blocked_range3d<int>(0,mBlocksZ,1, 0,mBlocksY,1, 0,mBlocksX,1),
            [=, &blockCount](const tbb::blocked_range3d<int>& r)
            {
      for (int z = r.pages().begin(); z < r.pages().end(); z++)
      {
         for (int y=r.rows().begin(); y!=r.rows().end(); ++y)//(int y = 0; y < mBlocksY; y++)
         {
            for (int x = r.cols().begin(); x < r.cols().end(); x++)
            {
               VoxelBlock* curBlock = GetBlockFromIndex(x, y, z);

               osg::Vec3 offsetFrom = mGridOffset;
               offsetFrom[0] += x * mBlockDimensions[0];
               offsetFrom[1] += y * mBlockDimensions[1];
               offsetFrom[2] += z * mBlockDimensions[2];

               osg::Vec3 offsetTo = offsetFrom + mBlockDimensions;

               curBlock->Init(mBlockDimensions, offsetFrom, mCellDimensions);
               
               osg::BoundingBox bb(offsetFrom, offsetTo);
               bool hasData = mVoxelActor->HasDataInAABB(bb);

               //bool collides = mVoxelActor->FastSampleWithAABB(bb, osg::Vec3i(50, 50, 10));

               if (hasData)
               {
                  int index = (z * mBlocksY * mBlocksX) + (y * mBlocksX) + x;

                  //only allocate this block if it is within our visual bounds
                  if (mAllocatedBounds.contains(GetCenterOfBlock(x, y, z)))
                  {
                     //attempt to load from cache
                     if (!curBlock->LoadCachedModel(mFullPathToFileCache, index))
                     {
                        //allocate this block
                        //curBlock->AllocateCombinedMesh(*mVoxelActor, mStaticResolution);
                        
                        curBlock->AllocateLODMesh(*mVoxelActor, mRes0, mDist0, mRes1, mDist1, mRes2, mDist2, mRes3, mViewDistance);

                        curBlock->SaveCachedModel(mFullPathToFileCache, index);

                     }

                     mRootNode->addChild(curBlock->GetOSGNode());
                  }
                  else
                  {
                     if (!curBlock->HasCachedModel(mFullPathToFileCache, index))
                     {
                        //std::cout << "Caching model for later use" << std::endl;
                        //curBlock->AllocateCombinedMesh(*mVoxelActor, mStaticResolution);
                        curBlock->AllocateLODMesh(*mVoxelActor, mRes0, mDist0, mRes1, mDist1, mRes2, mDist2, mRes3, mViewDistance);

                        curBlock->SaveCachedModel(mFullPathToFileCache, index);

                        curBlock->DeAllocate();
                     }
                  }
                  
                  curBlock->SetEmpty(false);
               }
               ++blockCount;
            }
            
            //std::cout << std::endl << mNumBlocks - blockCount << " Blocks remaining." << std::endl;

         }
      }
            });

      LOGN_DEBUG("voxelgrid.cpp", "Done Creating Voxel Grid");

      /*std::cout << std::endl << "Optimizing Grid" << std::endl;
      osgUtil::Optimizer opt;
      opt.optimize(mRootNode, osgUtil::Optimizer::SPATIALIZE_GROUPS);
      std::cout << std::endl << "Done Optimizing" << std::endl;*/
   }

   void VoxelGrid::CreatePagedLODGrid(const osg::Vec3& pos, VoxelActor& voxelActor)
   {
      OpenThreads::Mutex m;
      bool ReadFirstFile = false;

      if (!mInitialized)
      {
         LOG_ERROR("VoxelGrid has not been initialized.");
         return;
      }

      mAllocatedBounds = ComputeWorldBounds(pos);

      mVoxelActor = &voxelActor;

      LOGN_DEBUG("voxelgrid.cpp", "Creating Voxel Grid with " + dtUtil::ToString(mNumBlocks) + " Voxel Blocks");

      GenerateCacheString();

      if (!ReadBlockVisibility())
      {
         GenerateVisibility(voxelActor);

         bool success = WriteBlockVisibility();

         if (!success)
         {
            LOG_ERROR("Error writing visibility information.");
         }
      }

      
      OpenThreads::Atomic blockCount;
      for (int z = 0; z < mBlocksZ; z++)
      {         
         tbb::parallel_for(tbb::blocked_range<int>(0, mBlocksY),
            [=, &ReadFirstFile, &blockCount, &m](const tbb::blocked_range<int>& r)
         {
            for (int y = r.begin(); y != r.end(); ++y)//(int y = 0; y < mBlocksY; y++)
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

                  int index = (z * mBlocksY * mBlocksX) + (y * mBlocksX) + x;

                  if (!mBlockVisibility[index])
                  {
                     if (!ReadFirstFile)
                     {
                        OpenThreads::ScopedLock<OpenThreads::Mutex> addChildMutex(m);
                        ReadFirstFile = ReadFirstFile || curBlock->LoadCachedModel(mFullPathToFileCache, index);
                     }

                     if (!curBlock->LoadCachedModel(mFullPathToFileCache, index))
                     {
                        curBlock->WritePagedLOD(*mVoxelActor, index, mFullPathToFileCache, mCacheFolder, mRes0, mDist0, mRes1, mDist1, mRes2, mDist2, mRes3, mViewDistance);
                     }

                     {
                        OpenThreads::ScopedLock<OpenThreads::Mutex> addChildMutex(m);
                        mRootNode->addChild(curBlock->GetOSGNode());
                     }

                     curBlock->SetEmpty(false);
                  }
                  ++blockCount;
               }

               //std::cout << std::endl << mNumBlocks - blockCount << " Blocks remaining." << std::endl;

            }
         });
      }

      LOGN_DEBUG("voxelgrid.cpp", "Done Creating Voxel Grid");

      LOGN_DEBUG("voxelgrid.cpp", "Optimizing Grid");
      osgUtil::Optimizer opt;
      opt.optimize(mRootNode, osgUtil::Optimizer::SPATIALIZE_GROUPS);
      LOGN_DEBUG("voxelgrid.cpp", "Done Optimizing");
   }


   void VoxelGrid::GenerateVisibility(VoxelActor& voxelActor)
   {
      if (!mInitialized)
      {
         LOG_ERROR("VoxelGrid has not been initialized.");
         return;
      }
    
      LOGN_DEBUG("voxelgrid.cpp", "Generating Visibility");
      
      mBlockVisibility.resize(mNumBlocks);
      
      //int blockCount = 0;
      OpenThreads::Atomic blockCount;
      for (int z = 0; z < mBlocksZ; z++)
      {
         
         tbb::parallel_for(tbb::blocked_range<int>(0, mBlocksY),
            [=, &blockCount](const tbb::blocked_range<int>& r)
         {
            for (int y = r.begin(); y != r.end(); ++y)//(int y = 0; y < mBlocksY; y++)
            {
               for (int x = 0; x < mBlocksX; x++)
               {
                  osg::Vec3 offsetFrom = mGridOffset;
                  offsetFrom[0] += x * mBlockDimensions[0];
                  offsetFrom[1] += y * mBlockDimensions[1];
                  offsetFrom[2] += z * mBlockDimensions[2];

                  osg::Vec3 offsetTo = offsetFrom + mBlockDimensions;

                  osg::BoundingBox bb(offsetFrom, offsetTo);
                  bool hasData = mVoxelActor->HasDataInAABB(bb);

                  int index = (z * mBlocksY * mBlocksX) + (y * mBlocksX) + x;

                  if (hasData)
                  {                     
                     mBlockVisibility[index] = false;
                  }
                  else
                  {
                     mBlockVisibility[index] = true;
                  }
                  
                  ++blockCount;
               }

               //std::cout << std::endl << mNumBlocks - blockCount << " Blocks remaining." << std::endl;
            }
         });
      }

      LOGN_DEBUG("voxelgrid.cpp", "Done Generating Visibility");

      /*std::cout << "Visibility Table" << std::endl;

      for(int i = 0; i < mNumBlocks; ++i)
      {
         std::cout << dtUtil::ToString(mBlockVisibility[i]) << "   ";
      }

      std::cout << std::endl << std::endl;*/
   }



   VoxelBlock* VoxelGrid::GetBlockFromIndex(int x, int y, int z)
   {
      VoxelBlock* block = NULL;

      int index = (z * mBlocksY * mBlocksX) + (y * mBlocksX) + x;
      
      if (index >= 0 && index < mNumBlocks)
      {
         block = &mBlocks[index];
      }
      else
      {
         LOG_ERROR("Block index out of bounds.");
      }
      
      return block;
   }

   osg::Vec3 VoxelGrid::GetCenterOfBlock(int x, int y, int z)
   {
      float blockX = x * mBlockDimensions.x();
      float blockY = y * mBlockDimensions.y();
      float blockZ = z * mBlockDimensions.z();

      osg::Vec3 pos(blockX, blockY, blockZ);

      osg::Vec3 halfBlock = mBlockDimensions * 0.5;

      pos += halfBlock;
      pos += mGridOffset;
      
      return pos;
   }


   VoxelBlock* VoxelGrid::GetBlockFromIndex(int index)
   {
      VoxelBlock* block = NULL;

      if (index >= 0 && index < mNumBlocks)
      {
         block = &mBlocks[index];
      }
      else
      {
         LOG_ERROR("Block index out of bounds.");
      }

      return block;
   }

   osg::Vec3i VoxelGrid::GetIndexFromPos(const osg::Vec3& pos)
   {
      osg::Vec3 offset = pos - mGridOffset;
      int cellX = int(std::floor(offset[0] / mBlockDimensions[0]));
      int cellY = int(std::floor(offset[1] / mBlockDimensions[1]));
      int cellZ = int(std::floor(offset[2] / mBlockDimensions[2]));
         
      return osg::Vec3i(cellX, cellY, cellZ);

   }

   VoxelBlock* VoxelGrid::GetBlockFromPos(const osg::Vec3& pos)
   {
      VoxelBlock* block = NULL;
      osg::Vec3 offset = pos - mGridOffset;
      int cellX = int(std::floor(offset[0] / mBlockDimensions[0]));
      int cellY = int(std::floor(offset[1] / mBlockDimensions[1]));
      int cellZ = int(std::floor(offset[2] / mBlockDimensions[2]));

      int index = (cellZ * mBlocksY * mBlocksX) + (cellY * mBlocksX) + cellX;

      if (index > 0 && index < mNumBlocks)
      {
         block = &mBlocks[index];
      }

      return block;
   }

   const osg::Vec3i& VoxelGrid::GetStaticResolution() const
   {
      return mStaticResolution;
   }
   
   const osg::Vec3i& VoxelGrid::GetDynamicResolution() const
   {
      return mDynamicResolution;
   }

   osg::Node* VoxelGrid::GetOSGNode()
   {
      return mRootNode.get();
   }

   const osg::Node* VoxelGrid::GetOSGNode() const
   {
      return mRootNode.get();
   }
   
   bool VoxelGrid::ReadBlockVisibility()
   {
      std::string filePath = mFullPathToFileCache;
      std::string filename("VisibilityCache.dat");
      std::ifstream inFile(filePath + filename, std::ios::in | std::ios::binary);

      if (!inFile.fail())
      {
         //get length of file
         inFile.seekg(0, std::ios::end);
         int length = inFile.tellg();
         inFile.seekg(0, std::ios::beg);

         //read data
         char* buffer = new char[length];
         inFile.read(buffer, length);

         dtUtil::DataStream ds(buffer, length);

         char startChar = 'a';
         ds.Read(startChar);

         if (startChar != START_END_CHAR)
         {
            return false;
         }

         //write magic number
         int magicNum = 0;
         ds.Read(magicNum);

         if (magicNum != FILE_IDENT)
         {
            return false;
         }

         int numBlocks = 0;
         ds.Read(numBlocks);

         if (numBlocks != mNumBlocks)
         {
            LOG_ERROR("Unexpected number of voxel blocks in visibility file.");
         }

         mBlockVisibility.resize(mNumBlocks);

         for (int i = 0; i < mNumBlocks; ++i)
         {
            bool b = false;
            ds.Read(b);

            mBlockVisibility[i] = b;
         }
         
         inFile.close();
         
         return !inFile.fail();
      }
      else
      {
         LOG_ERROR("Error reading visibility data.");
         return false;
      }
   }

   bool VoxelGrid::WriteBlockVisibility()
   {      
      std::string filePath = mFullPathToFileCache;
      std::string filename("VisibilityCache.dat");
      LOGN_DEBUG("voxelgrid.cpp", "Writing visibility cache to " + filePath + filename);
      std::ofstream outFile(filePath + filename, std::ios::out | std::ios::binary);
      
      if (!outFile.fail())
      {
         dtUtil::DataStream ds;
         ds.Write(START_END_CHAR);
         ds.Write(FILE_IDENT);

         ds.Write(mNumBlocks);

         for (int i = 0; i < mNumBlocks; ++i)
         {
            ds.Write(mBlockVisibility[i]);
         }
         
         outFile.write(ds.GetBuffer(), ds.GetBufferSize());

         outFile.close();
         
         return !outFile.fail();

      }
      else
      {
         LOG_ERROR("Error writing visibility data.");
         return false;
      }

   }

   void VoxelGrid::GenerateCacheString()
   {
      dtCore::Project& proj = dtCore::Project::GetInstance();
      dtUtil::FileUtils&  fileUtil = dtUtil::FileUtils::GetInstance();

      std::string projectPath = proj.GetContext();

      std::stringstream folderName;
      
      folderName << "/Volumes/cache/" << mVoxelActor->GetDatabase().GetResourceName();

      try{

         //std::cout << std::endl << "Creating directory " << folderName.str() << std::endl;
         fileUtil.MakeDirectory(projectPath + folderName.str());

         folderName << "/BlockDim_x" << mBlocksX << "_y" << mBlocksY << "_z" << mBlocksZ;
         //std::cout << "Creating directory " << folderName.str() << std::endl;
         fileUtil.MakeDirectory(projectPath + folderName.str());

         folderName << "/Offset_x" << int(mGridOffset.x()) << "_y" << int(mGridOffset.y()) << "_z" << int(mGridOffset.z());
         //std::cout << "Creating directory " << folderName.str() << std::endl;
         fileUtil.MakeDirectory(projectPath + folderName.str());

         folderName << "/Resolution_x" << int(mStaticResolution.x()) << "_y" << int(mStaticResolution.y()) << "_z" << int(mStaticResolution.z()) << "/";
         //std::cout << "Creating directory " << folderName.str() << std::endl;
         fileUtil.MakeDirectory(projectPath + folderName.str());

         mCacheFolder = folderName.str();
         mFullPathToFileCache = projectPath + folderName.str();

      }
      catch (dtUtil::Exception& e)
      {
         LOG_ERROR("Error generating cache directory for VoxelGrid." + e.What());
      }

   }

   bool VoxelGrid::WriteVoxelDatabase()
   {
      bool result = false;

      std::stringstream fileName;

      fileName << mFullPathToFileCache << DATABASE_FILENAME;

      LOGN_DEBUG("voxelgrid.cpp", "Writing voxel database to " + fileName.str());


      if (dtUtil::FileUtils::GetInstance().DirExists(mFullPathToFileCache))
      {
         result = osgDB::writeNodeFile(*mRootNode, fileName.str());

         LOGN_DEBUG("voxelgrid.cpp", "Writing master voxel database " + fileName.str());
      }

      return result;
   }

   bool VoxelGrid::ReadVoxelDatabase()
   {
      bool result = false;

      std::string filePath = mFullPathToFileCache;

      std::stringstream fileName;

      fileName << filePath << DATABASE_FILENAME;

      LOGN_DEBUG("voxelgrid.cpp", "Reading master voxel database." + fileName.str());

      if (dtUtil::FileUtils::GetInstance().FileExists(fileName.str()))
      {
         osg::Node* n = dtUtil::FileUtils::GetInstance().ReadNode(fileName.str());
         if (n != nullptr)
         {
            mRootNode->addChild(n);
            result = true;
         }
         else
         {
            LOG_ERROR("Error reading voxel database, read result = NULL.");
         }

      }
      else
      {
         LOG_ERROR("Error reading voxel database, file does not exist.");
      }

      return result;
   }

   void VoxelGrid::ResetGrid()
   {
      //cannot regenerate if it was never created 
      if (mVoxelActor.valid())
      {
         //clear all cells to be regenerated
         mDirtyCells.clear();
   
         //clear all children
         mRootNode->removeChildren(0, mRootNode->getNumChildren());

         for (int i = 0; i < mNumBlocks; ++i)
         {
            if (mBlocks[i].IsAllocated())
            {
               mBlocks[i].DeAllocate();

               osg::Vec3 offsetFrom = mBlocks[i].GetOffset();

               //osg::Vec3 offsetTo = offsetFrom + mBlockDimensions;

               mBlocks[i].Init(mBlockDimensions, offsetFrom, mCellDimensions);

               if (!mBlocks[i].LoadCachedModel(mFullPathToFileCache, i))
               {
                  LOG_ERROR("Unable to load model for previously allocated block.");
               }
               else
               {
                  mBlocks[i].SetEmpty(false);

                  mRootNode->addChild(mBlocks[i].GetOSGNode());
               }
            }
         }

         LOGN_DEBUG("voxelgrid.cpp", "Optimizing Grid");
         osgUtil::Optimizer opt;
         opt.optimize(mRootNode, osgUtil::Optimizer::SPATIALIZE_GROUPS);
         LOGN_DEBUG("voxelgrid.cpp", "Done Optimizing");
      }

   }

} /* namespace dtVoxel */
