/*
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
 * @author Chris Rodgers
 */
#include <prefix/dtgameprefix-src.h>
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
      new dtCore::CloudPlane(6, 0.75f, 2, 1, .2, .96, 1, 1.f, 
      "Fake Clouds", "ThisFileIsNotReal.Imaginary");

   dtCore::RefPtr<dtCore::CloudPlane> clouds = 
      new dtCore::CloudPlane(6, 0.75f, 2, 1, .2, .96, 1, 1.f, 
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
