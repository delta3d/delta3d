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

namespace dtVoxel
{
   class VoxelGeometryTests : public dtPhysics::BaseDTPhysicsTestFixture
   {
      typedef dtPhysics::BaseDTPhysicsTestFixture BaseClass;

      CPPUNIT_TEST_SUITE(VoxelGeometryTests);

         CPPUNIT_TEST(testVoxelGeometry);
         CPPUNIT_TEST(testVoxelActorGeometryCreation);

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
         try
         {
            dtCore::RefPtr<VoxelActor> voxelActor;
            mGM->CreateActor(*VoxelActorRegistry::VOXEL_ACTOR_TYPE, voxelActor);
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
            voxelActor->SetDatabase(dtCore::ResourceDescriptor("Volumes:delta3d_island.vdb"));
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(1U));

            openvdb::BoolGrid::Ptr grid = boost::dynamic_pointer_cast<openvdb::BoolGrid>(voxelActor->GetGrid(0));
            CPPUNIT_ASSERT(grid);
            dtPhysics::PhysicsObjectPtr po = dtPhysics::PhysicsObject::CreateNew("TestVoxel");
            dtPhysics::TransformType xform;
            VoxelGeometryPtr geometry = VoxelGeometry::CreateVoxelGeometry(xform, 100.0f, grid);
            CPPUNIT_ASSERT(geometry.valid());
            po->CreateFromGeometry(*geometry);

            dtPhysics::RayCast ray;
            ray.SetOrigin(dtPhysics::VectorType(12.0f, 12.0f, 200.0f));
            ray.SetDirection(dtPhysics::VectorType(0.0f, 0.0f, -205.0f));
            std::vector<dtPhysics::RayCast::Report> hits;
            mPhysicsComp->GetPhysicsWorld().TraceRay(ray, hits);
            CPPUNIT_ASSERT_EQUAL(size_t(1U), hits.size());
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
            CPPUNIT_ASSERT_EQUAL(size_t(1U), hits.size());
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }

      }
   };



   CPPUNIT_TEST_SUITE_REGISTRATION(VoxelGeometryTests);

} /* namespace dtVoxel */
