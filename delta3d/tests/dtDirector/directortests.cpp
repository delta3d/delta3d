/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
* @author Roy Newton
*/
#include <prefix/dtgameprefix-src.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/datapathutils.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtDirector/director.h>
#include <dtDirector/valuenode.h>
#include <dtDAL/project.h>

/**
 * @class DirectorTests
 * @brief Unit tests for the enumeration module
 */
class DirectorTests : public CPPUNIT_NS::TestFixture {
   CPPUNIT_TEST_SUITE( DirectorTests );
   CPPUNIT_TEST( TestRunScript );
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      /**
       * Tests the running of the loaded script.
       */
      void TestRunScript();
         
   private:
      dtUtil::Log* mLogger;

   public:

      dtCore::RefPtr<dtDirector::Director> mDirector;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DirectorTests );

///////////////////////////////////////////////////////////////////////////////
void DirectorTests::setUp() 
{
   try
   {
      mLogger = &dtUtil::Log::GetInstance();

      dtDAL::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/tests/data/ProjectContext");

      mDirector = new dtDirector::Director();
      mDirector->Init();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }
   CPPUNIT_ASSERT(mLogger != NULL);
}

///////////////////////////////////////////////////////////////////////////////
void DirectorTests::tearDown() 
{
   mLogger = NULL;
   mDirector = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void DirectorTests::TestRunScript()
{	
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Director Graph.\n");

      CPPUNIT_ASSERT(mDirector->LoadScript("test"));

      // A pre-defined script should be loaded.
      CPPUNIT_ASSERT(mDirector->GetGraphRoot());
      CPPUNIT_ASSERT(!mDirector->GetGraphRoot()->GetEventNodes().empty());

      std::vector<dtDirector::Node*> nodes;
      mDirector->GetNodes("Remote Event", "Core", nodes);
      CPPUNIT_ASSERT(!nodes.empty());

      int count = (int)nodes.size();
      for (int index = 0; index < count; index++)
      {
         dtDirector::EventNode* event = dynamic_cast<dtDirector::EventNode*>(nodes[index]);
         if (event)
         {
            dtDAL::ActorProperty* prop = event->GetProperty("EventName");
            if (prop && prop->ToString() == "First")
            {
               event->Trigger();
               break;
            }
         }
      }

      while (mDirector->IsRunning())
      {
         mDirector->Update(0.5f, 0.5f);
      }

      dtDirector::ValueNode* result = mDirector->GetValueNode("Result");
      CPPUNIT_ASSERT(result);

      float resultValue = result->GetDouble();
      CPPUNIT_ASSERT(resultValue == 100);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }
}

//////////////////////////////////////////////////////////////////////////
