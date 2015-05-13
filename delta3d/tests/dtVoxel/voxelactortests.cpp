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
#include "../dtGame/basegmtests.h"

namespace dtVoxel
{
   class VectorActorTests : public dtGame::BaseGMTestFixture
   {
      typedef dtGame::BaseGMTestFixture BaseClass;
      CPPUNIT_TEST_SUITE(VectorActorTests);


      CPPUNIT_TEST_SUITE_END();

   public:
      /*override*/ void GetRequireLibraries(NameVector& names)
      {
         static const std::string voxelLib("dtVoxel");
         names.push_back(voxelLib);
      }

      /*override*/ void setUp()
      {
         BaseClass::setUp();
      }

      /*override*/ void tearDown()
      {
         BaseClass::tearDown();
      }
   };

} /* namespace dtVoxel */
