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

#include <dtVoxel/voxelmessagetype.h>
#include <dtVoxel/volumeupdatemessage.h>

#include <dtCore/system.h>

namespace dtVoxel
{
   class VoxelActorTests : public dtGame::BaseGMTestFixture
   {
      typedef dtGame::BaseGMTestFixture BaseClass;
      CPPUNIT_TEST_SUITE(VoxelActorTests);

         CPPUNIT_TEST(testVolumeLibraryExtRegistration);
         CPPUNIT_TEST(testVoxelActor);
         CPPUNIT_TEST(testVoxelActorRemoteUpdate);
         CPPUNIT_TEST(testVolumeUpdateMessageToFromStream);
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
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
            voxelActor->CompleteLoad();
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(1U));
            openvdb::GridBase::Ptr grid = voxelActor->GetGrid(0);
            CPPUNIT_ASSERT(grid);
            voxelActor->SetDatabase(dtCore::ResourceDescriptor::NULL_RESOURCE);
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
            voxelActor->SetDatabase(dtCore::ResourceDescriptor("Volumes:delta3d_island.vdb"));
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
            voxelActor->CompleteLoad();
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(1U));
            openvdb::GridBase::Ptr grid2 = voxelActor->GetGrid(0);
            CPPUNIT_ASSERT(grid2 && grid2 != grid);
         }
         catch(const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }

      void testVoxelActorRemoteUpdate()
      {
          try
          {
             dtCore::RefPtr<dtVoxel::VoxelActor> voxelActor;
             mGM->CreateActor(*VoxelActorRegistry::VOXEL_ACTOR_TYPE, voxelActor);
             CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
             voxelActor->SetDatabase(dtCore::ResourceDescriptor("Volumes:delta3d_island.vdb"));
             voxelActor->CompleteLoad();
             CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(1U));
             mGM->AddActor(*voxelActor, false, false);
             openvdb::BoolGrid::Ptr grid = boost::dynamic_pointer_cast<openvdb::BoolGrid>(voxelActor->GetGrid(0));
             CPPUNIT_ASSERT(grid);
             auto accessor = grid->getAccessor();

             dtCore::RefPtr<VolumeUpdateMessage> msg;
             mGM->GetMessageFactory().CreateMessage(VoxelMessageType::INFO_VOLUME_CHANGED, msg);
             CPPUNIT_ASSERT(msg.valid());
             dtCore::RefPtr<dtGame::MachineInfo> testMI = new dtGame::MachineInfo("blah");
             msg->SetSource(*testMI);
             msg->AddChangedValue<bool>(osg::Vec3(1.0f, 3.0f, 92.0f), true);
             msg->AddChangedValue<bool>(osg::Vec3(9.0f, 4.0f, 93.0f), false);
             msg->AddChangedValue<bool>(osg::Vec3(-71.0f, -8.0f, -96.0f), true);

             mGM->SendMessage(*msg);
             dtCore::System::GetInstance().Step(0.016f);

             CPPUNIT_ASSERT_MESSAGE("The actor only registers for messages about itself.", !accessor.isValueOn(openvdb::Coord(1,3,92)));
             msg->SetAboutActorId(voxelActor->GetId());

             voxelActor->SetLocalActorUpdatePolicy(dtGame::GameActorProxy::LocalActorUpdatePolicy::IGNORE_ALL);

             mGM->SendMessage(*msg);
             dtCore::System::GetInstance().Step(0.016f);

             CPPUNIT_ASSERT_MESSAGE("The actor doesn't want updates.", !accessor.isValueOn(openvdb::Coord(1,3,92)));
             voxelActor->SetLocalActorUpdatePolicy(dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_ALL);

             mGM->SendMessage(*msg);
             dtCore::System::GetInstance().Step(0.016f);

             CPPUNIT_ASSERT(accessor.isValueOn(openvdb::Coord(1,3,92)));
             CPPUNIT_ASSERT(accessor.getValue(openvdb::Coord(1,3,92)));

             CPPUNIT_ASSERT(accessor.isValueOn(openvdb::Coord(9,4,93)));
             CPPUNIT_ASSERT(!accessor.getValue(openvdb::Coord(9,4,93)));

             CPPUNIT_ASSERT(accessor.isValueOn(openvdb::Coord(-71,-8,-96)));
             CPPUNIT_ASSERT(accessor.getValue(openvdb::Coord(-71,-8,-96)));

             msg = NULL;

             mGM->GetMessageFactory().CreateMessage(VoxelMessageType::INFO_VOLUME_CHANGED, msg);
             CPPUNIT_ASSERT(msg.valid());
             msg->SetSource(*testMI);
             msg->SetAboutActorId(voxelActor->GetId());
             msg->AddDeactivatedIndex(osg::Vec3(1.0f, 3.0f, 92.0f));
             msg->AddDeactivatedIndex(osg::Vec3(9.0f, 4.0f, 93.0f));
             msg->AddDeactivatedIndex(osg::Vec3(-71.0f, -8.0f, -96.0f));

             mGM->SendMessage(*msg);
             dtCore::System::GetInstance().Step(0.016f);

             CPPUNIT_ASSERT(!accessor.isValueOn(openvdb::Coord(1,3,92)));
             CPPUNIT_ASSERT_EQUAL(accessor.getValue(openvdb::Coord(1,3,92)), grid->background());

             CPPUNIT_ASSERT(!accessor.isValueOn(openvdb::Coord(9,4,93)));
             CPPUNIT_ASSERT_EQUAL(accessor.isValueOn(openvdb::Coord(9,4,93)), grid->background());

             CPPUNIT_ASSERT(!accessor.isValueOn(openvdb::Coord(-71,-8,-96)));
             CPPUNIT_ASSERT_EQUAL(accessor.isValueOn(openvdb::Coord(-71,-8,-96)), grid->background());
          }
          catch(const dtUtil::Exception& ex)
          {
             CPPUNIT_FAIL(ex.ToString());
          }
       }

      void testVolumeUpdateMessageToFromStream()
      {
          try
          {
             dtCore::RefPtr<VolumeUpdateMessage> msg, msgResult;
             mGM->GetMessageFactory().CreateMessage(VoxelMessageType::INFO_VOLUME_CHANGED, msg);
             mGM->GetMessageFactory().CreateMessage(VoxelMessageType::INFO_VOLUME_CHANGED, msgResult);
             CPPUNIT_ASSERT(msg.valid());
             CPPUNIT_ASSERT(msgResult.valid());
             dtCore::RefPtr<dtGame::MachineInfo> testMI = new dtGame::MachineInfo("blah");
             msg->SetSource(*testMI);
             msgResult->SetSource(*testMI);
             msg->AddChangedValue<bool>(osg::Vec3(1.0f, 3.0f, 92.0f), true);
             msg->AddDeactivatedIndex(osg::Vec3(9.0f, 5.0f, 94.0f));
             msg->AddChangedValue<bool>(osg::Vec3(9.0f, 4.0f, 93.0f), false);
             msg->AddDeactivatedIndex(osg::Vec3(2.0f, 4.0f, 98.0f));
             msg->AddChangedValue<bool>(osg::Vec3(-71.0f, -8.0f, -96.0f), true);
             msg->AddDeactivatedIndex(osg::Vec3(-78.0f, -9.0f, -7.0f));
             msg->AddDeactivatedIndex(osg::Vec3(-79.0f, 32.0f, -1.0f));

             dtUtil::DataStream ds;

             msg->ToDataStream(ds);

             msgResult->FromDataStream(ds);

             CPPUNIT_ASSERT_EQUAL(size_t(7U), msg->GetIndicesChanged()->GetSize());
             CPPUNIT_ASSERT_EQUAL(size_t(7U), msg->GetValuesChanged()->GetSize());

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
            voxelActor->CompleteLoad();
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
