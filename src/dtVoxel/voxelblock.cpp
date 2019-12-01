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
#include <dtUtil/fileutils.h>
#include <dtUtil/stringutils.h>
#include <dtCore/project.h>

#include <osgUtil/Simplifier>
#include <osgUtil/Optimizer>

#include <osgDB/WriteFile>
#include <osg/PolygonMode>
#include <osg/PagedLOD>

#include <tbb/parallel_for.h>
#include <iostream>

namespace dtVoxel
{
   class FindVoxelCellVisitor : public osg::NodeVisitor
   {
   public:

      FindVoxelCellVisitor(const std::string& name) 
         : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
         , mLOD(nullptr)
         , mFoundNode(nullptr)
         , mName(name)
      {

      }

      virtual void apply(osg::Group& group)
      {
         if (group.getName() == mName)
         {
            mFoundNode = &group;
         }
         else
         {
            traverse(group);
         }
      }

      virtual void apply(osg::PagedLOD& lod)
      {
         mLOD = &lod;
         traverse(lod);
      }

      
      osg::PagedLOD* mLOD;
      osg::Group* mFoundNode;
      std::string mName;
   };



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

      mGridDimensions.set(
         int(std::floor(mWSDimensions[0] / mWSCellDimensions[0])),
         int(std::floor(mWSDimensions[1] / mWSCellDimensions[1])),
         int(std::floor(mWSDimensions[2] / mWSCellDimensions[2])));

      mNumCells = mGridDimensions[0] * mGridDimensions[1] * mGridDimensions[2];
      
