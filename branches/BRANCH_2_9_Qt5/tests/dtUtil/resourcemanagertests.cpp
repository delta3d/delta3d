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
* John K. Grant
* David Guthrie
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/resourcemanager.h>
#include <dtUtil/resourceloader.h>
#include <dtCore/resourcedescriptor.h>

#include <dtCore/refptr.h>

#include <string>

namespace dtUtil
{

   class MyString: public osg::Referenced
   {
   public:
      MyString(const dtCore::ResourceDescriptor& res):mRes(res){}

      dtCore::ResourceDescriptor mRes;
   };

   class TestLoader: public ResourceLoader<dtCore::ResourceDescriptor, MyString>, public osg::Referenced
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      typedef ResourceLoader<dtCore::ResourceDescriptor, MyString> BaseClass;
      typedef BaseClass::CallbackFunctor CallbackFunctor;
   
      void LoadResource(const dtCore::ResourceDescriptor& pVar, CallbackFunctor callback)
      {
         callback(pVar, new MyString(pVar), BaseClass::COMPLETE);
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

         dtCore::ResourceDescriptor resource1, resource2, resource3, resource4, resource5;
         dtCore::RefPtr<ResourceManager<dtCore::ResourceDescriptor, MyString> > mResourceManager;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( ResourceManagerTests );

   void ResourceManagerTests::setUp()
   {
      resource1 = dtCore::ResourceDescriptor("1");
      resource2 = dtCore::ResourceDescriptor("2");
      resource3 = dtCore::ResourceDescriptor("3");
      resource4 = dtCore::ResourceDescriptor("4");
      resource5 = dtCore::ResourceDescriptor("5");

      mResourceManager = new ResourceManager<dtCore::ResourceDescriptor, MyString>();
      mResourceManager->SetResourceLoader(new TestLoader());
   }

   void ResourceManagerTests::tearDown()
   {
   }


   void ResourceManagerTests::TestAddResource()
   {
      mResourceManager->AddResource(resource1, new MyString(resource1));

      mResourceManager->AddResource(resource2, new MyString(resource2));

      mResourceManager->AddResource(resource3, new MyString(resource3));

      mResourceManager->AddResource(resource4, new MyString(resource4));

      MyString* str = mResourceManager->GetResource(resource2);
      CPPUNIT_ASSERT(str->mRes == resource2);

      str = mResourceManager->GetResource(resource4);
      CPPUNIT_ASSERT(str->mRes == resource4);
   }

   
   void ResourceManagerTests::TestLoadResource()
   {
      
      mResourceManager->LoadResource(resource1);

      mResourceManager->LoadResource(resource2);

      mResourceManager->LoadResource(resource3);

      mResourceManager->LoadResource(resource4);

      MyString* str = mResourceManager->GetResource(resource2);
      CPPUNIT_ASSERT(str->mRes == resource2);

      str = mResourceManager->GetResource(resource4);
      CPPUNIT_ASSERT(str->mRes == resource4);
   }

   
   void ResourceManagerTests::TestFreeResource()
   {
      
      mResourceManager->AddResource(resource1, new MyString(resource1));

      mResourceManager->AddResource(resource2, new MyString(resource2));

      mResourceManager->AddResource(resource3, new MyString(resource3));

      mResourceManager->AddResource(resource4, new MyString(resource4));

      mResourceManager->FreeResource(resource3);
      MyString* str = mResourceManager->GetResource(resource3);
      CPPUNIT_ASSERT(str == 0);      

   }

   
   void ResourceManagerTests::TestFreeAll()
   {
      mResourceManager->AddResource(resource1, new MyString(resource1));

      mResourceManager->AddResource(resource2, new MyString(resource2));

      mResourceManager->AddResource(resource3, new MyString(resource3));

      mResourceManager->AddResource(resource4, new MyString(resource4));

      mResourceManager->FreeAll();

      MyString* str = mResourceManager->GetResource(resource1);
      CPPUNIT_ASSERT(str == 0);

      str = mResourceManager->GetResource(resource2);
      CPPUNIT_ASSERT(str == 0);

      str = mResourceManager->GetResource(resource3);
      CPPUNIT_ASSERT(str == 0);

      str = mResourceManager->GetResource(resource4);
      CPPUNIT_ASSERT(str == 0);
   }

}
