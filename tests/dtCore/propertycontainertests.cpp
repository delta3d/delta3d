/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2015, Caper Holdings, LLC
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
 * David Guthrie
 */
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtCore/refptr.h>

#include <dtCore/abstractenumactorproperty.h>
#include <dtCore/actoractorproperty.h>
#include <dtCore/actoridactorproperty.h>
#include <dtCore/arrayactorpropertybase.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/colorrgbaactorproperty.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/doubleactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventactorproperty.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/groupactorproperty.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/longactorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/bitmaskactorproperty.h>

#include <dtUtil/getsetmacros.h>
#include <dtCore/propertymacros.h>

#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>

#include <iostream>

namespace dtCore
{
   class TestPropertyContainer: public PropertyContainer
   {
   public:
      typedef PropertyContainer BaseClass;
      TestPropertyContainer()
      : mBool()
      , mInt()
      , mLong()
      , mFloat()
      , mDouble()
      , mString()
      , mVec3()
      , mVec4()
      //, mBitMask()
      {
         typedef PropertyRegHelper<TestPropertyContainer,TestPropertyContainer> RegHelperType;
         RegHelperType regHelper(*this, this, "Test");

         DT_REGISTER_PROPERTY(Bool, "", RegHelperType, regHelper);
         DT_REGISTER_PROPERTY(Int, "", RegHelperType, regHelper);
         DT_REGISTER_PROPERTY(Long, "", RegHelperType, regHelper);
         DT_REGISTER_PROPERTY(Float, "", RegHelperType, regHelper);
         DT_REGISTER_PROPERTY(Double, "", RegHelperType, regHelper);
         DT_REGISTER_PROPERTY(String, "", RegHelperType, regHelper);
         DT_REGISTER_PROPERTY(Vec3, "", RegHelperType, regHelper);
         DT_REGISTER_PROPERTY(Vec4, "", RegHelperType, regHelper);
         //DT_REGISTER_PROPERTY(BitMask, "", RegHelperType, regHelper);
      }
      const ObjectType& GetObjectType() const override { return *TEST_OBJ_TYPE;}

