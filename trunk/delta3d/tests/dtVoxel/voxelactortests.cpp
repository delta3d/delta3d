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
#include <dtVoxel/voxelactor.h>
#include <dtVoxel/voxelactorregistry.h>
#include "../dtGame/basegmtests.h"

namespace dtVoxel
{
   class VoxelActorTests : public dtGame::BaseGMTestFixture
   {
      typedef dtGame::BaseGMTestFixture BaseClass;
      CPPUNIT_TEST_SUITE(VoxelActorTests);

         CPPUNIT_TEST(testVoxelActor);

      CPPUNIT_TEST_SUITE_END();

   public:
      /*override*/ void GetRequiredLibraries(NameVector& names)
      {
         static const std::string voxelLib("dtVoxel");
         names.push_back(voxelLib);
      }

      void testVoxelActor()
      {
         try
         {
            dtCore::RefPtr<dtVoxel::VoxelActor> voxelActor;
            mGM->CreateActor(*VoxelActorRegistry::VOXEL_ACTOR_TYPE, voxelActor);
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(0U));
            voxelActor->SetDatabase(dtCore::ResourceDescriptor("StaticMeshes:delta3d_island.vdb"));
            CPPUNIT_ASSERT_EQUAL(voxelActor->GetNumGrids(), size_t(1U));
         }
         catch(const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(VoxelActorTests);

} /* namespace dtVoxel */
