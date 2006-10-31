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
   mTextureDir = "Textures";
   if(!dtUtil::FileUtils::GetInstance().DirExists(mTextureDir))
   {
      dtUtil::FileUtils::GetInstance().MakeDirectory(mTextureDir);
   }
   dtUtil::FileUtils::GetInstance().ChangeDirectory(mTextureDir);

   mAbsolutePath = dtUtil::FileUtils::GetInstance().GetAbsolutePath(
      dtUtil::FileUtils::GetInstance().CurrentDirectory());

   mPathDelimiter = dtUtil::FileUtils::PATH_SEPARATOR;
}

void CloudPlaneTests::tearDown()
{
   dtUtil::FileUtils::GetInstance().ChangeDirectory("..");
   dtUtil::FileUtils::GetInstance().FileDelete(mAbsolutePath+mPathDelimiter+"_testclouds.png");
   CPPUNIT_ASSERT(!dtUtil::FileUtils::GetInstance().FileExists(mAbsolutePath+mPathDelimiter+"_testclouds.png"));
   dtUtil::FileUtils::GetInstance().DirDelete(mTextureDir, true);
   CPPUNIT_ASSERT(!dtUtil::FileUtils::GetInstance().DirExists(mTextureDir));
}

void CloudPlaneTests::TestSaveAndLoad()
{

   /*
   dtCore::RefPtr<dtABC::Weather> weather = new dtABC::Weather();

   // Test all weather cloud states (each call should generate a texture).
   weather->SetBasicCloudType(dtABC::Weather::CloudType::CLOUD_FEW, textureDir);
   weather->SetBasicCloudType(dtABC::Weather::CloudType::CLOUD_SCATTERED, textureDir);
   weather->SetBasicCloudType(dtABC::Weather::CloudType::CLOUD_BROKEN, textureDir);
   weather->SetBasicCloudType(dtABC::Weather::CloudType::CLOUD_OVERCAST, textureDir);

   // Test weather saving
   CPPUNIT_ASSERT( 
      weather->SaveCloudTextures(asolutePath) == 4 );

   // Test weather loading
   CPPUNIT_ASSERT( weather->LoadCloudTextures(textureDir) == 4 );//*/

   dtCore::RefPtr<dtCore::CloudPlane> clouds = 
      new dtCore::CloudPlane(6, 0.75f, 2, 1, .2, .96, 1, 1.f, 
      "Test Clouds", mTextureDir );

   // Test Save
   CPPUNIT_ASSERT( clouds->SaveTexture(mAbsolutePath+mPathDelimiter+"_testclouds.png") );

   // Test Load
   CPPUNIT_ASSERT( clouds->LoadTexture(mTextureDir+mPathDelimiter+"_testclouds.png") );


   clouds = NULL;
   //weather = NULL;
}