      mCells = new VoxelCell[mNumCells];
   }
   
   void VoxelBlock::DeAllocate()
   {
      //std::cout << "DeAllocating Voxel Block" << std::endl;

      if (mVolume.valid())
      {
         mVolume->removeChildren(0, mVolume->getNumChildren());

         if (mCells != nullptr && IsAllocated())
         {
            for (int i = 0; i < mNumCells; ++i)
            {
               mCells[i].DeAllocate();
            }

            delete[] mCells;
            mCells = NULL;
         }
      }

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

   void VoxelBlock::CollectDirtyCells(VoxelActor& voxelActor, const osg::BoundingBox& bb, const osg::Vec3i& textureResolution, std::list<VoxelCellUpdateInfo>& dirtyCells)
   {
      osg::BoundingBox bounds(mOffset, mOffset + mWSDimensions);

      //reduce bounding volume to only include our region
      osg::BoundingBox dirtyBounds = bb.intersect(bounds);

      osg::Vec3i startIndex = GetIndexFromPos(dirtyBounds._min);
      osg::Vec3i endIndex = GetIndexFromPos(dirtyBounds._max);

      //std::cout << std::endl << "start index " << startIndex.x() << ", " << startIndex.y() << ", " << startIndex.z() << std::endl;
      //std::cout << "end index " << endIndex.x() << ", " << endIndex.y() << ", " << endIndex.z() << std::endl;


      for (int z = startIndex.z(); z <= endIndex.z(); z++)
      {
         for (int y = startIndex.y(); y <= endIndex.y(); y++)
         {
            for (int x = startIndex.x(); x <= endIndex.x(); x++)
            {
               if (z >= 0 && z < mGridDimensions.z())
               {
                  if (y >= 0 && y < mGridDimensions.y())
                  {
                     if (x >= 0 && x < mGridDimensions.x())
                     {
                        VoxelCell* vc = GetCellFromIndex(x, y, z);
                        
                        //std::cout << "Found Cell" << std::endl;

                        if (vc != nullptr /*&& !vc->IsDirty()*/)
                        {
                           //std::cout << "Cell is not dirty " << std::endl;

                           FindVoxelCellVisitor fv(GetCellName(x, y, z));
                           GetOSGNode()->accept(fv);

                           if (fv.mFoundNode != nullptr && fv.mLOD != nullptr)
                           {
                                                            
                              //std::cout << "Found Node" << std::endl;

                              if (!vc->IsDirty())
                              {
                                 fv.mLOD->setNumChildrenThatCannotBeExpired(fv.mLOD->getNumChildren());

                                 VoxelCellUpdateInfo updateInfo;
                                 updateInfo.mBlock = this;
                                 updateInfo.mCell = vc;
                                 updateInfo.mNodeToUpdate = fv.mFoundNode;
                                 updateInfo.mCellIndex.set(x, y, z);
                                 updateInfo.mStarted = false;
                                 updateInfo.mLODNode = fv.mLOD;

                                 vc->SetDirty(true);

                                 //std::cout << "Adding nodes to dirty cells" << std::endl;

                                 dirtyCells.push_back(updateInfo);
                              }

                              osg::Vec3 pos(x * mWSCellDimensions[0], y * mWSCellDimensions[1], z * mWSCellDimensions[2]);

                              osg::Matrix transform;
                              transform.makeScale(1.0f, 1.0f, 1.0f); //compute offset
                              transform.setTrans(pos + mOffset);

                              //start generating the mesh on a background thread
                              vc->CreateMeshWithTask(voxelActor, transform, mWSCellDimensions, textureResolution, dirtyBounds);
                           }
                           /*else
                           {
                              if (fv.mFoundNode == nullptr)
                              {
                                 std::cout << "Node is null " << GetCellName(x, y, z) << std::endl;
                              }
                              else if (fv.mLOD != nullptr)
                                 LOG_ERROR("LOD NODE IS NULL")
                           }*/
                        }
                     }
                  }
               }
            }
         }
      }
   }


   void VoxelBlock::RegenerateCell(VoxelActor& voxelActor, VoxelCell* cell, osg::Group* nodeToUpdate, const osg::Vec3i& cellIndex, const osg::Vec3i& textureResolution, float viewDistance)
   {
      //std::cout << "Voxel Block, regenerate cell" << std::endl;
      
      osg::Vec3 pos = GetPosFromIndex(cellIndex);
      osg::Vec3 offsetTo = pos + mWSCellDimensions;

      osg::Matrix transform;
      transform.makeScale(1.0f, 1.0f, 1.0f); //compute offset
      transform.setTrans(pos);
      
      dtCore::RefPtr<osg::Group> nodeToRemove = nodeToUpdate;
      dtCore::RefPtr<osg::Group> parent = nodeToUpdate->getParent(0);      

      parent->removeChild(nodeToRemove);

      bool wasAllocated = false;
      if (cell->IsAllocated())
      {
         wasAllocated = true;
         cell->DeAllocate();
      }

      cell->SetDirty(false);

      //todo using local grid should be faster
      osg::BoundingBox bb(pos, offsetTo);
      bool hasData = voxelActor.HasDataInAABB(bb);

      if (hasData)
      {
         //std::cout << "Creating new mesh." << std::endl;
         
         cell->TakeGeometry();

         cell->GetOSGNode()->setName(nodeToRemove->getName());
         parent->addChild(cell->GetOSGNode());

         if (!wasAllocated)
         {
            osg::Group* pt = mVolume->getChild(0)->asGroup();
            osg::PagedLOD* lod = dynamic_cast<osg::PagedLOD*>(pt);

            //std::cout << "looking for paged lod" << std::endl;

            //find the paged lod child                                 
            while (pt != nullptr && lod == nullptr && pt->getNumChildren() > 0)
            {
               //std::cout << "executing loop" << std::endl;

               pt = pt->getChild(0)->asGroup();
               lod = dynamic_cast<osg::PagedLOD*>(pt);
            }

            //std::cout << "found lod node" << std::endl;


            if (lod != nullptr)// && lod->getNumChildren() > 1)
            {
               lod->removeChildren(1, lod->getNumChildren());
               lod->setNumChildrenThatCannotBeExpired(1);
               lod->setRange(0, 0.0f, viewDistance); 
               //std::cout << "Marking child 0 to not expire, deleting rest of children" << std::endl;
            }
         }

      }
      else
      {
         cell->SetAllocated(false);
      }
       
   }



   void VoxelBlock::RegenerateAABB(VoxelActor& voxelActor, const osg::BoundingBox& bb, const osg::Vec3i& textureResolution)
   {
      osg::BoundingBox bounds(mOffset, mOffset + mWSDimensions);

      //reduce bounding volume to only include our region
      osg::BoundingBox dirtyBounds = bb.intersect(bounds);

      osg::Vec3i startIndex = GetIndexFromPos(dirtyBounds._min);
      osg::Vec3i endIndex = GetIndexFromPos(dirtyBounds._max);

      //std::cout << std::endl << "start index " << startIndex.x() << ", " << startIndex.y() << ", " << startIndex.z() << std::endl;
      //std::cout << std::endl << "end index " << endIndex.x() << ", " << endIndex.y() << ", " << endIndex.z() << std::endl;


      for (int z = startIndex.z(); z <= endIndex.z(); z++)
      {
         for (int y = startIndex.y(); y <= endIndex.y(); y++)
         {
            for (int x = startIndex.x(); x <= endIndex.x(); x++)
            {
               if (z >= 0 && z < mGridDimensions.z())
               {
                  if (y >= 0 && y < mGridDimensions.y())
                  {
                     if (x >= 0 && x < mGridDimensions.x())
                     {
                        VoxelCell* vc = GetCellFromIndex(x, y, z);

                        if (vc != nullptr)
                        {
                           //std::cout << "Regenerating Cell " << x << ", " << y << ", " << z << std::endl;

                           osg::Vec3 pos(x * mWSCellDimensions[0], y * mWSCellDimensions[1], z * mWSCellDimensions[2]);
                           osg::Vec3 offsetFrom = pos + mOffset;
                           osg::Vec3 offsetTo = offsetFrom + mWSCellDimensions;

                           osg::Matrix transform;
                           transform.makeScale(1.0f, 1.0f, 1.0f); //compute offset
                           transform.setTrans(offsetFrom);

                           FindVoxelCellVisitor fv(GetCellName(x, y, z));
                           GetOSGNode()->accept(fv);

                           if (fv.mFoundNode != nullptr)
                           {
                              dtCore::RefPtr<osg::Group> nodeToRemove = fv.mFoundNode;
                              dtCore::RefPtr<osg::Group> parent = nodeToRemove->getParent(0);

                              //std::cout << "Removing child geode." << std::endl;

                              parent->removeChild(nodeToRemove);
                              
                              bool wasAllocated = false;
                              if (vc->IsAllocated())
                              {                   
                                 wasAllocated = true;
                                 vc->DeAllocate();
                              }

                              //todo using local grid should be faster
                              osg::BoundingBox bb(offsetFrom, offsetTo);
                              bool hasData = voxelActor.HasDataInAABB(bb);

                              if (hasData)
                              {
                                 //std::cout << "Creating new mesh." << std::endl;

                                 vc->CreateMesh(voxelActor, transform, mWSCellDimensions, textureResolution);
                                 vc->GetOSGNode()->setName(fv.mName);
                                 parent->addChild(vc->GetOSGNode());
                                 
                                 if (!wasAllocated && fv.mLOD != NULL)
                                 {
                                    fv.mLOD->removeChildren(1, fv.mLOD->getNumChildren());
                                    fv.mLOD->setNumChildrenThatCannotBeExpired(1);
                                    fv.mLOD->setRange(0, 0.0f, voxelActor.GetViewDistance());
                                 }
                                 else if (fv.mLOD != NULL)
                                 {
                                    LOG_ERROR("LOD NODE IS NULL");
                                 }

                              }
                              else
                              {
                                 //std::cout << "new voxel cell is empty." << std::endl;

                              }
                           }
                           else
                           {
                              //LOG_ERROR("Error finding LOD node, cannot regenerate mesh.");
                           }
                           
                        }
                        else
                        {
                           LOG_ERROR("Invalid data voxel cell is NULL");
                        }
                     }
                  }
               }
            }
         }
      }

      SetDirty(false);
   }

   void VoxelBlock::AllocateCells(VoxelActor& voxelActor, osg::Group& parentNode, const osg::Vec3& gridDimensions, const osg::Vec3i& textureResolution)
   {
      //std::cout << "Allocating Individual Voxel Block Cells" << std::endl;
           
      for (unsigned int z = 0; z < gridDimensions[2]; ++z)
      {
         for (unsigned int y = 0; y < gridDimensions[1]; ++y)
         {
            for (unsigned int x = 0; x < gridDimensions[0]; ++x)
            {
               osg::Vec3 pos(x * mWSCellDimensions[0], y * mWSCellDimensions[1], z * mWSCellDimensions[2]);
               osg::Vec3 offsetFrom = pos + mOffset;
               osg::Vec3 offsetTo = offsetFrom + mWSCellDimensions;

               osg::Matrix transform;
               transform.makeScale(1.0f, 1.0f, 1.0f); //compute offset
               transform.setTrans(offsetFrom);

               //todo using local grid should be faster
               osg::BoundingBox bb(offsetFrom, offsetTo);
               bool hasData = voxelActor.HasDataInAABB(bb);

               if (hasData)
               {
                  //keeping a pointer and incrementing it should be much faster for a large contiguous dataset
                  int cellIndex = (z * gridDimensions[1] * gridDimensions[0]) + (y * gridDimensions[0]) + x;
                  
                  VoxelCell& vc = mCells[cellIndex];
                  vc.CreateMesh(voxelActor, transform, mWSCellDimensions, textureResolution);

                  vc.GetOSGNode()->setName(GetCellName(x, y, z));
                  parentNode.addChild(vc.GetOSGNode());
               }

            }
         }
      }

      //spatialize
      osgUtil::Optimizer opt;
      opt.optimize(&parentNode, osgUtil::Optimizer::SPATIALIZE_GROUPS);

      mIsAllocated = true;
   }

   void VoxelBlock::AllocateLODMesh(VoxelActor& voxelActor, const osg::Vec3i& resolution0, float dist0, const osg::Vec3i& resolution1, float dist1, const osg::Vec3i& resolution2, float dist2, const osg::Vec3i& resolution3, float viewDistance)
   {
      dtCore::RefPtr<osg::LOD> lodNode = new osg::LOD();

      dtCore::RefPtr<osg::Group> node0 = new osg::Group;
      dtCore::RefPtr<osg::Group> node1 = new osg::Group;
      dtCore::RefPtr<osg::Group> node2 = new osg::Group;
      //dtCore::RefPtr<osg::Group> node3 = new osg::Group;

          
      AllocateCells(voxelActor, *node0, mGridDimensions, resolution0);
      AllocateCombinedMesh(voxelActor, *node1, mGridDimensions, resolution1);
      AllocateCombinedMesh(voxelActor, *node2, mGridDimensions, resolution2);
      //AllocateCombinedMesh(voxelActor, *node3, mGridDimensions, resolution3);
      

      lodNode->addChild(node0, 0.0f, dist0);
      lodNode->addChild(node1, dist0, dist1);
      lodNode->addChild(node2, dist1, dist2);
      //lodNode->addChild(node3, dist2, viewDistance);

      mVolume->addChild(lodNode);
      
      mIsAllocated = true;
   }

   void VoxelBlock::WritePagedLOD(VoxelActor& voxelActor, int index, const std::string& filePath, const std::string& databasePath, const osg::Vec3i& resolution0, float dist0, const osg::Vec3i& resolution1, float dist1, const osg::Vec3i& resolution2, float dist2, const osg::Vec3i& resolution3, float viewDistance)
   {
      dtCore::RefPtr<osg::PagedLOD> lodNode = new osg::PagedLOD();
      lodNode->setDatabasePath(databasePath);
      lodNode->setRadius(0.5 * mWSDimensions.length());
      lodNode->setCenter(mOffset + (mWSDimensions * 0.5f));

      unsigned lods = voxelActor.GetNumLODs();
      
      if (lods > 3)
      {
         LOG_WARNING("Num LODs on the VoxelActor cannot be > 3, valid settings are 0, 1, 2, or 3");
         lods = 3;
      }

      dtCore::RefPtr<osg::Group> node0 = new osg::Group;
      
      dtCore::RefPtr<osg::Group> node1;
      dtCore::RefPtr<osg::Group> node2;
      dtCore::RefPtr<osg::Group> node3;


      AllocateCells(voxelActor, *node0, mGridDimensions, resolution0);
      std::string fileName = SaveCachedModel(filePath, *node0, index, 0);
      if (!fileName.empty())
      {
         lodNode->setFileName(0, fileName);
         lodNode->setRange(0, 0.0f, (lods == 0) ? viewDistance : dist0);
      }
      else
      {
         LOG_ERROR("Error writing paged lod node 0.");
      }

      if (lods > 0)
      {
         node1 = new osg::Group;

         AllocateCombinedMesh(voxelActor, *node1, mGridDimensions, resolution1);
         fileName = SaveCachedModel(filePath, *node1, index, 1);
         if (!fileName.empty())
         {
            lodNode->setFileName(1, fileName);
            lodNode->setRange(1, dist0, (lods == 1) ? viewDistance : dist1);
         }
         else
         {
            LOG_ERROR("Error writing paged lod node 1.");
         }

         if (lods > 1)
         {
            node2 = new osg::Group;

            AllocateCombinedMesh(voxelActor, *node2, mGridDimensions, resolution2);
            fileName = SaveCachedModel(filePath, *node2, index, 2);
            if (!fileName.empty())
            {
               lodNode->setFileName(2, fileName);
               lodNode->setRange(2, dist1, (lods == 2) ? viewDistance : dist2);
            }
            else
            {
               LOG_ERROR("Error writing paged lod node 2.");
            }
            if (lods > 2)
            {
               node3 = new osg::Group;

               AllocateCombinedMesh(voxelActor, *node3, mGridDimensions, resolution3);
               fileName = SaveCachedModel(filePath, *node3, index, 3);
               if (!fileName.empty())
               {
                  lodNode->setFileName(3, fileName);
                  lodNode->setRange(3, dist2, viewDistance);
               }
               else
               {
                  LOG_ERROR("Error writing paged lod node 3.");
               }
            }
         }
      }

      mVolume->addChild(lodNode);

      SaveCachedModel(filePath, index);

      mIsAllocated = true;
   }


   void VoxelBlock::AllocateCombinedMesh(VoxelActor& voxelActor, osg::Group& parentNode, const osg::Vec3& gridDimensions, const osg::Vec3i& textureResolution)
   {
      //std::cout << "Allocating Combined Voxel Block" << std::endl;

      dtCore::RefPtr<osg::Geometry> geom = new osg::Geometry();
      dtCore::RefPtr<osg::Vec3Array> vertArray = new osg::Vec3Array();            
      dtCore::RefPtr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(GL_TRIANGLES);

      //VoxelCell* cellToInit = &mCells[0];
      for (unsigned int z = 0; z < gridDimensions[2]; ++z)
      {
         for (unsigned int y = 0; y < gridDimensions[1]; ++y)
         {
            for (unsigned int x = 0; x < gridDimensions[0]; ++x)
            {
               osg::Vec3 pos(x * mWSCellDimensions[0], y * mWSCellDimensions[1], z * mWSCellDimensions[2]);
               osg::Vec3 offsetFrom = pos + mOffset;
               osg::Vec3 offsetTo = offsetFrom + mWSCellDimensions;

               osg::Matrix transform;
               transform.makeScale(1.0f, 1.0f, 1.0f); //compute offset
               transform.setTrans(offsetFrom);

               //todo using local grid should be faster
               osg::BoundingBox bb(offsetFrom, offsetTo);
               bool hasData = voxelActor.HasDataInAABB(bb);
               
               if (hasData)
               {
                  //keeping a pointer and incrementing it should be much faster for a large contiguous dataset
                  int cellIndex = (z * gridDimensions[1] * gridDimensions[0]) + (y * gridDimensions[0]) + x;
                  mCells[cellIndex].AddGeometry(voxelActor, transform, mWSCellDimensions, textureResolution, vertArray, drawElements);
               }

               //++cellToInit;
            }
         }
      }


      geom->setVertexArray(vertArray);
      geom->addPrimitiveSet(drawElements);

      osg::Geode* geode = new osg::Geode();
      geode->addDrawable(geom);
      parentNode.addChild(geode);      

      if (voxelActor.GetSimplify())
      {
         dtCore::RefPtr<osgUtil::Simplifier> simplifier = new osgUtil::Simplifier();
         simplifier->setSampleRatio(voxelActor.GetSampleRatio());
         simplifier->setDoTriStrip(false);
         parentNode.accept(*simplifier);
      }

      osgUtil::Optimizer opt;
      opt.optimize(&parentNode, osgUtil::Optimizer::MAKE_FAST_GEOMETRY);     
      
      mIsAllocated = true;
   }



   VoxelCell* VoxelBlock::GetCellFromIndex(int x, int y, int z)
   {
      VoxelCell* cell = NULL;

      int numCells = mGridDimensions[0] * mGridDimensions[1] * mGridDimensions[2];
      int cellIndex = (z * mGridDimensions[1] * mGridDimensions[0]) + (y * mGridDimensions[0]) + x;

      if (cellIndex >= 0 && cellIndex < numCells)
      {
         cell = &mCells[cellIndex];
      }
      else
      {
         std::cout << "Cell index out of bounds " << x << ", " << y << ", " << z << std::endl;
      }

      return cell;
   }

   osg::Vec3i VoxelBlock::GetIndexFromPos(const osg::Vec3& pos)
   {
      osg::Vec3 localPos = pos - mOffset;

      int indexX = int(std::floor(localPos[0] / mWSCellDimensions[0]));
      int indexY = int(std::floor(localPos[1] / mWSCellDimensions[1]));
      int indexZ = int(std::floor(localPos[2] / mWSCellDimensions[2]));

      return osg::Vec3i(indexX, indexY, indexZ);
   }

   osg::Vec3 VoxelBlock::GetPosFromIndex(const osg::Vec3i& index)
   {
      osg::Vec3 pos(index.x() * mWSCellDimensions[0], index.y() * mWSCellDimensions[1], index.z() * mWSCellDimensions[2]);
      pos += mOffset;

      return pos;
   }


   VoxelCell* VoxelBlock::GetCellFromPos(const osg::Vec3& pos)
   {
      VoxelCell* cell = NULL;

      osg::Vec3 localPos = pos - mOffset;

      int indexX = int(std::floor(localPos[0] / mWSCellDimensions[0]));
      int indexY = int(std::floor(localPos[1] / mWSCellDimensions[1]));
      int indexZ = int(std::floor(localPos[2] / mWSCellDimensions[2]));

      int numCells = mGridDimensions[0] * mGridDimensions[1] * mGridDimensions[2];
      int cellIndex = (indexZ * mGridDimensions[1] * mGridDimensions[0]) + (indexY * mGridDimensions[0]) + indexX;

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

   bool VoxelBlock::HasCachedModel(const std::string& folderName, int index)
   {
      bool result = false;
      
      std::string indexString;
      std::stringstream fileName;

      dtUtil::MakeIndexString(index, indexString, 8);

      fileName << folderName << "VoxelGrid_cache" << indexString << "_paged.osgt";

      if (dtUtil::FileUtils::GetInstance().FileExists(fileName.str()))
      {
         result = true;
      }

      return result;
   }

   bool VoxelBlock::LoadCachedModel(const std::string& folderName, int index)
   {
      bool result = false;
      
      std::string indexString;
      std::stringstream fileName;

      dtUtil::MakeIndexString(index, indexString, 8);

      fileName << folderName << "VoxelGrid_cache" << indexString << "_paged.osgt";

      if (dtUtil::FileUtils::GetInstance().FileExists(fileName.str()))
      {
         LOGN_DEBUG("voxelblock.cpp", "Reading block num " + dtUtil::ToString(index) + " from model cache " + fileName.str());

         osg::Node* n = dtUtil::FileUtils::GetInstance().ReadNode(fileName.str());
         if (n != nullptr)
         {
            /*osg::PagedLOD* lod = dynamic_cast<osg::PagedLOD*>(n->asGroup()->getChild(0));
            if (lod)
            {
               std::cout << "Changing lod options" << std::endl;
            }*/

            mVolume->addChild(n);
               
            mIsAllocated = true;
            result = true;
         }
         else
         {
            LOG_ERROR("Error reading cached node.");
         }
         
      }

      return result;
   }



   bool VoxelBlock::SaveCachedModel(const std::string& folderName, int index)
   {
      bool result = false;

      std::string indexString;
      std::stringstream fileName;

      dtUtil::MakeIndexString(index, indexString, 8);

      fileName << folderName << "VoxelGrid_cache" << indexString << "_paged.osgt";

      if (dtUtil::FileUtils::GetInstance().DirExists(folderName))
      {
         result = osgDB::writeNodeFile(*mVolume, fileName.str());

         LOGN_DEBUG("voxelblock.cpp", "Writing PagedLOD for block num " + dtUtil::ToString(index) + " to model cache " + fileName.str());
      }

      return result;
   }

   std::string VoxelBlock::SaveCachedModel(const std::string& folderName, osg::Node& n, int index, int lod)
   {
      std::string result;

      std::string indexString;
      std::string lodString;
      std::stringstream fileName;

      dtUtil::MakeIndexString(index, indexString, 8);
      dtUtil::MakeIndexString(lod, lodString, 3);

      fileName << "VoxelGrid_cache" << indexString << "_lod" << lodString << ".osgb";

      if (dtUtil::FileUtils::GetInstance().DirExists(folderName))
      {
         bool success = osgDB::writeNodeFile(n, folderName + fileName.str());

         if (success)
         {
            //result will be empty string if this fails
            result = fileName.str();
            LOGN_DEBUG("voxelblock.cpp", "Writing lod " + dtUtil::ToString(lod) + " for block num " + dtUtil::ToString(index) + " to model   cache " + folderName + fileName.str());
         }
         else
         {
            LOG_ERROR("Error writing block to disk.");
         }

      }

      return result;
   }

   std::string VoxelBlock::GetCellName(int x, int y, int z)
   {
      std::stringstream strs;
      strs << x << y << z << "_VoxelCell";

      return strs.str();
   }


} /* namespace dtVoxel */
