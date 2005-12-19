/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Roy Newton
 */

#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/deprecationmgr.h>
#include <cppunit/extensions/HelperMacros.h>


/**
 * @class DeprecationMgrTests
 * @brief Unit tests for the deprecation manager
 */
class DeprecationMgrTests : public CPPUNIT_NS::TestFixture {
   CPPUNIT_TEST_SUITE( DeprecationMgrTests );
   CPPUNIT_TEST( TestAddDeprecatedFunction );   
   CPPUNIT_TEST( TestCallDeprecatedFunction );   
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      /**
       * Tests adding a deprecated function to the manager
       */
      void TestAddDeprecatedFunction();

      /**
      * Tests calling a deprecated function that has been added to the manager
      */
      void TestCallDeprecatedFunction();

   private:
      /**
       * Function to deprecate (no args)
       */
      void funcToDeprecate1();
      
      /**
      * Function to deprecate (args)
      */
      void funcToDeprecate2(int x, char* y);

      /**
      * The replacement function for funcToDeprecate1()
      */
      void correctFunction1();

      /**
      * The replacement function for funcToDeprecate2(int x, char* y)
      */
      void correctFunction2(int x, std::string& y);
      
      dtUtil::Log* mLogger; 
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DeprecationMgrTests );

///////////////////////////////////////////////////////////////////////////////
void DeprecationMgrTests::setUp() 
{
   try
   {
      mLogger = &dtUtil::Log::GetInstance();		
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
void DeprecationMgrTests::tearDown() 
{
   mLogger = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void DeprecationMgrTests::TestAddDeprecatedFunction()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Adding a deprecated function.\n");
      DEPRECATE("", "");
      DEPRECATE("void DeprecationMgrTests::functionToDeprecate1()", "DeprecationMgrTests::correctFunction1()");
      DEPRECATE("void DeprecationMgrTests::functionToDeprecate2(int x, char* y)", "DeprecationMgrTests::correctFunction2(int x, std::string& y)");      
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

///////////////////////////////////////////////////////////////////////////////
void DeprecationMgrTests::TestCallDeprecatedFunction()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Calling a deprecated function.\n");
      funcToDeprecate1();
      funcToDeprecate2(1, "test");
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

void DeprecationMgrTests::correctFunction1(){}
void DeprecationMgrTests::correctFunction2(int x, std::string& y){}
void DeprecationMgrTests::funcToDeprecate1(){}
void DeprecationMgrTests::funcToDeprecate2(int x, char* y){}
