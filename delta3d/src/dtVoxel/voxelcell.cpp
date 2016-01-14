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

#include <dtVoxel/voxelcell.h>
#include <osgVolume/VolumeTile>
#include <osgVolume/MultipassTechnique>
#include <osgVolume/RayTracedTechnique>
#include <osgVolume/FixedFunctionTechnique>
#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osgUtil/Simplifier>

#include <dtVoxel/marchingcubes.h>

#include <dtVoxel/aabbintersector.h>

#include <dtUtil/mathdefines.h>
#include <dtUtil/log.h>

#include <tbb/parallel_for.h>
#include <tbb/blocked_range3d.h>
#include <tbb/mutex.h>
#include <tbb/task_scheduler_init.h>

#include <dtCore/timer.h>

namespace dtVoxel
{
   int HashVec3(std::map<osg::Vec3, int>& vectorMap, const osg::Vec3& vec, bool& inserted)
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

   //this is always 1 because the actual values are interploated from 0-1 using the iso value property now
   float isolevel = 1.0f;

   CreateMeshTask::CreateMeshTask(const osg::Vec3& offset, const osg::Vec3& texelSize, const osg::Vec3i& resolution, double isolevel, openvdb::FloatGrid::Ptr grid)
      : mIsDone(false)
      , mIsoLevel(isolevel)
      , mOffset(offset)
      , mTexelSize(texelSize)
      , mResolution(resolution)
      , mMesh(new osg::Geode())
      , mGrid(grid)
   {
   }

   bool CreateMeshTask::IsDone() const
   {
      return mIsDone;
   }


   osg::Geode* CreateMeshTask::TakeGeometry()
   {
      return mMesh.release();
   }

