/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2006, Delta3D
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
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAnim/characterwrapper.h>

#include <dtDAL/project.h>

#include <dtCore/globals.h>
#include <dtCore/refptr.h>

#include <osg/Math>
#include <sstream>

#include <string>

namespace dtAnim
{

   class CharacterWrapperTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( CharacterWrapperTests );
      CPPUNIT_TEST( TestCharacterWrapper );
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestCharacterWrapper(); 

      dtCore::RefPtr<CharacterWrapper> mCharacter;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( CharacterWrapperTests );

   void CharacterWrapperTests::setUp()
   {      
      dtDAL::Project::GetInstance().SetContext(dtCore::GetDeltaRootPath() + "/examples/data/demoMap");

      std::string modelPath = dtCore::FindFileInPathList("SkeletalMeshes/marine_test.xml");
      CPPUNIT_ASSERT(!modelPath.empty());

      mCharacter = new CharacterWrapper(modelPath);
      
   }

   void CharacterWrapperTests::tearDown()
   {
      mCharacter = 0;
   }


   void CharacterWrapperTests::TestCharacterWrapper()
   {
      float heading = 90.0f;

      mCharacter->SetHeading(heading);
      CPPUNIT_ASSERT_EQUAL(heading, mCharacter->GetHeading());

      mCharacter->SetRotationSpeed(heading);
      mCharacter->Update(1.0f);
      CPPUNIT_ASSERT_EQUAL(heading + heading, fabs(mCharacter->GetHeading()));

      osg::Vec3 pos(5.0, 5.0, 10.0);
      dtCore::Transform tx;
      mCharacter->GetTransform(tx);
      tx.SetTranslation(pos);
      mCharacter->SetTransform(tx);
      
      float speed = 1.0f;
      mCharacter->SetHeading(0.0f);
      mCharacter->SetRotationSpeed(0.0f);
      mCharacter->SetSpeed(1.0f);
      mCharacter->Update(1.0f);
      mCharacter->GetTransform(tx);
      pos = tx.GetTranslation();
      CPPUNIT_ASSERT_EQUAL(osg::Vec3(5.0f, 6.0f, 10.0f), pos);

      CPPUNIT_ASSERT_EQUAL(0.0f, mCharacter->GetHeading());
      mCharacter->RotateToHeading(90.0f, 0.5f);
      CPPUNIT_ASSERT_EQUAL(45.0f, mCharacter->GetHeading());

      mCharacter->SetHeading(90.0f);
      mCharacter->RotateToPoint(osg::Vec3(0.0f, 1.0f, 0.0f), 1.0f);
      CPPUNIT_ASSERT_EQUAL(0.0f, mCharacter->GetHeading()); 

      mCharacter->PlayAnimation("Walk");
      mCharacter->Update(0.1f);
      CPPUNIT_ASSERT_EQUAL(true, mCharacter->IsAnimationPlaying("Walk"));
     
   } 
}