      DT_DECLARE_ACCESSOR_INLINE(bool, Bool);
      DT_DECLARE_ACCESSOR_INLINE(int, Int);
      DT_DECLARE_ACCESSOR_INLINE(long, Long);
      DT_DECLARE_ACCESSOR_INLINE(float, Float);
      DT_DECLARE_ACCESSOR_INLINE(double, Double);
      DT_DECLARE_ACCESSOR_INLINE(std::string, String);
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, Vec3);
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec4, Vec4);
      //DT_DECLARE_ACCESSOR_INLINE(unsigned, BitMask);

   private:
      static RefPtr<ObjectType> TEST_OBJ_TYPE;
   };

   typedef dtCore::RefPtr<TestPropertyContainer> TestPCPtr;

   RefPtr<ObjectType> TestPropertyContainer::TEST_OBJ_TYPE(new ObjectType("Bob", "Test", "We like bob", nullptr));

   class PropertyContainerTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(PropertyContainerTests);
      CPPUNIT_TEST(TestPropertyMetaDataDefaults);
      CPPUNIT_TEST(TestPropertyCopy);
      CPPUNIT_TEST(TestPropertyCopyMetaData);
      CPPUNIT_TEST_SUITE_END();

   public:
      PropertyContainerTests() {}
      virtual ~PropertyContainerTests() {}

      void setUp() override {}
      void tearDown() override {}
      void TestPropertyMetaDataDefaults()
      {
         TestPCPtr pc1 = new TestPropertyContainer;

         CPPUNIT_ASSERT(pc1->GetProperty("Bool") != nullptr);
         PropertyPtr boolProp = pc1->GetProperty("Bool");

         // If any of these change, it could break other unit tests, so if you change something, look for other uses.
         CPPUNIT_ASSERT(!boolProp->GetAdvanced());
         CPPUNIT_ASSERT(!boolProp->GetAlwaysSave());
         CPPUNIT_ASSERT(!boolProp->IsReadOnly());
         CPPUNIT_ASSERT(!boolProp->GetIgnoreWhenSaving());
         CPPUNIT_ASSERT(boolProp->GetMultipleEdit());
         CPPUNIT_ASSERT(boolProp->GetSendInFullUpdate());
         CPPUNIT_ASSERT(!boolProp->GetSendInPartialUpdate());
      }

      void TestPropertyCopy()
      {
         TestPCPtr pc1 = new TestPropertyContainer;
         TestPCPtr pc2 = new TestPropertyContainer;

         pc1->SetBool(true);
         pc1->SetInt(36);
         pc1->SetLong(91);
         pc1->SetFloat(78.2f);
         pc1->SetDouble(923.23325333);
         pc1->SetString("Teague is League with a T");
         pc1->SetVec3(osg::Vec3(1.1f,2.2f,3.3f));
         pc1->SetVec4(osg::Vec4(1.1f,2.2f,3.3f,7.7f));

         pc2->CopyPropertiesFrom(*pc1);

         CPPUNIT_ASSERT_EQUAL(pc1->GetBool(), pc2->GetBool());
         CPPUNIT_ASSERT_EQUAL(pc1->GetInt(), pc2->GetInt());
         CPPUNIT_ASSERT_EQUAL(pc1->GetLong(), pc2->GetLong());
         CPPUNIT_ASSERT_EQUAL(pc1->GetFloat(), pc2->GetFloat());
         CPPUNIT_ASSERT_EQUAL(pc1->GetDouble(), pc2->GetDouble());
         CPPUNIT_ASSERT_EQUAL(pc1->GetString(), pc2->GetString());
         CPPUNIT_ASSERT_EQUAL(pc1->GetVec3(), pc2->GetVec3());
         CPPUNIT_ASSERT_EQUAL(pc1->GetVec4(), pc2->GetVec4());
      }

      void TestPropertyCopyMetaData()
      {
         TestPCPtr pc1 = new TestPropertyContainer;
         TestPCPtr pc2 = new TestPropertyContainer;

         CPPUNIT_ASSERT(pc1->GetProperty("Bool") != nullptr);
         PropertyPtr boolProp = pc1->GetProperty("Bool");
         boolProp->SetAdvanced(true);
         boolProp->SetAlwaysSave(true);
         boolProp->SetReadOnly(true);
         boolProp->SetIgnoreWhenSaving(true);
         boolProp->SetMultipleEdit(false);
         boolProp->SetSendInFullUpdate(false);
         boolProp->SetSendInPartialUpdate(true);
         PropertyPtr boolProp2 = pc2->GetProperty("Bool");
         CPPUNIT_ASSERT(boolProp2.valid());

         pc2->CopyPropertiesFrom(*pc1, false);

         CPPUNIT_ASSERT(!boolProp2->GetAdvanced());
         CPPUNIT_ASSERT(!boolProp2->GetAlwaysSave());
         CPPUNIT_ASSERT(!boolProp2->IsReadOnly());
         CPPUNIT_ASSERT(!boolProp2->GetIgnoreWhenSaving());
         CPPUNIT_ASSERT(boolProp2->GetMultipleEdit());
         CPPUNIT_ASSERT(boolProp2->GetSendInFullUpdate());
         CPPUNIT_ASSERT(!boolProp2->GetSendInPartialUpdate());

         pc2->CopyPropertiesFrom(*pc1, true);

         CPPUNIT_ASSERT(boolProp2->GetAdvanced());
         CPPUNIT_ASSERT(boolProp2->GetAlwaysSave());
         CPPUNIT_ASSERT(boolProp2->IsReadOnly());
         CPPUNIT_ASSERT(boolProp2->GetIgnoreWhenSaving());
         CPPUNIT_ASSERT(!boolProp2->GetMultipleEdit());
         CPPUNIT_ASSERT(!boolProp2->GetSendInFullUpdate());
         CPPUNIT_ASSERT(boolProp2->GetSendInPartialUpdate());
      }
   private:
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(PropertyContainerTests);
}
