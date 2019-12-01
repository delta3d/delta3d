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

#include <dtVoxel/voxelgeometry.h>
#include <dtVoxel/voxelactor.h>
#include <dtVoxel/voxelactorregistry.h>
#include "../dtPhysics/basedtphysicstestfixture.h"
#include <dtPhysics/physicsobject.h>
#include <openvdb/openvdb.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
#include <dtCore/system.h>

namespace dtVoxel
{
   class VoxelGeometryTests : public dtPhysics::BaseDTPhysicsTestFixture
   {
      typedef dtPhysics::BaseDTPhysicsTestFixture BaseClass;

      CPPUNIT_TEST_SUITE(VoxelGeometryTests);

         CPPUNIT_TEST(testVoxelGeometry);
         CPPUNIT_TEST(testVoxelActorGeometryCreation);
         CPPUNIT_TEST(testVoxelActorGeometryCreateRemote);

      CPPUNIT_TEST_SUITE_END();

   public:
      void GetRequiredLibraries(NameVector& names) override
      {
         static const std::string voxelLib("dtVoxel");
         static const std::string dtPhysicsLib("dtPhysics");
         BaseClass::GetRequiredLibraries(names);
         names.push_back(voxelLib);
      }
      void testVoxelGeometry()
      {
         ChangeEngine(GetPhysicsEngineList()[0]);
         testVoxelGeometry(PhysicsTesselationMode::BOX_2_TRI_PER_SIDE);
         testVoxelGeometry(PhysicsTesselationMode::BOX_1_TRI_PER_SIDE);
      }

      void testVoxelGeometry(PhysicsTesselationMode& mode)
      {
         try
         {
            dtCore::RefPtr<VoxelActor> voxelActor;
            mGM->CreateActor(*VoxelActorRegistry::VOXEL_ACTOR_TYPE, voxelActor);
            voxelActor->SetPhysicsTesselationMode(mode);
            dtPhysics::PhysicsActCompPtr pac = new dtPhysics::PhysicsActComp;
            voxelActor->AddComponent(*pac);
            dtPhysics::PhysicsObjectPtr po = dtPhysics::PhysicsObject::CreateNew("TestVoxel");
            dtPhysics::TransformType xform;
            po->SetMass(100.0f);
            po->SetMechanicsType(dtPhysics::MechanicsType::STATIC);
            po->SetPrimitiveType(dtPhysics::PrimitiveType::CUSTOM_CONCAVE_MESH);
            pac->AddPhysicsObject(*po, true);

            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
            voxelActor->SetDatabase(dtCore::ResourceDescriptor("Volumes:delta3d_island.vdb"));
            voxelActor->CompleteLoad();
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(1U));

            openvdb::BoolGrid::Ptr grid = boost::dynamic_pointer_cast<openvdb::BoolGrid>(voxelActor->GetGrid(0));
            CPPUNIT_ASSERT(grid);

            mGM->AddActor(*voxelActor, false, false);

            tbb::parallel_for(tbb::blocked_range2d<float>(-4.0f,8.0f,1, -4.0f,8.0f,1),
                  [&](const tbb::blocked_range2d<float>& r)
                  {

               std::ostringstream oss;
               std::vector<dtPhysics::RayCast::Report> hits;

               for (float i = r.rows().begin(); i < r.rows().end(); i += 0.02)
               {
                  for (float j = r.cols().begin(); j < r.cols().end(); j += 0.02)
                  {
                     dtPhysics::RayCast ray;
                     ray.SetOrigin(dtPhysics::VectorType(i, j, 220.0f));
                     ray.SetDirection(dtPhysics::VectorType(0.0f, 0.0f, -260.0f));
                     hits.clear();
                     mPhysicsComp->GetPhysicsWorld().TraceRay(ray, hits);
                     oss.str("");
                     oss << "No hits for ray x=" << i << " y=" << j;
                     CPPUNIT_ASSERT_MESSAGE(oss.str(), hits.size() > 0U);
                     size_t prevSize = hits.size();
                     ray.SetOrigin(dtPhysics::VectorType(i, 1100.0f, j));
                     ray.SetDirection(dtPhysics::VectorType(0.0f, -2200.0f, 0.0f));
                     mPhysicsComp->GetPhysicsWorld().TraceRay(ray, hits);
                     oss.str("");
                     oss << "No hits for ray x=" << i << " z=" << j;
                     CPPUNIT_ASSERT_MESSAGE(oss.str(), hits.size() > prevSize);
                     int found = 0;
                     for (unsigned k = 0; k < hits.size(); ++k)
                     {
                        dtPhysics::VectorType v = hits[k].mHitPos;
                        openvdb::Vec3d ov(v.x(), v.y(), v.z());
                        openvdb::Coord coord1 = grid->transform().worldToIndexNodeCentered(ov);
                        openvdb::Coord coord2 = grid->transform().worldToIndexCellCentered(ov);
                        std::ostringstream ss;
                        ss << "The collision coordinates should match up with cells in the grid." <<   ov << " " << coord1 << " ";
                        openvdb::BoolGrid::ConstAccessor acc = grid->getConstAccessor();
                        if (acc.isValueOn(coord1) || acc.isValueOn(coord2)) ++found;
                     }
                     CPPUNIT_ASSERT(found > 0);
                  }
               }
                  });
            mGM->DeleteActor(*voxelActor);
            dtCore::System::GetInstance().Step(0.016);
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }

