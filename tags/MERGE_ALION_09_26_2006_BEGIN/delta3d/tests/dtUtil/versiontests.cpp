/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006 MOVES Institute 
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
 *
 * @author Chris Osborn
 */

#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/version.h>

namespace dtTest
{
   class VersionTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( VersionTests );
      CPPUNIT_TEST( TestGetVersion );
      CPPUNIT_TEST( TestGetLibraryName );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

      void TestGetVersion();
      void TestGetLibraryName();
   };
}

CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::VersionTests );

using namespace dtTest;

void VersionTests::setUp()
{
}

void VersionTests::tearDown()
{
}

void VersionTests::TestGetVersion()
{
#if defined(DELTA3D_VERSION_MAJOR) && DELTA3D_VERSION_MAJOR == 1 && defined(DELTA3D_VERSION_MINOR) && DELTA3D_VERSION_MINOR == 3 && defined(DELTA3D_VERSION_PATCH) && DELTA3D_VERSION_PATCH == 0
    CPPUNIT_ASSERT_EQUAL( std::string("1.3.0"),
                          std::string( Delta3DGetVersion() ) );
#endif
      
}

void VersionTests::TestGetLibraryName()
{
   CPPUNIT_ASSERT_EQUAL( std::string("Delta3D Library"),
                         std::string( Delta3DGetLibraryName() ) );
}