   void CreateMeshTask::operator()()
   {
      unsigned blockSize = 2U;
      tbb::task_scheduler_init init(blockSize);

      dtCore::Timer_t startTime = dtCore::Timer::Instance()->Tick();
      dtCore::RefPtr<osg::Geometry> geom = new osg::Geometry();
      dtCore::RefPtr<osg::Vec3Array> vertArray = new osg::Vec3Array();
      dtCore::RefPtr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(GL_TRIANGLES);
      
      tbb::mutex /*hashMtx,*/ elemMtx;

      tbb::parallel_for(tbb::blocked_range3d<int>(0, mResolution[0], 1U, 0, mResolution[1], 16U, 0, mResolution[2],  9U),
            [&](const tbb::blocked_range3d<int>& r)
            {
         openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler> sampler(mGrid->getConstAccessor(), mGrid->transform());
         // It may not be as efficient for rendering to have one per thread, but having to lock a shared kills performance.
         std::map<osg::Vec3, int> mVectorMap;

         dtCore::RefPtr<osg::Vec3Array> vertArraySub = new osg::Vec3Array();
         dtCore::RefPtr<osg::DrawElementsUInt> drawElementsSub = new osg::DrawElementsUInt(GL_TRIANGLES);

         //reusing this improves performance by quite a bit
         osg::Vec3 vertlist[12];
         GRIDCELL grid;
         TRIANGLE triangles[5];

         for (int i = r.pages().begin(); i < r.pages().end(); ++i)
         {
            for (int j = r.rows().begin(); j < r.rows().end(); ++j)
            {
               for (int k = r.cols().begin(); k < r.cols().end(); ++k)
               {
                  double worldX = mOffset[0] + (i * mTexelSize[0]);
                  double worldY = mOffset[1] + (j * mTexelSize[1]);
                  double worldZ = mOffset[2] + (k * mTexelSize[2]);

                  osg::Vec3 from(worldX, worldY, worldZ);

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


                  for (int n = 0; n < numTriangles; ++n)
                  {
                     for (int i = 0; i < 3; ++i)
                     {
                        bool inserted = false;
                        int index = -1;
                        {
                           index = HashVec3(mVectorMap, triangles[n].p[i], inserted);
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
         tbb::mutex::scoped_lock sl(elemMtx);
         
         //this seems to cause a performance bottleneck in VS2013 x64
         vertArray->reserve(vertArray->size() +  vertArraySub->size());
         
         int startingIdx = vertArray->size();
         vertArray->insert(vertArray->end(), vertArraySub->begin(), vertArraySub->end());
         
         //this seems to cause a performance bottleneck in VS2013 x6
         drawElements->reserve(drawElements->size() +  drawElementsSub->size());
         
         for (auto itr = drawElementsSub->begin(), itrEnd = drawElementsSub->end(); itr != itrEnd; ++itr)
         {
            drawElements->push_back((*itr) + startingIdx);
         }
         //drawElements->insert(drawElements->end(), drawElementsSub->begin(), drawElementsSub->end());
            });
      //std::cout << "Num Verts " << vertArray->getNumElements() << std::endl;

      
      geom->setVertexArray(vertArray);
      geom->addPrimitiveSet(drawElements);

      mMesh->addDrawable(geom);

      mIsDone = true;
      LOGN_DEBUG("voxelcell.cpp", "Time to update cell ms: " + dtUtil::ToString(dtCore::Timer::Instance()->DeltaMil(startTime, dtCore::Timer::Instance()->Tick())));
   }

   double CreateMeshTask::SampleCoord(double x, double y, double z, openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler>& fastSampler)
   {
      double result = (fastSampler.wsSample(openvdb::Vec3R(x, y, z)));
      
      dtUtil::Clamp(result, 0.0, mIsoLevel);

      result = dtUtil::MapRangeValue(result, 0.0, mIsoLevel, 0.0, 1.0);
      return result;
   }

   class VoxelCellImpl
   {
   public:
      VoxelCellImpl()
         : mIsAllocated(false)
         , mIsDirty(false)
      {

      }

      bool mIsAllocated;
      bool mIsDirty;

      osg::Vec3 mOffset;
      dtCore::RefPtr<osg::Group> mMeshNode;
      dtCore::RefPtr<CreateMeshTask> mCreateMeshTask;

      std::map<osg::Vec3, int> mVectorMap;

      dtCore::RefPtr<osgVolume::ImageLayer> mImage;
      dtCore::RefPtr<osgVolume::VolumeTile> mVolumeTile;
   };

   VoxelCell::VoxelCell()
      : mImpl(new VoxelCellImpl())
   {
   }

   VoxelCell::~VoxelCell()
   {
      delete mImpl;
      mImpl = NULL;
   }


   bool VoxelCell::IsDirty() const
   {
      return mImpl->mIsDirty;
   }

   void VoxelCell::SetDirty(bool b)
   {
      mImpl->mIsDirty = b;
   }

   void VoxelCell::SetAllocated(bool b)
   {
      mImpl->mIsAllocated = b;
   }

   bool VoxelCell::IsAllocated() const
   {
      return mImpl->mIsAllocated;
   }

   void VoxelCell::DeAllocate()
   {
      //std::cout << "DeAllocating Voxel Cell " << std::endl;

      mImpl->mMeshNode = NULL;
      mImpl->mImage = NULL;
      mImpl->mVolumeTile = NULL;

      mImpl->mIsAllocated = false;
   }

   double VoxelCell::SampleCoord(double x, double y, double z, double isovalue, openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler>& fastSampler)
   {
      double result = (fastSampler.wsSample(openvdb::Vec3R(x, y, z)));
      dtUtil::Clamp(result, 0.0, isovalue);
      
      result = dtUtil::MapRangeValue(result, 0.0, isovalue, 0.0, 1.0);
      return result;
   }

   void VoxelCell::AddGeometry(VoxelActor& voxelActor, osg::Matrix& transform, const osg::Vec3& cellSize, const osg::Vec3i& resolution, osg::Vec3Array* vertArray, osg::DrawElementsUInt* drawElements)
   {
      mImpl->mOffset = transform.getTrans();

      osg::Vec3 texelSize(cellSize[0] / float(resolution[0]), cellSize[1] / float(resolution[1]), cellSize[2] / float(resolution[2]));

      openvdb::FloatGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::FloatGrid>(voxelActor.GetGrid(0));
      //openvdb::FloatGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::FloatGrid>(localGrid);

      openvdb::FloatGrid::ConstAccessor accessor = gridB->getConstAccessor();

      openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler>
         fastSampler(accessor, gridB->transform());

      bool inserted = false;
      //Insert items into map
      for (unsigned i = 0; i < vertArray->getNumElements(); ++i)
      {
         osg::Vec3 pos = vertArray->operator[](i);
         
         HashVec3(mImpl->mVectorMap, pos, inserted);
      }

      //reusing this improves performance by quite a bit 
      osg::Vec3 vertlist[12];
      double isovalue = voxelActor.GetIsoLevel();

      for (int i = 0; i < resolution[0]; ++i)
      {
         for (int j = 0; j < resolution[1]; ++j)
         {
            for (int k = 0; k < resolution[2]; ++k)
            {
               double worldX = mImpl->mOffset[0] + (i * texelSize[0]);
               double worldY = mImpl->mOffset[1] + (j * texelSize[1]);
               double worldZ = mImpl->mOffset[2] + (k * texelSize[2]);

               osg::Vec3 from(worldX, worldY, worldZ);

               GRIDCELL grid;
               TRIANGLE triangles[5];

               grid.p[0] = from;
               grid.val[0] = SampleCoord(grid.p[0].x(), grid.p[0].y(), grid.p[0].z(), isovalue, fastSampler);

               grid.p[1].set(from[0] + texelSize[0], from[1], from[2]);
               grid.val[1] = SampleCoord(grid.p[1].x(), grid.p[1].y(), grid.p[1].z(), isovalue, fastSampler);

               grid.p[2].set(from[0] + texelSize[0], from[1] + texelSize[1], from[2]);
               grid.val[2] = SampleCoord(grid.p[2].x(), grid.p[2].y(), grid.p[2].z(), isovalue, fastSampler);

               grid.p[3].set(from[0], from[1] + texelSize[1], from[2]);
               grid.val[3] = SampleCoord(grid.p[3].x(), grid.p[3].y(), grid.p[3].z(), isovalue, fastSampler);

               grid.p[4].set(from[0], from[1], from[2] + texelSize[2]);
               grid.val[4] = SampleCoord(grid.p[4].x(), grid.p[4].y(), grid.p[4].z(), isovalue, fastSampler);

               grid.p[5].set(from[0] + texelSize[0], from[1], from[2] + texelSize[2]);
               grid.val[5] = SampleCoord(grid.p[5].x(), grid.p[5].y(), grid.p[5].z(), isovalue, fastSampler);

               grid.p[6].set(from[0] + texelSize[0], from[1] + texelSize[1], from[2] + texelSize[2]);
               grid.val[6] = SampleCoord(grid.p[6].x(), grid.p[6].y(), grid.p[6].z(), isovalue, fastSampler);

               grid.p[7].set(from[0], from[1] + texelSize[1], from[2] + texelSize[2]);
               grid.val[7] = SampleCoord(grid.p[7].x(), grid.p[7].y(), grid.p[7].z(), isovalue, fastSampler);

               int numTriangles = PolygonizeCube(grid, isolevel, triangles, &vertlist[0]);

               for (int n = 0; n < numTriangles; ++n)
               {
                  for (int i = 0; i < 3; ++i)
                  {
                     bool added = false;
                     int index = HashVec3(mImpl->mVectorMap, triangles[n].p[i], added);

                     if (added)
                     {
                        vertArray->push_back(triangles[n].p[i]);
                     }

                     drawElements->addElement(index);
                  }
               }
            }
         }
      }

      //Clear Map
      mImpl->mVectorMap.clear();
   }

   void VoxelCell::CreateMeshWithTask(VoxelActor& voxelActor, osg::Matrix& transform, const osg::Vec3& cellSize, const osg::Vec3i& resolution)
   {
      {
         mImpl->mOffset = transform.getTrans();

         osg::Vec3 texelSize(cellSize[0] / float(resolution[0]), cellSize[1] / float(resolution[1]), cellSize[2] / float(resolution[2]));


         //openvdb::FloatGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::FloatGrid>(localGrid);
         openvdb::FloatGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::FloatGrid>(voxelActor.GetGrid(0));


         mImpl->mCreateMeshTask = new CreateMeshTask(mImpl->mOffset, texelSize, resolution, voxelActor.GetIsoLevel(), gridB);

      }
   }

   bool VoxelCell::RunTask(bool allowBackgroundThreading)
   {
      if (!mImpl->mCreateMeshTask->IsDone())
      {
         if (allowBackgroundThreading)
         {
            dtUtil::ThreadPool::AddTask(*mImpl->mCreateMeshTask, dtUtil::ThreadPool::BACKGROUND);
         }
         else
         {
            mImpl->mCreateMeshTask->operator ()();
         }
         return true;
      }
      return false;
   }

   void VoxelCell::TakeGeometry()
   {
      mImpl->mMeshNode = new osg::Group();

      mImpl->mMeshNode->addChild(mImpl->mCreateMeshTask->TakeGeometry());

      mImpl->mIsAllocated = true;
   }

   bool VoxelCell::CheckTaskStatus()
   {
      if (mImpl->mCreateMeshTask->IsDone())
      {
         if (mImpl->mCreateMeshTask->WaitUntilComplete(2))
         {
            return true;
         }
      }
      else
      {
         if (mImpl->mCreateMeshTask->WaitUntilComplete(-1))
         {
            return true;
         }
      }
      return false;   
   }


   void VoxelCell::CreateMesh(VoxelActor& voxelActor, osg::Matrix& transform, const osg::Vec3& cellSize, const osg::Vec3i& resolution)
   {
      //static int mesh_count = 0;

      //std::cout << "Creating Voxel Cell " << mesh_count++ << std::endl;

      mImpl->mMeshNode = new osg::Group();

      dtCore::RefPtr<osg::Geometry> geom = new osg::Geometry();
      dtCore::RefPtr<osg::Vec3Array> vertArray = new osg::Vec3Array();
      dtCore::RefPtr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(GL_TRIANGLES);

      mImpl->mOffset = transform.getTrans();
            
      AddGeometry(voxelActor, transform, cellSize, resolution, vertArray, drawElements);
      
      geom->setVertexArray(vertArray);      
      geom->addPrimitiveSet(drawElements);

      dtCore::RefPtr<osg::Geode> geode = new osg::Geode();
      geode->addDrawable(geom);

      mImpl->mMeshNode->addChild(geode);

      if (voxelActor.GetSimplify())
      {
         dtCore::RefPtr<osgUtil::Simplifier> simplifier = new osgUtil::Simplifier();
         simplifier->setSampleRatio(voxelActor.GetSampleRatio());
         simplifier->setDoTriStrip(false);
         mImpl->mMeshNode->accept(*simplifier);
      }

      mImpl->mIsAllocated = true;
   }

   void VoxelCell::CreateImage(VoxelActor& voxelActor, openvdb::GridBase::Ptr localGrid, osg::Matrix& transform, const osg::Vec3& cellSize, const osg::Vec3i& texture_resolution)
   {
       static int count = 0;

       std::cout << "Creating Voxel Cell " << count++ << std::endl;

       mImpl->mVolumeTile = new osgVolume::VolumeTile;

       dtCore::RefPtr<osgVolume::Locator> locator = new osgVolume::Locator();
       mImpl->mVolumeTile->setLocator(locator.get());

       osg::Vec3 pos = transform.getTrans();

       locator->setTransformAsExtents(
          pos.x() + (-cellSize.x() / 2.0), pos.y() + (-cellSize.y() / 2.0),
          pos.x() + (cellSize.x() / 2.0), pos.y() + (cellSize.y() / 2.0),
          pos.z() + (-cellSize.z() / 2.0), pos.z() + (cellSize.z() / 2.0));


       AllocateImage(voxelActor, localGrid, cellSize, texture_resolution[0], texture_resolution[1], texture_resolution[2]);

       mImpl->mImage->setLocator(locator.get());
       mImpl->mVolumeTile->setLayer(mImpl->mImage.get());
       
       osg::ref_ptr<osgVolume::AlphaFuncProperty> ap = new
           osgVolume::AlphaFuncProperty(0.02f);
       
       osg::ref_ptr<osgVolume::TransparencyProperty> tp = new
           osgVolume::TransparencyProperty(1.0f);

       osg::ref_ptr<osgVolume::SampleRatioProperty> sr =
           new osgVolume::SampleRatioProperty(1.0);


       osgVolume::IsoSurfaceProperty* isop = new osgVolume::IsoSurfaceProperty(1.0);

       //osg::ref_ptr<osgVolume::ExteriorTransparencyFactorProperty> etfp = new
         //  osgVolume::ExteriorTransparencyFactorProperty(0.0f);

       osg::ref_ptr<osgVolume::CompositeProperty> cp = new
           osgVolume::CompositeProperty;

       //cp->addProperty(new osgVolume::LightingProperty);

       cp->addProperty(ap.get());
       cp->addProperty(tp.get());
       cp->addProperty(sr.get());
       cp->addProperty(isop);
       //cp->addProperty(etfp.get());

       mImpl->mImage->addProperty(cp.get());
       //mImage->setDefaultValue(osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f));

       mImpl->mVolumeTile->setVolumeTechnique(new osgVolume::MultipassTechnique());


       //create hull
       //osg::Group* hullRoot = new osg::Group();
       //osg::Box* box = new osg::Box(pos, cellSize.x(), cellSize.y(), cellSize.z());
       //osg::ShapeDrawable* sd = new osg::ShapeDrawable(box);
       //osg::Geode* sdg = new osg::Geode();
       //sdg->addDrawable(sd);
       //hullRoot->addChild(sdg);
       //mVolumeTile->addChild(hullRoot);

   }

   void VoxelCell::AllocateImage(VoxelActor& voxelActor, openvdb::GridBase::Ptr gridPtr, const osg::Vec3& cellSize, int width, int height, int slices)
   {
       dtCore::RefPtr<osg::Image> image = new osg::Image;

       GLenum pixelFormat = GL_RGBA;  // GL_ALPHA, GL_LUMINANCE, GL_RGB or GL_RGBA
       GLenum dataType = GL_UNSIGNED_BYTE;
       int components = osg::Image::computeNumComponents(pixelFormat);


       // Make sure 0's are written in the image's data segment
       int imageSize = width * height * slices * components;
       unsigned char* dataPtr = new unsigned char[imageSize], *ptr;
       unsigned char charToFill = 0;
       memset(dataPtr, charToFill, imageSize);
       
       openvdb::FloatGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::FloatGrid>(gridPtr);

       openvdb::FloatGrid::ConstAccessor accessor = gridB->getConstAccessor();
       
       openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::BoxSampler>
          fastSampler(accessor, gridB->transform());
       
       const osg::Matrixd& mat = mImpl->mVolumeTile->getLocator()->getTransform();
       osg::Vec3 offset = mat.getTrans();
       
       std::cout << "Cell Offset: " << offset[0] << " " << offset[1] << " " << offset[2] << std::endl;

       osg::Vec3 texelSize(cellSize[0] / float(width), cellSize[1] / float(height), cellSize[2] / float(slices));
       //osg::Vec3 halfTexel = texelSize * 0.5;
              
       AABBIntersector<openvdb::FloatGrid> aabb(boost::dynamic_pointer_cast<openvdb::FloatGrid>(gridPtr));
       
       //this one we will increment as we set
       ptr = dataPtr;
       
       int numCellsVacant = 0;

       for (int i = 0; i < slices; ++i)
       {
           for (int j = 0; j < height; ++j)
           {
               for (int k = 0; k < width; ++k)
               {
                  double worldX = offset[0] + (k * texelSize[0]);
                  double worldY = offset[1] + (j * texelSize[1]);
                  double worldZ = offset[2] + (i * texelSize[2]);

                  osg::Vec3 from(worldX, worldY, worldZ);
                  osg::Vec3 to = from + texelSize;

                  osg::BoundingBox bb(from, to);

                  openvdb::BBoxd bbox(openvdb::Vec3d(bb.xMin(), bb.yMin(), bb.zMin()), openvdb::Vec3d(bb.xMax(), bb.yMax(), bb.zMax()));

                  //openvdb::GridBase::Ptr gridPtr = voxelActor.CollideWithAABB(bb);

                  aabb.SetWorldBB(bbox);
                  bool hasData = aabb.HasDataInAABB();
                  //openvdb::GridBase::Ptr grid2Ptr = aabb.GetHits();


                  if (hasData)
                  {
                     *(ptr++) = (unsigned char)255;// voxel;
                     *(ptr++) = (unsigned char)255;// voxel;
                     *(ptr++) = (unsigned char)255;// voxel;
                     *(ptr++) = (unsigned char)255;// voxel;
                  }
                  else
                  {
                     *(ptr++) = (unsigned char)0;// voxel;
                     *(ptr++) = (unsigned char)0;// voxel;
                     *(ptr++) = (unsigned char)0;// voxel;
                     *(ptr++) = (unsigned char)0;// voxel;

                     ++numCellsVacant;
                  }
               }
           }
       }

       std::cout << "Percent Cells Vacant = " << float(numCellsVacant) / float(slices * height * width) << std::endl;

       image->setImage(width, height, slices, pixelFormat, pixelFormat, dataType,
           dataPtr, osg::Image::USE_NEW_DELETE);

       mImpl->mImage = new osgVolume::ImageLayer(image.get());
   }
   
   osg::Node* VoxelCell::GetOSGNode()
   {
      if (mImpl->mMeshNode.valid()) return mImpl->mMeshNode;
      else return mImpl->mVolumeTile.get();
   }

   const osg::Node* VoxelCell::GetOSGNode() const
   {
      if (mImpl->mMeshNode.valid()) return mImpl->mMeshNode;
      else return mImpl->mVolumeTile.get();
   }

   osg::Vec3 VoxelCell::GetOffset() const
   {
      return mImpl->mOffset;
   }


} /* namespace dtVoxel */
