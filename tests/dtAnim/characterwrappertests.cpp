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
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAnim/characterwrapper.h>

#include <dtDAL/project.h>

#include <dtCore/globals.h>
#include <dtCore/refptr.h>
#include <dtCore/transform.h>

#include <osg/Math>
#include <osg/io_utils>
#include <cmath>
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
      mCharacter = NULL;
   }

   void CharacterWrapperTests::TestCharacterWrapper()
   {
      float heading = 90.0f;

      mCharacter->SetHeading(heading);
      CPPUNIT_ASSERT_EQUAL(heading, mCharacter->GetHeading());

      mCharacter->SetRotationSpeed(heading);
      mCharacter->Update(1.0f);
      CPPUNIT_ASSERT_EQUAL(heading + heading, std::abs(mCharacter->GetHeading()));

      osg::Vec3 pos(5.0, 5.0, 10.0);
      dtCore::Transform tx;
      mCharacter->GetTransform(tx);
      tx.SetTranslation(pos);
      mCharacter->SetTransform(tx);

      //float speed = 1.0f;
      mCharacter->SetHeading(0.0f);
      mCharacter->SetRotationSpeed(0.0f);
      mCharacter->SetSpeed(1.0f);
      mCharacter->Update(1.0f);
      mCharacter->GetTransform(tx);
      tx.GetTranslation(pos);
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