      void testVoxelActorGeometryCreation()
      {
         ChangeEngine(GetPhysicsEngineList()[0]);
         try
         {
            VoxelActorPtr voxelActor;
            mGM->CreateActor(*VoxelActorRegistry::VOXEL_ACTOR_TYPE, voxelActor);
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
            voxelActor->SetDatabase(dtCore::ResourceDescriptor("Volumes:delta3d_island.vdb"));
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
            voxelActor->CompleteLoad();
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(1U));

            dtPhysics::PhysicsActCompPtr pac = new dtPhysics::PhysicsActComp;
            voxelActor->AddComponent(*pac);

            dtPhysics::PhysicsObjectPtr po = dtPhysics::PhysicsObject::CreateNew("TestVoxel");
            po->SetPrimitiveType(dtPhysics::PrimitiveType::CUSTOM_CONCAVE_MESH);
            po->SetMechanicsType(dtPhysics::MechanicsType::STATIC);
            pac->AddPhysicsObject(*po);

            mGM->AddActor(*voxelActor, false, false);

            dtPhysics::RayCast ray;
            ray.SetOrigin(dtPhysics::VectorType(12.0f, 12.0f, 200.0f));
            ray.SetDirection(dtPhysics::VectorType(0.0f, 0.0f, -205.0f));
            std::vector<dtPhysics::RayCast::Report> hits;
            mPhysicsComp->GetPhysicsWorld().TraceRay(ray, hits);
            CPPUNIT_ASSERT_EQUAL(size_t(4U), hits.size());
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }

      }

      void testVoxelActorGeometryCreateRemote()
      {
         ChangeEngine(GetPhysicsEngineList()[0]);
         testVoxelActorGeometryCreateRemote(true);
         testVoxelActorGeometryCreateRemote(false);
      }

      void testVoxelActorGeometryCreateRemote(bool createRemotePhysics)
      {
         try
         {
            VoxelActorPtr voxelActor;
            mGM->CreateRemoteActor(*VoxelActorRegistry::VOXEL_ACTOR_TYPE, voxelActor);
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
            voxelActor->SetCreateRemotePhysics(createRemotePhysics);
            voxelActor->SetDatabase(dtCore::ResourceDescriptor("Volumes:delta3d_island.vdb"));
            voxelActor->CompleteLoad();
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(1U));

            dtPhysics::PhysicsActCompPtr pac = new dtPhysics::PhysicsActComp;
            voxelActor->AddComponent(*pac);

            dtPhysics::PhysicsObjectPtr po = dtPhysics::PhysicsObject::CreateNew("TestVoxel");
            po->SetPrimitiveType(dtPhysics::PrimitiveType::CUSTOM_CONCAVE_MESH);
            po->SetMechanicsType(dtPhysics::MechanicsType::STATIC);
            pac->AddPhysicsObject(*po);

            mGM->AddActor(*voxelActor, true, false);

            dtPhysics::RayCast ray;
            ray.SetOrigin(dtPhysics::VectorType(12.0f, 12.0f, 200.0f));
            ray.SetDirection(dtPhysics::VectorType(0.0f, 0.0f, -205.0f));
            std::vector<dtPhysics::RayCast::Report> hits;
            mPhysicsComp->GetPhysicsWorld().TraceRay(ray, hits);
            CPPUNIT_ASSERT_EQUAL(createRemotePhysics? size_t(4U): size_t(0U), hits.size());

            mGM->DeleteActor(*voxelActor);
            dtCore::System::GetInstance().Step(0.016f);
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }
   };



   CPPUNIT_TEST_SUITE_REGISTRATION(VoxelGeometryTests);

} /* namespace dtVoxel */
