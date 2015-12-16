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
* @author David Guthrie
*/
#include <prefix/unittestprefix.h>
#include <vector>
#include <string>
#include <sstream>

#include <cstdio>

#include <osg/Math>
#include <dtUtil/librarysharingmanager.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/actorfactory.h>
#include <dtCore/actortype.h>
#include <dtCore/actorproperty.h>

///here we need to both link in and load dynamically the same library.
#include <testActorLibrary/testactorlib.h>

#include <cppunit/extensions/HelperMacros.h>

class UtilTests : public CPPUNIT_NS::TestFixture 
{
    CPPUNIT_TEST_SUITE(UtilTests);
    CPPUNIT_TEST(TestLibrarySharing);
    CPPUNIT_TEST(TestPaths);
    CPPUNIT_TEST_SUITE_END();

public:
    
   void setUp() 
   {
      try 
      {
         dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
         std::string logName("UtilTests");
   
         logger = &dtUtil::Log::GetInstance();
         //logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
           
         //ensure the example library is unloaded.
         dtCore::ActorFactory::GetInstance().UnloadActorRegistry(mExampleLibraryName);
         dtCore::ActorFactory::GetInstance().UnloadActorRegistry(mActorLibraryName);
   
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
   
   void tearDown()
   {
      try
      {
         logger = &dtUtil::Log::GetInstance();
         dtCore::ActorFactory::GetInstance().UnloadActorRegistry(mExampleLibraryName);
         dtCore::ActorFactory::GetInstance().UnloadActorRegistry(mActorLibraryName);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
      }
//      catch (const std::exception& e)
//      {
//         CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
//      }
   }

   void TestPaths()
   {
      std::vector<std::string> toFill;
      
      dtUtil::LibrarySharingManager& lsm = dtUtil::LibrarySharingManager::GetInstance();
      lsm.GetSearchPath(toFill);
      CPPUNIT_ASSERT_MESSAGE("Search Path should default to empty.", toFill.empty());
      
      lsm.AddToSearchPath("hello");      
      lsm.AddToSearchPath("jojo");      

      lsm.GetSearchPath(toFill);
      CPPUNIT_ASSERT_MESSAGE("Search Path should have two items.", toFill.size() == 2);

      lsm.RemoveFromSearchPath("hello");      
      lsm.RemoveFromSearchPath("jojo");      

      lsm.GetSearchPath(toFill);
      CPPUNIT_ASSERT_MESSAGE("Search Path should be empty.", toFill.empty());
      
   }

   void TestPathSearching()
   {
      std::vector<std::string> toFill;
      
      dtUtil::LibrarySharingManager& lsm = dtUtil::LibrarySharingManager::GetInstance();
      lsm.GetSearchPath(toFill);
      CPPUNIT_ASSERT_MESSAGE("Search Path should default to empty.", toFill.empty());
      
      lsm.AddToSearchPath("hello");      
      lsm.AddToSearchPath("jojo");      

      lsm.GetSearchPath(toFill);
      CPPUNIT_ASSERT_MESSAGE("Search Path should have two items.", toFill.size() == 2);

      lsm.RemoveFromSearchPath("hello");      
      lsm.RemoveFromSearchPath("jojo");      

      lsm.GetSearchPath(toFill);
      CPPUNIT_ASSERT_MESSAGE("Search Path should be empty.", toFill.empty());
      
   }
   
   void TestLibrarySharing()
   {
      dtCore::ActorFactory& libMgr = dtCore::ActorFactory::GetInstance();
      dtCore::ActorTypeVec actors;
      std::vector<dtCore::ActorProperty*> props;
   
      CPPUNIT_ASSERT(libMgr.GetRegistry(mActorLibraryName) == nullptr);
      
      CPPUNIT_ASSERT_NO_THROW(libMgr.LoadActorRegistry(mActorLibraryName));

      libMgr.GetActorTypes(actors);
      
      CPPUNIT_ASSERT(actors.size() > 0);
      
      
      //make it initialize.
      dtUtil::LibrarySharingManager& lsm = dtUtil::LibrarySharingManager::GetInstance();
      
      CPPUNIT_ASSERT_THROW_MESSAGE("Loading a missing library should have failed.",
                                    lsm.LoadSharedLibrary("some gobbelty gook library"),
                                    dtUtil::LibrarySharingManager::LibraryLoadingException);
      
      dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> lib1 = lsm.LoadSharedLibrary(mActorLibraryName);
      dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> lib2 = lsm.LoadSharedLibrary(mExampleLibraryName);
   
      dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> lib1a = lsm.LoadSharedLibrary(mActorLibraryName);
      dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> lib2a = lsm.LoadSharedLibrary(mExampleLibraryName);
        
      CPPUNIT_ASSERT(lib1 == lib1a);
      CPPUNIT_ASSERT(lib2 == lib2a);
       
      std::ostringstream oss;
      //the library manager doesn't hold one, one is for the lsm, and one for each lib variable 
      oss << "The actor lib reference count should be 4, but it is " <<  lib1->referenceCount(); 
      
      CPPUNIT_ASSERT_MESSAGE(oss.str(), lib1->referenceCount() == 4);
   
      //One for the lsm, and one for each lib variable 
      oss.str("");
      oss << "The example lib reference count should be 3, but it is " <<  lib2->referenceCount(); 
      CPPUNIT_ASSERT_MESSAGE(oss.str(), lib2->referenceCount() == 3);
      
      CPPUNIT_ASSERT_MESSAGE("The actor libraries in the handles should be the same.", lib1 == lib1a);
      CPPUNIT_ASSERT_MESSAGE("The example actor libraries in the handles should be the same.", lib2 == lib2a);
      
      lib1a = NULL;
      lib2a = NULL;
      
      oss.str("");
      oss << "The actor lib reference count should be 3, but it is " <<  lib1->referenceCount(); 
      CPPUNIT_ASSERT_MESSAGE(oss.str(), lib1->referenceCount() == 3);
   
      oss.str("");
      oss << "The example lib reference count should be 2, but it is " <<  lib2->referenceCount(); 
      CPPUNIT_ASSERT_MESSAGE(oss.str(), lib2->referenceCount() == 2);
   
      dtUtil::LibrarySharingManager::LibraryHandle*  firstLib = lib1.get();      
      //dtUtil::LibrarySharingManager::LibraryHandle*  secondLib = lib2.get();      
      
      lib1 = NULL;
      //should close the lib.
      lib2 = NULL;
   
      //should NOT reload.
      lib1 = lsm.LoadSharedLibrary(mActorLibraryName);
      //It should have reloaded the lib.
      lib2 = lsm.LoadSharedLibrary(mExampleLibraryName);
   
      oss.str("");
      oss << "The actor lib reference count should be 3, but it is " <<  lib1->referenceCount(); 
      CPPUNIT_ASSERT_MESSAGE(oss.str(), lib1->referenceCount() == 3);
   
      oss.str("");
      oss << "The example lib reference count should be 2, but it is " <<  lib2->referenceCount(); 
      CPPUNIT_ASSERT_MESSAGE("The example lib reference count should be 2", lib2->referenceCount() == 2);
   
      CPPUNIT_ASSERT_MESSAGE("The actor library pointers should be the same.", firstLib == lib1.get());
      //Can't really test this because it will fail randomly since the code generated is likely to pick the same place
      //in memory to for the new instance.  One can check the log to see if it's opening the library twice.  It seems to be.
      //CPPUNIT_ASSERT_MESSAGE("The example actor library pointers should not be the same.", secondLib != lib2.get());
   }
    
    
private:
    static const std::string mExampleLibraryName;
    static const std::string mActorLibraryName;
    dtUtil::Log* logger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( UtilTests );

const std::string UtilTests::mExampleLibraryName="testActorLibrary";
const std::string UtilTests::mActorLibraryName="dtActors";
