/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2016, Caper Holdings, LLC
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

#include <dtVoxel/createmeshtask.h>
#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/Geode>

#include <dtUtil/mathdefines.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>

#include <tbb/parallel_for.h>
#include <tbb/blocked_range3d.h>
#include <tbb/mutex.h>
#include <tbb/task_scheduler_init.h>

#include <dtCore/timer.h>

namespace dtVoxel
{
   int HashVec3WithMap(std::map<osg::Vec3, int>& vectorMap, const osg::Vec3& vec, bool& inserted)
   {
      inserted = true;
      int index = vectorMap.size();
      auto insertResult = vectorMap.insert(std::make_pair(vec, index));
      //if insert fails
      if (!insertResult.second)
      {
         //take the existing index
         index = insertResult.first->second;
         inserted = false;
      }

      return index;
   }   

   CreateMeshTask::CreateMeshTask(const osg::Vec3& offset, const osg::Vec3& texelSize, const osg::Vec3i& resolution, double isolevel, openvdb::FloatGrid::Ptr grid)
      : mSkipBackFaces(true)
      , mCacheTriangleData(true)
      , mNumThreads(2)
      , mIsDone(false)     
      , mMode(Default)
      , mUseCache(false)
      , mUseBoundingBox(false)
      , mTime(0.0)
      , mIsoLevel(isolevel)
      , mOffset(offset)
      , mTexelSize(texelSize)
      , mResolution(resolution)
      , mDirtyBounds()
      , mMesh(new osg::Geode())
      , mGrid(grid)
      , mCacheData()
   {
      if (mCacheTriangleData)
      {
         mCacheData.resize(mResolution[0] * mResolution[1] * mResolution[2]);
         mOccupancy.resize(mResolution[0] * mResolution[1] * mResolution[2], 0);
      }
   }

   CreateMeshTask::~CreateMeshTask()
   {
      mMesh = nullptr;
      mGrid = nullptr;
      mOccupancy.clear();
      mCacheData.clear();
   }

   bool CreateMeshTask::IsDone() const
   {
      return mIsDone;
   }

   double CreateMeshTask::GetTime() const
   {
      return mTime;
   }

   osg::Geode* CreateMeshTask::TakeGeometry()
   {
      return mMesh.release();
   }

   void CreateMeshTask::operator()()
   {
      switch (mMode)
      {
      case RunInSingleThread:
         RunSingleThreaded();
         break;

      case Default:
      case UseSetNumThreads:
      case UseMaxThreads:
         RunMultiThreads();
         break;      
      }
   }

   void CreateMeshTask::RunSingleThreaded()
   {
      dtCore::Timer_t startTime = dtCore::Timer::Instance()->Tick();

      openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler> sampler(mGrid->getConstAccessor(), mGrid->transform());

      TRIANGLE triangles[5];

      std::map<osg::Vec3, int> vectorMap;

      dtCore::RefPtr<osg::Geometry> geom = new osg::Geometry();
      dtCore::RefPtr<osg::Vec3Array> vertArray = new osg::Vec3Array();
      dtCore::RefPtr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(GL_TRIANGLES);

      for (int i = 0; i < mResolution[0]; ++i)
      {
         for (int j = 0; j < mResolution[1]; ++j)
         {
            for (int k = 0; k < mResolution[2]; ++k)
            {
               int numTriangles = SampleSingleCell(i, j, k, sampler, triangles);

               for (int n = 0; n < numTriangles; ++n)
               {
                  if (mSkipBackFaces && triangles[n].n[0].z() < 0.0 && triangles[n].n[1].z() < 0.0 && triangles[n].n[2].z() < 0.0)
                  {
                     //skipping triangle 
                  }
                  else
                  {
                     for (int i = 0; i < 3; ++i)
                     {
                        bool inserted = false;
                        int numVerts = vertArray->size();
                        int index = HashVec3WithMap(vectorMap, triangles[n].p[i], inserted);

                        if (inserted)
                        {
                           vertArray->push_back(triangles[n].p[i]);
                        }

                        drawElements->addElement(index);
                     }
                  }
               }
            }
         }
      }

      //std::cout << "Num Verts " << vertArray->getNumElements() << std::endl;


      geom->setVertexArray(vertArray);
      geom->addPrimitiveSet(drawElements);

      mMesh->addDrawable(geom);

      vectorMap.clear();
      mIsDone = true;
   
      mTime = dtCore::Timer::Instance()->DeltaMil(startTime, dtCore::Timer::Instance()->Tick());
      LOGN_DEBUG("createmeshtask.cpp", "Time to update cell ms: " + dtUtil::ToString(mTime));
   }

