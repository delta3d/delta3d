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
#include <openvdb/tools/Interpolation.h>

#include <iostream>

namespace dtVoxel
{

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
      dtCore::RefPtr<osg::Geode> mMeshNode;
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

   void VoxelCell::CreateMesh(VoxelActor& voxelActor, openvdb::GridBase::Ptr localGrid, osg::Matrix& transform, const osg::Vec3& cellSize, const osg::Vec3i& resolution)
   {
      static int mesh_count = 0;

      //std::cout << "Creating Voxel Cell " << mesh_count++ << std::endl;

      mImpl->mMeshNode = new osg::Geode();

      dtCore::RefPtr<osg::Geometry> geom = new osg::Geometry();
      dtCore::RefPtr<osg::Vec3Array> vertArray = new osg::Vec3Array();
      dtCore::RefPtr<osg::Vec3Array> normalArray = new osg::Vec3Array();
      dtCore::RefPtr<osg::Vec3Array> colorArray = new osg::Vec3Array();
      dtCore::RefPtr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(GL_TRIANGLES);

      mImpl->mOffset = transform.getTrans();
      
      osg::Vec3 texelSize(cellSize[0] / float(resolution[0]), cellSize[1] / float(resolution[1]), cellSize[2] / float(resolution[2]));


      openvdb::BoolGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::BoolGrid>(localGrid);

      openvdb::BoolGrid::ConstAccessor accessor = gridB->getConstAccessor();

      openvdb::tools::GridSampler<openvdb::BoolGrid::ConstAccessor, openvdb::tools::BoxSampler>
         fastSampler(accessor, gridB->transform());

      float isolevel = 1.0f;

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
               grid.val[0] = !fastSampler.wsSample(openvdb::Vec3R(grid.p[0].x(), grid.p[0].y(), grid.p[0].z()));

               grid.p[1].set(from[0] + texelSize[0], from[1], from[2]);
               grid.val[1] = !fastSampler.wsSample(openvdb::Vec3R(grid.p[1].x(), grid.p[1].y(), grid.p[1].z()));

               grid.p[2].set(from[0] + texelSize[0], from[1] + texelSize[1], from[2]);
               grid.val[2] = !fastSampler.wsSample(openvdb::Vec3R(grid.p[2].x(), grid.p[2].y(), grid.p[2].z()));

               grid.p[3].set(from[0], from[1] + texelSize[1], from[2]);
               grid.val[3] = !fastSampler.wsSample(openvdb::Vec3R(grid.p[3].x(), grid.p[3].y(), grid.p[3].z()));

               grid.p[4].set(from[0], from[1], from[2] + texelSize[2]);
               grid.val[4] = !fastSampler.wsSample(openvdb::Vec3R(grid.p[4].x(), grid.p[4].y(), grid.p[4].z()));

               grid.p[5].set(from[0] + texelSize[0], from[1], from[2] + texelSize[2]);
               grid.val[5] = !fastSampler.wsSample(openvdb::Vec3R(grid.p[5].x(), grid.p[5].y(), grid.p[5].z()));

               grid.p[6].set(from[0] + texelSize[0], from[1] + texelSize[1], from[2] + texelSize[2]);
               grid.val[6] = !fastSampler.wsSample(openvdb::Vec3R(grid.p[6].x(), grid.p[6].y(), grid.p[6].z()));

               grid.p[7].set(from[0], from[1] + texelSize[1], from[2] + texelSize[2]);
               grid.val[7] = !fastSampler.wsSample(openvdb::Vec3R(grid.p[7].x(), grid.p[7].y(), grid.p[7].z()));

               int numTriangles = PolygonizeCube(grid, isolevel, triangles);

               for (int n = 0; n < numTriangles; ++n)
               {
                  drawElements->addElement(vertArray->size());
                  vertArray->push_back(triangles[n].p[0]);

                  drawElements->addElement(vertArray->size());
                  vertArray->push_back(triangles[n].p[1]);

                  drawElements->addElement(vertArray->size());
                  vertArray->push_back(triangles[n].p[2]);

                  normalArray->push_back(triangles[n].n[0]);
                  normalArray->push_back(triangles[n].n[1]);
                  normalArray->push_back(triangles[n].n[2]);

                  colorArray->push_back(osg::Vec3(1.0f, 1.0f, 1.0f));
               }
            }
         }
      }

      //std::cout << "Num Triangles Before Simplifier " << vertArray->size() << std::endl;
      
      geom->setVertexArray(vertArray);      
      geom->setColorArray(colorArray);
      geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      geom->addPrimitiveSet(drawElements);
      
      mImpl->mMeshNode->addDrawable(geom);

      dtCore::RefPtr<osgUtil::Simplifier> simplifier = new osgUtil::Simplifier();
      simplifier->setMaximumLength(100.0f);
      simplifier->setDoTriStrip(true);
      mImpl->mMeshNode->accept(*simplifier);

      //std::cout << "Num Triangles After Simplifier " << geom->getVertexArray()->getNumElements() << std::endl;

      mImpl->mMeshNode->accept(*simplifier);

      /*osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
      polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
      ss->setAttributeAndModes(polymode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);*/
      
      osg::StateSet* ss = mImpl->mMeshNode->getOrCreateStateSet();
      ss->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

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
       
       openvdb::BoolGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::BoolGrid>(gridPtr);

       openvdb::BoolGrid::ConstAccessor accessor = gridB->getConstAccessor();
       
       openvdb::tools::GridSampler<openvdb::BoolGrid::ConstAccessor, openvdb::tools::BoxSampler>
          fastSampler(accessor, gridB->transform());
       
       const osg::Matrixd& mat = mImpl->mVolumeTile->getLocator()->getTransform();
       osg::Vec3 offset = mat.getTrans();
       
       std::cout << "Cell Offset: " << offset[0] << " " << offset[1] << " " << offset[2] << std::endl;

       osg::Vec3 texelSize(cellSize[0] / float(width), cellSize[1] / float(height), cellSize[2] / float(slices));
       osg::Vec3 halfTexel = texelSize * 0.5;
              
       AABBIntersector<openvdb::BoolGrid> aabb(boost::dynamic_pointer_cast<openvdb::BoolGrid>(gridPtr));
       
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

                  openvdb::GridBase::Ptr gridPtr = voxelActor.CollideWithAABB(bb);

                  aabb.SetWorldBB(bbox);
                  aabb.Intersect();
                  openvdb::GridBase::Ptr grid2Ptr = aabb.GetHits();


                  if (grid2Ptr != NULL && !grid2Ptr->empty())
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

   /*openvdb::GridBase::Ptr VoxelCell::CreateLocalResolutionGrid(openvdb::GridBase::Ptr gridPtr, const osg::Vec3& cellSize)
   {
      openvdb::Mat4R m;
      m.setToScale(openvdb::Vec3R(cellSize[0], cellSize[1], cellSize[2]));
      openvdb::math::Transform::Ptr xform = openvdb::math::Transform::createLinearTransform(m);
      xform->voxelSize(openvdb::Vec3R(cellSize[0], cellSize[1], cellSize[2]));

      openvdb::BoolGrid::Ptr outGrid = openvdb::BoolGrid::create();
      outGrid->setTransform(xform);

      struct Local {
         static inline void op(
            const openvdb::BoolGrid::ValueOnCIter& iter,
            typename openvdb::BoolGrid::Accessor& accessor)
         {
            if (iter.isVoxelValue()) { // set a single voxel
               accessor.setValue(iter.getCoord(), openvdb::math::Abs(*iter) > FLT_EPSILON);
            }
            else { // fill an entire tile
               openvdb::CoordBBox bbox;
               iter.getBoundingBox(bbox);
               accessor.getTree()->fill(bbox, openvdb::math::Abs(*iter) > FLT_EPSILON);
            }
         }
      };

      openvdb::tools::transformValues(gridPtr->cbeginValueOn(), *outGrid, Local::op);

      return outGrid;
   }*/
   
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

       /*osg::Vec3 result;
       if (mImpl->mVolumeTile.valid())
       {
          osgVolume::Locator* locator = mImpl->mVolumeTile->getLocator();
          if (locator != NULL)
          {
             result = locator->getTransform().getTrans();
          }
       }

       return result;*/
   }


} /* namespace dtVoxel */
