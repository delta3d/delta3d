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
#include <dtVoxel/marchingcubes.h>
#include <dtVoxel/voxelactor.h>

#include <dtUtil/mathdefines.h>

#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osgUtil/Simplifier>



namespace dtVoxel
{

   VoxelGridDebugDrawable::VoxelGridDebugDrawable()
      : mSceneRoot(new osg::Group())
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

   double VoxelGridDebugDrawable::SampleCoord(double x, double y, double z, openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler>& fastSampler)
   {
      double result = (fastSampler.wsSample(openvdb::Vec3R(x, y, z)));
      dtUtil::Clamp(result, -0.33, 0.36);

      result = dtUtil::MapRangeValue(result, -0.33, 0.36, 0.0, 1.0);
      return result;
   }

   void VoxelGridDebugDrawable::CreateDebugDrawable(dtVoxel::VoxelGrid& grid)
   {
      mSceneRoot->removeChildren(0, mSceneRoot->getNumChildren());

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
         //osg::Vec3 offset = grid.GetGridOffset();

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

   void VoxelGridDebugDrawable::CreateScreenSpaceMesh(VoxelActor& voxelActor, const osg::Vec3& offset, const osg::Vec3& resolution, const osg::Vec3& stepSize)
   {
      //clear any existing meshes
      mSceneRoot->removeChildren(0, mSceneRoot->getNumChildren());

      dtCore::RefPtr<osg::Geode> meshNode = new osg::Geode();

      dtCore::RefPtr<osg::Geometry> geom = new osg::Geometry();
      dtCore::RefPtr<osg::Vec3Array> vertArray = new osg::Vec3Array();
      dtCore::RefPtr<osg::Vec3Array> normalArray = new osg::Vec3Array();
      dtCore::RefPtr<osg::Vec3Array> colorArray = new osg::Vec3Array();
      dtCore::RefPtr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(GL_TRIANGLES);      

      /*openvdb::BoolGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::BoolGrid>(voxelActor.GetGrid(0));

      openvdb::BoolGrid::ConstAccessor accessor = gridB->getConstAccessor();

      openvdb::tools::GridSampler<openvdb::BoolGrid::ConstAccessor, openvdb::tools::PointSampler>
      fastSampler(accessor, gridB->transform());*/


      openvdb::FloatGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::FloatGrid>(voxelActor.GetGrid(0));
      //openvdb::FloatGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::FloatGrid>(localGrid);

      openvdb::FloatGrid::ConstAccessor accessor = gridB->getConstAccessor();

      openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler>
         fastSampler(accessor, gridB->transform());

      //reusing this improves performance by quite a bit 
      osg::Vec3 vertlist[12];

      float isolevel = 1.0f;

      for (float i = 0; i < resolution[0]; i += stepSize[0])
      {
         for (float j = 0; j < resolution[1]; j += stepSize[1])
         {
            for (float k = 0; k < resolution[2]; k += stepSize[2])
            {
               double worldX = offset[0] + i;
               double worldY = offset[1] + j;
               double worldZ = offset[2] + k;

               osg::Vec3 from(worldX, worldY, worldZ);

               GRIDCELL grid;
               TRIANGLE triangles[5];

               grid.p[0] = from;
               grid.val[0] = SampleCoord(grid.p[0].x(), grid.p[0].y(), grid.p[0].z(), fastSampler);

               grid.p[1].set(from[0] + stepSize[0], from[1], from[2]);
               grid.val[1] = SampleCoord(grid.p[1].x(), grid.p[1].y(), grid.p[1].z(), fastSampler);

               grid.p[2].set(from[0] + stepSize[0], from[1] + stepSize[1], from[2]);
               grid.val[2] = SampleCoord(grid.p[2].x(), grid.p[2].y(), grid.p[2].z(), fastSampler);

               grid.p[3].set(from[0], from[1] + stepSize[1], from[2]);
               grid.val[3] = SampleCoord(grid.p[3].x(), grid.p[3].y(), grid.p[3].z(), fastSampler);

               grid.p[4].set(from[0], from[1], from[2] + stepSize[2]);
               grid.val[4] = SampleCoord(grid.p[4].x(), grid.p[4].y(), grid.p[4].z(), fastSampler);

               grid.p[5].set(from[0] + stepSize[0], from[1], from[2] + stepSize[2]);
               grid.val[5] = SampleCoord(grid.p[5].x(), grid.p[5].y(), grid.p[5].z(), fastSampler);

               grid.p[6].set(from[0] + stepSize[0], from[1] + stepSize[1], from[2] + stepSize[2]);
               grid.val[6] = SampleCoord(grid.p[6].x(), grid.p[6].y(), grid.p[6].z(), fastSampler);

               grid.p[7].set(from[0], from[1] + stepSize[1], from[2] + stepSize[2]);
               grid.val[7] = SampleCoord(grid.p[7].x(), grid.p[7].y(), grid.p[7].z(), fastSampler);

               bool allSamplesZero = true;
               bool enablePrintOuts = true;
               for (int s = 0; s < 8 && enablePrintOuts; ++s)
               {
                  if (grid.val[s] < 0.0)// && grid.val[s] != 1)
                  {
                     allSamplesZero = false;
                     break;
                  }
               }

               if (!allSamplesZero)
               {
                  std::cout << std::endl << "Texel i" << i << ", j " << j << ", k " << k << std::endl;
                  std::cout << "Pos (" << worldX << ", " << worldY << ", " << worldZ << ")" << std::endl;


                  for (int p = 0; p < 8; ++p)
                  {
                     std::cout << std::endl;

                     std::cout << "Sample Value: " << grid.val[p];
                  }

                  std::cout << std::endl;
               }

               int numTriangles = PolygonizeCube(grid, isolevel, triangles, &vertlist[0]);


               if (!allSamplesZero)
               {
                  std::cout << "NumTriangles " << numTriangles << std::endl << std::endl;
               }

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

      meshNode->addDrawable(geom);

      dtCore::RefPtr<osgUtil::Simplifier> simplifier = new osgUtil::Simplifier();
      simplifier->setMaximumLength(100.0f);
      simplifier->setDoTriStrip(true);
      meshNode->accept(*simplifier);

      //std::cout << "Num Triangles After Simplifier " << geom->getVertexArray()->getNumElements() << std::endl;

      meshNode->accept(*simplifier);

      osg::StateSet* ss = meshNode->getOrCreateStateSet();

      osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
      polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
      ss->setAttributeAndModes(polymode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

      ss->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
      
      mSceneRoot->addChild(meshNode);
   }


} /* namespace dtVoxel */
