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
#include <dtVoxel/voxelactor.h>
#include <prefix/unittestprefix.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/coordinates.h>
#include <dtCore/refptr.h>
#include <dtCore/project.h>
#include <dtVoxel/voxelactor.h>
#include <dtVoxel/voxelactorregistry.h>
#include <dtVoxel/aabbintersector.h>
#include "../dtGame/basegmtests.h"

namespace dtVoxel
{
   class VoxelActorTests : public dtGame::BaseGMTestFixture
   {
      typedef dtGame::BaseGMTestFixture BaseClass;
      CPPUNIT_TEST_SUITE(VoxelActorTests);

         CPPUNIT_TEST(testVolumeLibraryExtRegistration);
         CPPUNIT_TEST(testVoxelActor);
         CPPUNIT_TEST(testVoxelColliderAABB);

      CPPUNIT_TEST_SUITE_END();

   public:
      void GetRequiredLibraries(NameVector& names) override
      {
         static const std::string voxelLib("dtVoxel");
         names.push_back(voxelLib);
      }

      void testVolumeLibraryExtRegistration()
      {
         dtCore::Project::ResourceTree rt;
         dtCore::Project::GetInstance().GetResourcesOfType(dtCore::DataType::VOLUME, rt);
         CPPUNIT_ASSERT(rt.size() > 0);
         bool found = false;
         dtCore::Project::ResourceTree::iterator ti = rt.get_tree_iterator();
         dtCore::ResourceDescriptor rd("Volumes:delta3d_island.vdb");
         ti = ti.tree_ref().find(dtCore::ResourceTreeNode("delta3d_island.vdb", "Volumes", &rd, 0));
         found = ti != rt.end() && ti->getResource().GetResourceIdentifier() == "Volumes:delta3d_island.vdb";
         CPPUNIT_ASSERT(found);
      }

      void testVoxelActor()
      {
         try
         {
            dtCore::RefPtr<dtVoxel::VoxelActor> voxelActor;
            mGM->CreateActor(*VoxelActorRegistry::VOXEL_ACTOR_TYPE, voxelActor);
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
            voxelActor->SetDatabase(dtCore::ResourceDescriptor("Volumes:delta3d_island.vdb"));
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(1U));
         }
         catch(const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }
      void testVoxelColliderAABB()
      {
         try
         {
            dtCore::RefPtr<dtVoxel::VoxelActor> voxelActor;
            mGM->CreateActor(*VoxelActorRegistry::VOXEL_ACTOR_TYPE, voxelActor);
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
            voxelActor->SetDatabase(dtCore::ResourceDescriptor("Volumes:delta3d_island.vdb"));
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(1U));

            openvdb::BoolGrid::Ptr grid = boost::dynamic_pointer_cast<openvdb::BoolGrid>(voxelActor->GetGrid(0));
            CPPUNIT_ASSERT(grid);

            AABBIntersector<openvdb::BoolGrid> aabb(grid);
            openvdb::Index64 leafVoxelsStart = grid->tree().activeLeafVoxelCount();
            openvdb::CoordBBox indexBox(openvdb::Coord(10,10,0), openvdb::Coord(12,12,50));

            openvdb::BoolGrid::TreePtrType testTree(new openvdb::BoolGrid::TreeType);
            testTree->fill(indexBox, true, true);

            aabb.SetIndexBB(indexBox);
            aabb.Intersect();
            CPPUNIT_ASSERT_EQUAL(leafVoxelsStart, grid->tree().activeLeafVoxelCount());
            openvdb::BoolGrid::Ptr hitGrid1 = aabb.GetHits();
            CPPUNIT_ASSERT(hitGrid1->tree().activeLeafVoxelCount() < testTree->activeLeafVoxelCount());
            CPPUNIT_ASSERT_EQUAL(openvdb::Index64(22), hitGrid1->tree().activeLeafVoxelCount());


            osg::BoundingBox bb;
            bb.set(osg::Vec3(12.0f, 16.0f, 0.0f), osg::Vec3(12.1f, 16.1f, 33.0f));
            openvdb::GridBase::Ptr gridBase = voxelActor->CollideWithAABB(bb, 0);
            CPPUNIT_ASSERT(gridBase);
            openvdb::BoolGrid::Ptr hitGrid2 = boost::dynamic_pointer_cast<openvdb::BoolGrid>(gridBase);
            CPPUNIT_ASSERT(hitGrid2);
            CPPUNIT_ASSERT_EQUAL(openvdb::Index64(3), hitGrid2->tree().activeLeafVoxelCount());

         }
         catch(const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(VoxelActorTests);

} /* namespace dtVoxel */
