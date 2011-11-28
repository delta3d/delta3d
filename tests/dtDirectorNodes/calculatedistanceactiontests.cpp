/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2011, MOVES Institute
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
* @author Erik Johnson
*/

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtDirectorNodes/calculatedistanceaction.h>
#include <dtDirectorNodes/nodelibraryregistry.h>

class CalculateDistanceActionTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(CalculateDistanceActionTests);
   CPPUNIT_TEST(TestSquaredResult);
   CPPUNIT_TEST(TestNonSquaredResult);
   CPPUNIT_TEST_SUITE_END();

public:
   virtual void setup();
   virtual void tearDown();

   void TestSquaredResult();
   void TestNonSquaredResult();
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CalculateDistanceActionTests );


//////////////////////////////////////////////////////////////////////////
void CalculateDistanceActionTests::setup()
{

}

//////////////////////////////////////////////////////////////////////////
void CalculateDistanceActionTests::tearDown()
{

}
//////////////////////////////////////////////////////////////////////////
void CalculateDistanceActionTests::TestSquaredResult()
{
   dtCore::RefPtr<dtDirector::CalculateDistanceAction> calcDistAction = new dtDirector::CalculateDistanceAction();
   osg::Vec3d pointA(0.0, 0.0, 0.0);
   osg::Vec3d pointB(10.0, 0.0, 0.0);

   calcDistAction->Init(*dtDirector::NodeLibraryRegistry::CALCULATE_DISTANCE_ACTION_NODE_TYPE, NULL);
   calcDistAction->SetA(pointA);
   calcDistAction->SetB(pointB);
   calcDistAction->SetSquaredResult(true);

  calcDistAction->Update(0.1f, 0.1f, 0, true);

  const double calculatedDist2 = osg::Vec3d(pointA-pointB).length2();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("CalculateDistanceAction didn't return correct squared distance value", 
                                calculatedDist2, calcDistAction->GetResult());

}

//////////////////////////////////////////////////////////////////////////
void CalculateDistanceActionTests::TestNonSquaredResult()
{
   dtCore::RefPtr<dtDirector::CalculateDistanceAction> calcDistAction = new dtDirector::CalculateDistanceAction();
   osg::Vec3d pointA(0.0, 0.0, 0.0);
   osg::Vec3d pointB(10.0, 0.0, 0.0);

   calcDistAction->Init(*dtDirector::NodeLibraryRegistry::CALCULATE_DISTANCE_ACTION_NODE_TYPE, NULL);
   calcDistAction->SetA(pointA);
   calcDistAction->SetB(pointB);
   calcDistAction->SetSquaredResult(false);

   calcDistAction->Update(0.1f, 0.1f, 0, true);

   const double calculatedDist = osg::Vec3d(pointA-pointB).length();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalculateDistanceAction didn't return correct distance value", 
                                 calculatedDist, calcDistAction->GetResult());
}
