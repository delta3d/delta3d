/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
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
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>

namespace dtTest
{
   class DataPathUtilsTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(DataPathUtilsTests);
      CPPUNIT_TEST(TestGetHomeDirectory);
      CPPUNIT_TEST(TestEnvironment);
      CPPUNIT_TEST(TestFindFileInPathList);
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp()
         {}
         void tearDown()
         {}

         void TestGetHomeDirectory()
         {
            std::string testHomeDir = "./";
#ifdef DELTA_WIN32
            testHomeDir = dtUtil::GetEnvironment("SystemDrive") + dtUtil::GetEnvironment("HOMEPATH");
#else
            testHomeDir = dtUtil::GetEnvironment("HOME");
#endif
            CPPUNIT_ASSERT_EQUAL(testHomeDir, dtUtil::GetHomeDirectory());
         }

         /// tests handling and order of handling of multiple listeners for state changes.
         void TestEnvironment()
         {
            std::string result = dtUtil::GetEnvironment("nothing");
            CPPUNIT_ASSERT_EQUAL_MESSAGE("An environment variable that doesn't exist should yield a result of "". ", std::string(), result);
            dtUtil::SetEnvironment("silly", "goose");
            result = dtUtil::GetEnvironment("silly");
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The environment variable \"silly\" should have the value \"goose\".", std::string("goose"), result);
         }

         void TestFindFileInPathList()
         {
            std::vector<std::string> projectPaths;
            projectPaths.push_back("../ExaMples");
            projectPaths.push_back("../share/delta3d/examples");
            projectPaths.push_back("../../Examples");
            std::string result = dtUtil::FindFileInPathList("data", projectPaths);

            CPPUNIT_ASSERT(dtUtil::FileUtils::GetInstance().DirExists(result));

            result = dtUtil::FindFileInPathList("data/StaticMeshes/physics_happy_sphere.ive", projectPaths);
            CPPUNIT_ASSERT(dtUtil::FileUtils::GetInstance().FileExists(result));
#ifndef DELTA_WIN32
            result = dtUtil::FindFileInPathList("data/StaticMeshes/physics_happy_sphere.ive", projectPaths, false);
            CPPUNIT_ASSERT(result.empty());
#endif
         }
         
      private:
   };
}

CPPUNIT_TEST_SUITE_REGISTRATION(dtTest::DataPathUtilsTests);
