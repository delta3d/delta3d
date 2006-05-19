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
* @author David Guthrie
*/
#include <vector>
#include <string>
#include <sstream>

#include <stdio.h>
#include <time.h>

#include <osg/Math>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <dtCore/globals.h>
#include <dtUtil/librarysharingmanager.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/actortype.h>
#include <dtDAL/actorproperty.h>

///here we need to both link in and load dynamically the same library.
#include <testActorLibrary/testActorLib.h>

#include <cppunit/extensions/HelperMacros.h>

class UtilTests : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE( UtilTests );
    CPPUNIT_TEST( TestLibrarySharing );
    CPPUNIT_TEST_SUITE_END();

public:
    
   void setUp() 
   {
      try 
      {
         dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
         std::string logName("UtilTests");
   
         logger = &dtUtil::Log::GetInstance();
         //logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
           
         //ensure the example library is unloaded.
         dtDAL::LibraryManager::GetInstance().UnloadActorRegistry(mExampleLibraryName);
   
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
   
   void tearDown() {}

   void TestLibrarySharing()
   {
      dtDAL::LibraryManager& libMgr = dtDAL::LibraryManager::GetInstance();
      std::vector<dtCore::RefPtr<dtDAL::ActorType> > actors;
      std::vector<dtDAL::ActorProperty *> props;
   
      CPPUNIT_ASSERT(libMgr.GetRegistry(mActorLibraryName) != NULL);
      
      libMgr.GetActorTypes(actors);
      
      CPPUNIT_ASSERT(actors.size() > 0);
      
      
      //make it initialize.
      dtUtil::LibrarySharingManager& lsm = dtUtil::LibrarySharingManager::GetInstance();
      
      try
      {
         dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> junk = lsm.LoadSharedLibrary("some gobbelty gook library");
         CPPUNIT_FAIL("Loading a missing library should have failed.");
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_ASSERT_MESSAGE("The exception enumeration is incorrect.", ex.TypeEnum() == dtUtil::LibrarySharingManager::ExceptionEnum::LibraryLoadingError);
      }
      
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
    static char* mExampleLibraryName;
    static char* mActorLibraryName;
    dtUtil::Log* logger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( UtilTests );

#if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
char* UtilTests::mExampleLibraryName="testActorLibraryd";
char* UtilTests::mActorLibraryName="dtActorsd";
#else
char* UtilTests::mExampleLibraryName="testActorLibrary";
char* UtilTests::mActorLibraryName="dtActors";
#endif

