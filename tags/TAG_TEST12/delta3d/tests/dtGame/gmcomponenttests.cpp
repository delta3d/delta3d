/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2010, Alion Science and Technology Corporation
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 */

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/scene.h>
#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtUtil/stringutils.h>

class GMComponentTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(GMComponentTests);
   CPPUNIT_TEST(TestComponentRemovingItselfDuringMessage);
   CPPUNIT_TEST(TestComponentRemovingAnotherDuringMessage);
   CPPUNIT_TEST(TestComponentAddingAnotherDuringMessage);
   //CPPUNIT_TEST(TestComponentMessagePerformance); //disabled - just used for benchmarking
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp() {};
   void tearDown() {};

   void TestComponentRemovingItselfDuringMessage();
   void TestComponentRemovingAnotherDuringMessage();
   void TestComponentAddingAnotherDuringMessage();
   void TestComponentMessagePerformance();
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(GMComponentTests);


////////////////////////////////////////////////////////////////////////////////
void GMComponentTests::TestComponentRemovingItselfDuringMessage()
{
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
   dtCore::RefPtr<dtGame::GameManager> gm = new dtGame::GameManager(*scene);

   class CompB : public dtGame::GMComponent
   {
   public:
      CompB():
         dtGame::GMComponent("CompB")
         {}

         virtual void ProcessMessage(const dtGame::Message& message)
         {
            GetGameManager()->RemoveComponent(*this);
         }
   };

   gm->AddComponent(*new CompB());

   dtCore::System::GetInstance().Start(); 
   dtCore::System::GetInstance().Step();//CompB should remove itself here

   std::vector<dtGame::GMComponent*> comps;
   gm->GetAllComponents(comps);

   CPPUNIT_ASSERT_MESSAGE("Component didn't remove itself during a message", comps.empty());

   gm->Shutdown();
   gm = NULL;
   scene = NULL;
}

class CompA : public dtGame::GMComponent
{
public:
   CompA():
      dtGame::GMComponent("CompA")
      {}
};

class CompB : public dtGame::GMComponent
{
public:
   CompB():
      dtGame::GMComponent("CompB")
      ,mCompA(new CompA())
      {}

      virtual void ProcessMessage(const dtGame::Message& message)
      {
         GetGameManager()->RemoveComponent(*mCompA);
      }

      virtual void OnAddedToGM()
      {
         GetGameManager()->AddComponent(*mCompA);
      }

      dtCore::RefPtr<CompA> mCompA;
};

////////////////////////////////////////////////////////////////////////////////
void GMComponentTests::TestComponentRemovingAnotherDuringMessage()
{
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
   dtCore::RefPtr<dtGame::GameManager> gm = new dtGame::GameManager(*scene);

   gm->AddComponent(*new CompB());

   std::vector<dtGame::GMComponent*> comps;
   gm->GetAllComponents(comps);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Component didn't add another during a message",
                                size_t(2), comps.size());

   dtCore::System::GetInstance().Start(); 
   dtCore::System::GetInstance().Step();//CompA should have been removed here

   gm->GetAllComponents(comps);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Component didn't remove itself during a message",
                                size_t(1), comps.size());
   gm->Shutdown();
   gm = NULL;
   scene = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void GMComponentTests::TestComponentAddingAnotherDuringMessage()
{
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
   dtCore::RefPtr<dtGame::GameManager> gm = new dtGame::GameManager(*scene);

   class CompA : public dtGame::GMComponent
   {
   public:
      CompA():
         dtGame::GMComponent("CompA")
         {}
   };

   class CompB : public dtGame::GMComponent
   {
   public:
      CompB():
         dtGame::GMComponent("CompB"),
         mAdded(false)
         {}

         virtual void ProcessMessage(const dtGame::Message& message)
         {
            if (!mAdded)
            {
               GetGameManager()->AddComponent(*new CompA());
               mAdded = true;
            }
         }

         bool mAdded;
   };

   gm->AddComponent(*new CompB());
 
   dtCore::System::GetInstance().Start(); 
   dtCore::System::GetInstance().Step();//CompA should have been added here
   
   std::vector<dtGame::GMComponent*> comps;
   gm->GetAllComponents(comps);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Component didn't add another during a message",
                                size_t(2), comps.size());
   gm->Shutdown();
   gm = NULL;
   scene = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void GMComponentTests::TestComponentMessagePerformance()
{
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
   dtCore::RefPtr<dtGame::GameManager> gm = new dtGame::GameManager(*scene);

   class CompA : public dtGame::GMComponent
   {
   public:
      CompA(const std::string& name):
         dtGame::GMComponent(name)
         {}
   };

   for (int i=0; i<100; i++)
   {      
      gm->AddComponent(*new CompA(dtUtil::ToString(i)));
   }
   

   dtCore::System::GetInstance().Start(); 

   for (int i=0; i<100; i++)
   {
      dtCore::System::GetInstance().Step();
   }

   gm->Shutdown();
   gm = NULL;
   scene = NULL;
}
