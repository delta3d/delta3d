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
* @author Chris Rodgers
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/cloudplane.h>
#include <dtABC/weather.h>
#include <dtUtil/fileutils.h>

namespace dtTest
{
   /// unit tests for dtCore::CloudPlane
   class CloudPlaneTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( CloudPlaneTests );
      CPPUNIT_TEST( TestSaveAndLoad );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         /// tests handling and order of handling of multiple listeners for state changes.
         void TestSaveAndLoad();

      private:
         std::string mTextureDir;
         std::string mAbsolutePath;
         std::string mPathDelimiter;
   };

}

CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::CloudPlaneTests );

using namespace dtTest;

void CloudPlaneTests::setUp()
{
   mAbsolutePath = dtUtil::FileUtils::GetInstance().GetAbsolutePath(
      dtUtil::FileUtils::GetInstance().CurrentDirectory());

   mPathDelimiter = dtUtil::FileUtils::PATH_SEPARATOR;

   mTextureDir = mAbsolutePath+mPathDelimiter+"testClouds.dds";

}

void CloudPlaneTests::tearDown()
{
   dtUtil::FileUtils::GetInstance().FileDelete(mTextureDir);
   CPPUNIT_ASSERT(!dtUtil::FileUtils::GetInstance().FileExists(mTextureDir));
}

void CloudPlaneTests::TestSaveAndLoad()
{
   dtCore::RefPtr<dtCore::CloudPlane> fakeCloud = 
      new dtCore::CloudPlane(6, 0.75f, 2, 1.0f, 0.2f, 0.96f, 1, 1.0f, 
      "Fake Clouds", "ThisFileIsNotReal.Imaginary");

   dtCore::RefPtr<dtCore::CloudPlane> clouds = 
      new dtCore::CloudPlane(6, 0.75f, 2, 1.0f, 0.2f, 0.96f, 1, 1.0f, 
      "Test Clouds", "");

   bool result = clouds->SaveTexture(mTextureDir);
   // Test Save
   CPPUNIT_ASSERT(result); 

   result = clouds->LoadTexture(mTextureDir);
   // Test Load
   CPPUNIT_ASSERT(result);

   dtCore::RefPtr<dtCore::CloudPlane> clouds2 = 
      new dtCore::CloudPlane(1000.0f, "Test Clouds2", mTextureDir);

   clouds->SetColor(osg::Vec4(0.5,0.5,0.5,0.5));
   clouds2->SetColor(osg::Vec4(1.0,1.0,1.0,1.0));

   CPPUNIT_ASSERT( clouds2->GetColor() == osg::Vec4(1.0,1.0,1.0,1.0) );
   CPPUNIT_ASSERT( clouds->GetColor() != clouds2->GetColor());

   clouds->SetWind(3.0f, 3.0f); 
   clouds2->SetWind(3.0f, 3.5f);

   CPPUNIT_ASSERT( clouds->GetWind() == osg::Vec2(3.0,3.0) );
   CPPUNIT_ASSERT( clouds->GetWind() != clouds2->GetWind());

   fakeCloud = NULL;
   clouds = NULL;
   clouds2 = NULL;
}
