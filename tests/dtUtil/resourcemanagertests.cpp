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
 * John K. Grant
 * David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/resourcemanager.h>
#include <dtUtil/resourceloader.h>

#include <dtCore/refptr.h>

#include <string>

namespace dtUtil
{

   class MyString: public osg::Referenced
   {
   public:
      MyString(const std::string& pStr):mStr(pStr){} 

      std::string mStr;
   };

   class TestLoader: public ResourceLoader<std::string, MyString>
   {
   public:

   
      MyString* LoadResource(const std::string& pVar)
      {
         return new MyString(pVar);
      }
      void FreeResource(MyString* pResource)
      {
      }

   };

   class ResourceManagerTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( ResourceManagerTests );
         CPPUNIT_TEST( TestAddResource );
         CPPUNIT_TEST( TestLoadResource );
         CPPUNIT_TEST( TestFreeResource );
         CPPUNIT_TEST( TestFreeAll );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         void TestAddResource();
         void TestLoadResource(); 
         void TestFreeResource();
         void TestFreeAll();

      private:

         std::string resource1, resource2, resource3, resource4, resource5;
         dtCore::RefPtr<ResourceManager<std::string, MyString> > mResourceManager;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( ResourceManagerTests );

   void ResourceManagerTests::setUp()
   {
      resource1 = ("1");
      resource2 = ("2");
      resource3 = ("3");
      resource4 = ("4");
      resource5 = ("5");

      mResourceManager = new ResourceManager<std::string, MyString>();
      mResourceManager->SetResourceLoader(new TestLoader());
   }

   void ResourceManagerTests::tearDown()
   {
   }


   void ResourceManagerTests::TestAddResource()
   {
      mResourceManager->AddResource("resource1", new MyString(resource1));

      mResourceManager->AddResource("resource2", new MyString(resource2));

      mResourceManager->AddResource("resource3", new MyString(resource3));

      mResourceManager->AddResource("resource4", new MyString(resource4));

      MyString* str = mResourceManager->GetResource("resource2");     
      CPPUNIT_ASSERT(str->mStr == resource2);

      str = mResourceManager->GetResource("resource4");      
      CPPUNIT_ASSERT(str->mStr == resource4);
   }

   
   void ResourceManagerTests::TestLoadResource()
   {
      
      mResourceManager->LoadResource("resource1", resource1);

      mResourceManager->LoadResource("resource2", resource2);

      mResourceManager->LoadResource("resource3", resource3);

      mResourceManager->LoadResource("resource4", resource4);

      MyString* str = mResourceManager->GetResource("resource2");     
      CPPUNIT_ASSERT(str->mStr == resource2);

      str = mResourceManager->GetResource("resource4");      
      CPPUNIT_ASSERT(str->mStr == resource4);
   }

   
   void ResourceManagerTests::TestFreeResource()
   {
      
      mResourceManager->AddResource("resource1", new MyString(resource1));

      mResourceManager->AddResource("resource2", new MyString(resource2));

      mResourceManager->AddResource("resource3", new MyString(resource3));

      mResourceManager->AddResource("resource4", new MyString(resource4));

      mResourceManager->FreeResource("resource3");
      MyString* str = mResourceManager->GetResource("resource3");      
      CPPUNIT_ASSERT(str == 0);      

   }

   
   void ResourceManagerTests::TestFreeAll()
   {
      mResourceManager->AddResource("resource1", new MyString(resource1));

      mResourceManager->AddResource("resource2", new MyString(resource2));

      mResourceManager->AddResource("resource3", new MyString(resource3));

      mResourceManager->AddResource("resource4", new MyString(resource4));

      mResourceManager->FreeAll();

      MyString* str = mResourceManager->GetResource("resource1");     
      CPPUNIT_ASSERT(str == 0);

      str = mResourceManager->GetResource("resource2");      
      CPPUNIT_ASSERT(str == 0);

      str = mResourceManager->GetResource("resource3");     
      CPPUNIT_ASSERT(str == 0);

      str = mResourceManager->GetResource("resource4");      
      CPPUNIT_ASSERT(str == 0);
   }

}