   void CreateMeshTask::RunMultiThreads()
   {
      int trianglesSkipped = 0;
      if (mMode == UseMaxThreads)
      {
         tbb::task_scheduler_init init();
      }
      else
      {
         tbb::task_scheduler_init init(mNumThreads);
      }

      dtCore::Timer_t startTime = dtCore::Timer::Instance()->Tick();
      dtCore::RefPtr<osg::Geometry> geom = new osg::Geometry();
      dtCore::RefPtr<osg::Vec3Array> vertArray = new osg::Vec3Array();
      dtCore::RefPtr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(GL_TRIANGLES);

      tbb::mutex /*hashMtx,*/ elemMtx;

      tbb::parallel_for(tbb::blocked_range3d<int>(0, mResolution[0], 1U, 0, mResolution[1], 16U, 0, mResolution[2], 9U),
         [&](const tbb::blocked_range3d<int>& r)
      {
         openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler> sampler(mGrid->getConstAccessor(), mGrid->transform());
         // It may not be as efficient for rendering to have one per thread, but having to lock a shared kills performance.
         std::map<osg::Vec3, int> vectorMap;

         dtCore::RefPtr<osg::Vec3Array> vertArraySub = new osg::Vec3Array();
         dtCore::RefPtr<osg::DrawElementsUInt> drawElementsSub = new osg::DrawElementsUInt(GL_TRIANGLES);

         int numTrisDropped = 0;

         //this is always 1 because the actual values are interploated from 0-1 using the iso value property now
         const float isolevel = 1.0f;

         //reusing this improves performance by quite a bit
         osg::Vec3 vertlist[12];
         GRIDCELL grid;
         TRIANGLE triangles[5];

         for (int k = r.cols().begin(); k < r.cols().end(); ++k)
         {
            for (int j = r.rows().begin(); j < r.rows().end(); ++j)
            {
               for (int i = r.pages().begin(); i < r.pages().end(); ++i)
               {
                  int numTriangles = SampleSingleCell(i, j, k, sampler, triangles);

                  for (int n = 0; n < numTriangles; ++n)
                  {
                     if (mSkipBackFaces && triangles[n].n[0].z() < 0.0 && triangles[n].n[1].z() < 0.0 && triangles[n].n[2].z() < 0.0)
                     {
                        //skipping triangle 
                        ++numTrisDropped;
                     }
                     else
                     {
                        for (int i = 0; i < 3; ++i)
                        {
                           bool inserted = false;
                           int index = -1;
                           {
                              index = HashVec3WithMap(vectorMap, triangles[n].p[i], inserted);
                              if (inserted)
                              {
                                 vertArraySub->push_back(triangles[n].p[i]);
                                 if ((vertArraySub->size() - 1) != unsigned(index))
                                 {
                                    printf("Error! %lu %d\n", vertArraySub->size(), index);
                                 }
                              }
                           }

                           drawElementsSub->addElement(index);

                        }
                     }
                  }
               }
            }
         }
         tbb::mutex::scoped_lock sl(elemMtx);

         //this seems to cause a performance bottleneck in VS2013 x64
         vertArray->reserve(vertArray->size() + vertArraySub->size());

         int startingIdx = vertArray->size();
         vertArray->insert(vertArray->end(), vertArraySub->begin(), vertArraySub->end());

         //this seems to cause a performance bottleneck in VS2013 x64
         drawElements->reserve(drawElements->size() + drawElementsSub->size());

         for (auto itr = drawElementsSub->begin(), itrEnd = drawElementsSub->end(); itr != itrEnd; ++itr)
         {
            drawElements->push_back((*itr) + startingIdx);
         }

         trianglesSkipped += numTrisDropped;
         //drawElements->insert(drawElements->end(), drawElementsSub->begin(), drawElementsSub->end());
      });
      //std::cout << "Num Verts " << vertArray->getNumElements() << std::endl;


      geom->setVertexArray(vertArray);
      geom->addPrimitiveSet(drawElements);

      mMesh->addDrawable(geom);

      if (mCacheTriangleData)
      {
         //setup to use the cache next time through
         mUseCache = true;
      }

      mIsDone = true;
      mTime = dtCore::Timer::Instance()->DeltaMil(startTime, dtCore::Timer::Instance()->Tick());
      LOGN_DEBUG("createmeshtask.cpp", "Time to update cell ms: " + dtUtil::ToString(mTime));
      //LOGN_DEBUG("createmeshtask.cpp", "Num Backfaces Removed " + dtUtil::ToString(trianglesSkipped));
   }

   double CreateMeshTask::SampleCoord(double x, double y, double z, openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler>& fastSampler)
   {
      double result = (fastSampler.wsSample(openvdb::Vec3R(x, y, z)));
      
      dtUtil::Clamp(result, 0.0, mIsoLevel);

      result = dtUtil::MapRangeValue(result, 0.0, mIsoLevel, 0.0, 1.0);
      return result;
   }

