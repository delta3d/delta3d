/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, MOVES Institute
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* @author Chris Osborn
*/
#include <prefix/unittestprefix.h>
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
