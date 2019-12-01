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
 * Bradley Anderegg
 */
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include "testastarutils.h"
#include <list>

using namespace dtAI;

//////////////////////////////////////////////////////////////////////////
//these are static from testastarutils.h
dtAI::AStarTest_PathData* dtAI::AStarTest_PathData::sPathData = NULL;

dtAI::PathCostData* dtAI::PathCostData::sCostData = NULL;

typedef std::list<float> PATH;
//////////////////////////////////////////////////////////////////////////


namespace dtTest
{
   class AStarTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(AStarTests);
      CPPUNIT_TEST(TestCreatePath);
      CPPUNIT_TEST(TestCreatePathVector);
      CPPUNIT_TEST(TestNavMesh);
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void init();
      void destroy();
      void TestCreatePath();
      void TestCreatePathVector();
      void TestNavMesh();

   private:
      void PrintStats(const TestAStar::config_type& pConfig);
      void TestPathForCorrectness(int pathNum, PathFindResult pResult, const TestContainer& pPath);

      TestAStar mAStar;

      float mCorrectPaths[4][6];
   };


   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(AStarTests);

   void AStarTests::setUp()
   {
   }

   void AStarTests::tearDown()
   {
   }


   void AStarTests::init()
   {
      dtAI::AStarTest_PathData::sPathData = new dtAI::AStarTest_PathData(100);
      dtAI::PathCostData::sCostData = new dtAI::PathCostData();

      // 1 to 2 cost: 1
      PATH pOne;
      pOne.push_back(2);
      PathCostData::sCostData->SetCost(1, 1, 2);

      // 1 to 4 cost: 1
      pOne.push_back(4);
      PathCostData::sCostData->SetCost(1, 1, 4);

      AStarTest_PathData::sPathData->AddPath(1, pOne);



      // 2 to 3 cost: 1
      PATH pTwo;
      pTwo.push_back(3);
      AStarTest_PathData::sPathData->AddPath(2, pTwo);
      PathCostData::sCostData->SetCost(1, 2, 3);
      PathCostData::sCostData->SetCost(2, 2, 4);
      PathCostData::sCostData->SetCost(2, 2, 5);
      PathCostData::sCostData->SetCost(4, 2, 6);

      // 3 to 5 cost: 1
      PATH pThree;
      pThree.push_back(5);
      PathCostData::sCostData->SetCost(1, 3, 5);
      PathCostData::sCostData->SetCost(3, 3, 6);

      //3 to 4 cost: 5
      pThree.push_back(4);
      PathCostData::sCostData->SetCost(5, 3, 4);

      AStarTest_PathData::sPathData->AddPath(3, pThree);


      // 4 to 5 cost: 1
      PATH pFour;
      pFour.push_back(5);
      PathCostData::sCostData->SetCost(1, 4, 5);

      // 4 to 6 cost 1
      pFour.push_back(6);
      PathCostData::sCostData->SetCost(1, 4, 6);

      AStarTest_PathData::sPathData->AddPath(4, pFour);

      // 5 to 6 cost 2
      PATH pFive;
      pFive.push_back(6);
      PathCostData::sCostData->SetCost(2, 5, 6);

      AStarTest_PathData::sPathData->AddPath(5, pFive);


      //four precomputed paths to test for correctness
      mCorrectPaths[0][0] = 1;
      mCorrectPaths[0][1] = 4;
      mCorrectPaths[0][2] = 6;

      mCorrectPaths[1][0] = 1;
      mCorrectPaths[1][1] = 2;
      mCorrectPaths[1][2] = 3;
      mCorrectPaths[1][3] = 5;
      mCorrectPaths[1][4] = 6;

      mCorrectPaths[2][0] = 1;
      mCorrectPaths[2][1] = 2;
      mCorrectPaths[2][2] = 3;
      mCorrectPaths[2][3] = 4;
      mCorrectPaths[2][4] = 6;

      mCorrectPaths[3][0] = 1;
      mCorrectPaths[3][1] = 2;
      mCorrectPaths[3][2] = 3;
      mCorrectPaths[3][3] = 4;
      mCorrectPaths[3][4] = 5;
      mCorrectPaths[3][5] = 6;

   }

   void AStarTests::destroy()
   {
      delete AStarTest_PathData::sPathData;
      delete PathCostData::sCostData;
   }

   void AStarTests::TestNavMesh()
   {
      dtAI::WaypointManager* man = &dtAI::WaypointManager::GetInstance();
      dtAI::NavMesh* navMesh = &man->GetNavMesh();

      dtAI::Waypoint one, two, three, four, five;
      one.SetID(1);
      two.SetID(2);
      three.SetID(3);
      four.SetID(4);
      five.SetID(5);

      navMesh->AddPathSegment(&one, &two);
      navMesh->AddPathSegment(&two, &three);
      navMesh->AddPathSegment(&two, &four);
      navMesh->AddPathSegment(&two, &five);
      navMesh->AddPathSegment(&two, &one);
      navMesh->AddPathSegment(&three, &four);
      navMesh->AddPathSegment(&four, &five);

      CPPUNIT_ASSERT(navMesh->ContainsPath(&one, &two));
      CPPUNIT_ASSERT(navMesh->ContainsPath(&two, &three));
      CPPUNIT_ASSERT(navMesh->ContainsPath(&two, &four));
      CPPUNIT_ASSERT(navMesh->ContainsPath(&two, &five));
      CPPUNIT_ASSERT(navMesh->ContainsPath(&two, &one));
      CPPUNIT_ASSERT(navMesh->ContainsPath(&three, &four));
      CPPUNIT_ASSERT(navMesh->ContainsPath(&four, &five));

      navMesh->RemovePathSegment(&two, &four);
      navMesh->RemovePathSegment(&four, &five);

      CPPUNIT_ASSERT(navMesh->ContainsPath(&one, &two));
      CPPUNIT_ASSERT(navMesh->ContainsPath(&two, &three));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&two, &four));
      CPPUNIT_ASSERT(navMesh->ContainsPath(&two, &five));
      CPPUNIT_ASSERT(navMesh->ContainsPath(&two, &one));
      CPPUNIT_ASSERT(navMesh->ContainsPath(&three, &four));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&four, &five));

      navMesh->RemoveAllPaths(&two);

      CPPUNIT_ASSERT(!navMesh->ContainsPath(&one, &two));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&two, &three));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&two, &four));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&two, &five));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&two, &one));
      CPPUNIT_ASSERT(navMesh->ContainsPath(&three, &four));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&four, &five));

      navMesh->RemoveAllPaths(&three);

      CPPUNIT_ASSERT(!navMesh->ContainsPath(&one, &two));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&two, &three));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&two, &four));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&two, &five));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&two, &one));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&three, &four));
      CPPUNIT_ASSERT(!navMesh->ContainsPath(&four, &five));
   }

   void AStarTests::TestCreatePath()
   {
      init();

      //test 1
      mAStar.Reset(1, 6);
      TestPathForCorrectness(0, mAStar.FindPath(), mAStar.GetPath());

      //test 2
      //change cost 1 to 4 to cost: 5
      PathCostData::sCostData->SetCost(5, 1, 4);
      mAStar.Reset(1, 6);
      TestPathForCorrectness(1, mAStar.FindPath(), mAStar.GetPath());

      //test 3
      //change cost 3 to 4 to 1
      PathCostData::sCostData->SetCost(1, 3, 4);
      PathCostData::sCostData->SetCost(2, 3, 6);
      mAStar.Reset(1, 6);
      TestPathForCorrectness(2, mAStar.FindPath(), mAStar.GetPath());

      //test 4
      //change cost 4 to 6 to 5
      PathCostData::sCostData->SetCost(5, 4, 6);
      PathCostData::sCostData->SetCost(4, 3, 6);
      //change cost 3 to 5 to 5
      PathCostData::sCostData->SetCost(5, 3, 5);
      mAStar.Reset(1, 6);
      TestPathForCorrectness(3, mAStar.FindPath(), mAStar.GetPath());

      destroy();
   }

   void AStarTests::TestCreatePathVector()
   {
      init();

      //procedural points 0, 7

      // 0 to 1 cost: 1
      PATH pZero;
      pZero.push_back(1);
      PathCostData::sCostData->SetCost(1, 0, 1);
      AStarTest_PathData::sPathData->AddPath(0, pZero);

      // 6 to 7 cost: 1
      PATH pSix;
      pSix.push_back(7);
      PathCostData::sCostData->SetCost(1, 6, 7);
      AStarTest_PathData::sPathData->AddPath(6, pSix);


      std::vector<float> pFrom;
      pFrom.push_back(0);
      pFrom.push_back(1);

      std::vector<float> pTo;
      pTo.push_back(7);
      pTo.push_back(6);

      //test 1
      mAStar.Reset(pFrom, pTo);
      TestPathForCorrectness(0, mAStar.FindPath(), mAStar.GetPath());

      destroy();
   }

   void AStarTests::TestPathForCorrectness(int pathNum, PathFindResult pResult, const TestContainer& pPathList)
   {
      std::vector<float> pPath(pPathList.begin(), pPathList.end());

      if (pathNum == 0)
      {
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[0][0],pPath[0]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[0][1],pPath[1]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[0][2],pPath[2]);
      }
      else if (pathNum == 1)
      {
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[1][0],pPath[0]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[1][1],pPath[1]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[1][2],pPath[2]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[1][3],pPath[3]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[1][4],pPath[4]);
      }
      else if (pathNum == 2)
      {
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[2][0],pPath[0]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[2][1],pPath[1]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[2][2],pPath[2]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[2][3],pPath[3]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[2][4],pPath[4]);
      }
      else if (pathNum == 3)
      {
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[3][0],pPath[0]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[3][1],pPath[1]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[3][2],pPath[2]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[3][3],pPath[3]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[3][4],pPath[4]);
        CPPUNIT_ASSERT_EQUAL(mCorrectPaths[3][5],pPath[5]);
      }

   }

   void AStarTests::PrintStats(const TestAStar::config_type& pConfig)
   {
     /* std::cout << std::endl << "STATS: " << std::endl;
      std::cout << "Path Size: " << pConfig.mResult.size() << std::endl;
      std::cout << "Nodes Explored: " << pConfig.mTotalNodesExplored << std::endl;
      std::cout << "Total Time in Ms: " << pConfig.mTotalTime << std::endl;
      std::cout << "Total Cost: " << pConfig.mTotalCost << std::endl << std::endl;*/
   }

} // namespace dtTest
