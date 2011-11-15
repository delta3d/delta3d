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
#include <prefix/unittestprefix.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/datapathutils.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtDirector/director.h>
#include <dtDirector/valuenode.h>
#include <dtCore/project.h>

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
      dtCore::RefPtr<dtDirector::Director> mDirector2;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DirectorTests );

///////////////////////////////////////////////////////////////////////////////
void DirectorTests::setUp()
{
   try
   {
      mLogger = &dtUtil::Log::GetInstance();

      dtCore::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/tests/data/ProjectContext");

      mDirector = new dtDirector::Director();
      mDirector->Init();

      mDirector2 = new dtDirector::Director();
      mDirector2->Init();
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
   mDirector2 = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void DirectorTests::TestRunScript()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Director Graph.\n");

      // Load our test script.
      try
      {
         dtCore::ResourceDescriptor resource("directors:test.dtdir");
         std::string path = dtCore::Project::GetInstance().GetResourcePath(resource);
         mDirector->LoadScript(path);
         mDirector2->LoadScript(path);
      }
      catch (dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string("dtDirector test script didn't load correctly: ") + e.ToString());
      }

      // Test XML saving.
      try
      {
         std::string contextPath = dtCore::Project::GetInstance().GetContext();
         mDirector->SaveScript(contextPath + "/directors/save test.dtdir");
      }
      catch (dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string("dtDirector test script didn't save in XML correctly: ") + e.ToString());
      }

      // Test Binary Saving.
      try
      {
         std::string contextPath = dtCore::Project::GetInstance().GetContext();
         mDirector->SaveScript(contextPath + "/directors/save test.dtdirb");
      }
      catch (dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string("dtDirector test script didn't save in binary correctly: ") + e.ToString());
      }

      // Test Binary Loading.
      try
      {
         dtCore::ResourceDescriptor resource("directors:save test.dtdirb");
         std::string path = dtCore::Project::GetInstance().GetResourcePath(resource);
         mDirector->LoadScript(path);
      }
      catch (dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string("dtDirector test script didn't load from binary correctly: ") + e.ToString());
      }

      // A pre-defined script should be loaded.
      CPPUNIT_ASSERT_MESSAGE("dtDirector didn't have a Graph Root after loading the test script", mDirector->GetGraphRoot()!=NULL);
      CPPUNIT_ASSERT_MESSAGE("dtDirector's Graph Root didn't have any EventNodes", mDirector->GetGraphRoot()->GetEventNodes().empty() == false);

      std::vector<dtDirector::Node*> nodes;
      mDirector->GetNodes("Remote Event", "Core", "EventName", "Execute Test", nodes);
      CPPUNIT_ASSERT_MESSAGE("Couldn't find the node Remote Event 'Execute Test' after loading script", nodes.empty() == false);

      // Trigger our test execution event.
      int count = (int)nodes.size();
      for (int index = 0; index < count; index++)
      {
         dtDirector::EventNode* event = dynamic_cast<dtDirector::EventNode*>(nodes[index]);
         if (event)
         {
            event->Trigger();
         }
      }

      // Wait for the test script to complete.
      while (mDirector->IsRunning())
      {
         mDirector->Update(0.5f, 0.5f);
      }

      // Once the test is complete, our result value should be true.
      dtDirector::ValueNode* result = mDirector->GetValueNode("Result");
      CPPUNIT_ASSERT_MESSAGE("Could not get the ValueNode named 'Result'", result != NULL);

      bool resultValue = result->GetBoolean();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("'Result' ValueNode didn't have the correct returned value", true, resultValue);

      // Now check the global result value on the second loaded script to make sure
      // global values work properly.
      result = mDirector2->GetValueNode("Result");
      CPPUNIT_ASSERT_MESSAGE("Could not get the ValueNode named 'Result'", result != NULL);

      resultValue = result->GetBoolean();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("'Result' ValueNode didn't have the correct returned value", true, resultValue);
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
