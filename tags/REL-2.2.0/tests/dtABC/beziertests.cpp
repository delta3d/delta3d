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
* Erik Johnson
*/

#include <cppunit/extensions/HelperMacros.h>
#include <dtABC/beziernode.h>
#include <dtABC/beziercontrolpoint.h>
#include <dtABC/beziercontroller.h>
#include <dtCore/refptr.h>

/// unit tests for Bezier curve classes
class BezierTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE( BezierTests );
   CPPUNIT_TEST( TestReachingEndOfPath );
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp() {};
   void tearDown() {};
   void TestReachingEndOfPath();
};

CPPUNIT_TEST_SUITE_REGISTRATION( BezierTests );


void BezierTests::TestReachingEndOfPath()
{
   using namespace dtABC;
   using namespace dtCore;

   RefPtr<BezierNode> node0 = new BezierNode();
   RefPtr<BezierNode> node1 = new BezierNode();
   RefPtr<BezierNode> node2 = new BezierNode();
   RefPtr<BezierControlPoint> ctrl0 = new BezierControlPoint();
   RefPtr<BezierControlPoint> ctrl1 = new BezierControlPoint();
   RefPtr<BezierControlPoint> ctrl2 = new BezierControlPoint();
   RefPtr<BezierControlPoint> ctrl3 = new BezierControlPoint();

   Transform xform;
   xform.SetTranslation(0.f, 0.f, 0.f);
   node0->SetTransform(xform);
   node0->SetTimeToNext(1.f);
   node0->SetStep(0.02f);
   node0->SetNext(node1.get());
   node0->SetExit(ctrl0.get());

   xform.SetTranslation(0.f, -10.f, 0.f);
   node1->SetTransform(xform);
   node1->SetTimeToNext(1.f);
   node1->SetStep(0.02f);
   node1->SetNext(node2.get());
   node1->SetEntry(ctrl1.get());
   node1->SetExit(ctrl2.get());
   node1->SetPrev(node0.get());

   xform.SetTranslation(0.f, -20.f, 0.f);
   node2->SetTransform(xform);
   node2->SetTimeToNext(1.f);
   node2->SetStep(0.02f);
   node2->SetEntry(ctrl3.get());
   node2->SetPrev(node1.get());

   ctrl0->SetParent(node0.get());
   ctrl1->SetParent(node1.get());
   ctrl2->SetParent(node1.get());
   ctrl3->SetParent(node2.get());

   RefPtr<Transformable> target = new Transformable("object");

   RefPtr<BezierController> controller = new BezierController();
   controller->SetTimeStep(0.01f);
   controller->SetTargetObject(target.get());
   controller->SetStartNode(node0.get());
   controller->CheckCreatePath();
   controller->Start();
   controller->Update(0.4f);
   controller->Update(0.4f);
   controller->Update(0.4f);
   controller->Update(0.4f);
   controller->Update(0.4f);
   controller->Update(0.4f);
   controller->Update(0.4f);
   controller->Update(0.4f);
}