   int CreateMeshTask::ComputeIndex(int i, int j, int k) const
   {
      return (k * mResolution[1] * mResolution[0]) + (j * mResolution[0]) + i;
   }

   int CreateMeshTask::SampleSingleCell(int i, int j, int k, openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler>& sampler, TRIANGLE* triangles)
   {  
      //this is always 1 because the actual values are interploated from 0-1 using the iso value property now
      const float isolevel = 1.0f;

      int index = ComputeIndex(i, j, k);
      
      double worldX = mOffset[0] + (i * mTexelSize[0]);
      double worldY = mOffset[1] + (j * mTexelSize[1]);
      double worldZ = mOffset[2] + (k * mTexelSize[2]);
      
      osg::Vec3 from(worldX, worldY, worldZ);


      if (mUseCache)
      {
         if (!mUseBoundingBox)
         {         
            LOGN_WARNING("createmeshtask.cpp", "No Bounding Box set, but task is attempting to load from cache.");
         }
         else
         {
            osg::Vec3 origin(worldX, worldY, worldZ);
            osg::BoundingBox cellBB(from - (mTexelSize * 1.5), from + (mTexelSize * 2.0));
            
            //only read the cached data if we did not modify this cell
            if (!mDirtyBounds.intersects(cellBB))
            {
               int numTriangles = mOccupancy[index];

               CopyTriangleData(&(mCacheData[index].mTris[0]), triangles, numTriangles);

               //exit out early with cached data
               return numTriangles;
            }            
            else
            {
               //continue and regenerate cached data
            }
         }
      }


      GRIDCELL grid;
      osg::Vec3 vertlist[12];

      grid.p[0] = from;
      grid.val[0] = SampleCoord(grid.p[0].x(), grid.p[0].y(), grid.p[0].z(), sampler);

      grid.p[1].set(from[0] + mTexelSize[0], from[1], from[2]);
      grid.val[1] = SampleCoord(grid.p[1].x(), grid.p[1].y(), grid.p[1].z(), sampler);
 
      grid.p[2].set(from[0] + mTexelSize[0], from[1] + mTexelSize[1], from[2]);
      grid.val[2] = SampleCoord(grid.p[2].x(), grid.p[2].y(), grid.p[2].z(), sampler);

      grid.p[3].set(from[0], from[1] + mTexelSize[1], from[2]);
      grid.val[3] = SampleCoord(grid.p[3].x(), grid.p[3].y(), grid.p[3].z(), sampler);

      grid.p[4].set(from[0], from[1], from[2] + mTexelSize[2]);
      grid.val[4] = SampleCoord(grid.p[4].x(), grid.p[4].y(), grid.p[4].z(), sampler);

      grid.p[5].set(from[0] + mTexelSize[0], from[1], from[2] + mTexelSize[2]);
      grid.val[5] = SampleCoord(grid.p[5].x(), grid.p[5].y(), grid.p[5].z(), sampler);

      grid.p[6].set(from[0] + mTexelSize[0], from[1] + mTexelSize[1], from[2] + mTexelSize[2]);
      grid.val[6] = SampleCoord(grid.p[6].x(), grid.p[6].y(), grid.p[6].z(), sampler);

      grid.p[7].set(from[0], from[1] + mTexelSize[1], from[2] + mTexelSize[2]);
      grid.val[7] = SampleCoord(grid.p[7].x(), grid.p[7].y(), grid.p[7].z(), sampler);


      int numTriangles = PolygonizeCube(grid, isolevel, triangles, &vertlist[0]);
      
      if (mCacheTriangleData)
      {
         mOccupancy[index] = numTriangles;
         
         if (numTriangles > 0)
         {
            CopyTriangleData(triangles, &(mCacheData[index].mTris[0]), numTriangles);
         }
      }

      return numTriangles;
   }

   void CreateMeshTask::CopyTriangleData(const TRIANGLE* triData, TRIANGLE* toFill, int numTris)
   {
      for (int cachedTriangle = 0; cachedTriangle < numTris; ++cachedTriangle)
      {
         for (int vertNum = 0; vertNum < 3; ++vertNum)
         {
            //copy the normal and position for each vertex of each triangle
            toFill[cachedTriangle].p[vertNum].set(triData[cachedTriangle].p[vertNum]);
            toFill[cachedTriangle].n[vertNum].set(triData[cachedTriangle].n[vertNum]);
         }
      }
   }

   void CreateMeshTask::UpdateWithBounds(const osg::BoundingBox& bb)
   {
      if (!mIsDone)
      {
         mDirtyBounds.expandBy(bb);
      }
      else
      {
         mDirtyBounds = bb;
         mMesh = new osg::Geode();
         mIsDone = false;
         mUseBoundingBox = true;
      }
   }

   
} /* namespace dtVoxel */
