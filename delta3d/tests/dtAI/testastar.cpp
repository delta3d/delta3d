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
* @author Bradley Anderegg
*/

#include <cppunit/extensions/HelperMacros.h>
#include "testastarutils.h"
#include <list>


using namespace dtAI;

//////////////////////////////////////////////////////////////////////////
//these are static from testastarutils.h
dtAI::AStarTest_PathData* dtAI::AStarTest_PathData::sPathData = new dtAI::AStarTest_PathData(100);

dtAI::PathCostData* dtAI::PathCostData::sCostData = new dtAI::PathCostData();

typedef std::list<unsigned> PATH;
//////////////////////////////////////////////////////////////////////////


namespace dtTest
{
   class AStarTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(AStarTests);
      CPPUNIT_TEST(TestCreatePath);
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestCreatePath();

   private:
      void PrintStats(const TestAStar::config_type& pConfig);
      void TestPathForCorrectness(int pathNum, TestAStar::AStarResult pResult, const TestContainer& pPath);


      TestAStar mAStar;

      int mCorrectPaths[4][6];
   };


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( AStarTests );


void AStarTests::setUp()
{
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

void AStarTests::tearDown()
{
   delete AStarTest_PathData::sPathData;
   delete PathCostData::sCostData;
}

void AStarTests::TestCreatePath()
{  
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
}

void AStarTests::TestPathForCorrectness(int pathNum, TestAStar::AStarResult pResult, const TestContainer& pPathList)
{
   std::vector<unsigned> pPath(pPathList.begin(), pPathList.end());   

   if(pathNum == 0)
   {
      CPPUNIT_ASSERT(mCorrectPaths[0][0] == pPath[0]);
      CPPUNIT_ASSERT(mCorrectPaths[0][1] == pPath[1]);
      CPPUNIT_ASSERT(mCorrectPaths[0][2] == pPath[2]);
   }
   else if(pathNum == 1)
   {
      CPPUNIT_ASSERT(mCorrectPaths[1][0] == pPath[0]);
      CPPUNIT_ASSERT(mCorrectPaths[1][1] == pPath[1]);
      CPPUNIT_ASSERT(mCorrectPaths[1][2] == pPath[2]);
      CPPUNIT_ASSERT(mCorrectPaths[1][3] == pPath[3]);
      CPPUNIT_ASSERT(mCorrectPaths[1][4] == pPath[4]);
   }
   else if(pathNum == 2)
   {
      CPPUNIT_ASSERT(mCorrectPaths[2][0] == pPath[0]);
      CPPUNIT_ASSERT(mCorrectPaths[2][1] == pPath[1]);
      CPPUNIT_ASSERT(mCorrectPaths[2][2] == pPath[2]);
      CPPUNIT_ASSERT(mCorrectPaths[2][3] == pPath[3]);
      CPPUNIT_ASSERT(mCorrectPaths[2][4] == pPath[4]);
   }
   else if(pathNum == 3)
   {
      CPPUNIT_ASSERT(mCorrectPaths[3][0] == pPath[0]);
      CPPUNIT_ASSERT(mCorrectPaths[3][1] == pPath[1]);
      CPPUNIT_ASSERT(mCorrectPaths[3][2] == pPath[2]);
      CPPUNIT_ASSERT(mCorrectPaths[3][3] == pPath[3]);
      CPPUNIT_ASSERT(mCorrectPaths[3][4] == pPath[4]);
      CPPUNIT_ASSERT(mCorrectPaths[3][5] == pPath[5]);
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

}

