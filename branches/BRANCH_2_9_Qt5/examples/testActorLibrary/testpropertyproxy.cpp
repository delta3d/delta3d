/* -*-c++-*-
 * testActorLibrary - testpropertyproxy (.h & .cpp) - Using 'The MIT License'
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
 * Curtiss Murphy
 */
#include "testpropertyproxy.h"

#include <dtCore/object.h>
#include <dtCore/scene.h>

#include <dtCore/arrayactorproperty.h>
#include <dtCore/arrayactorpropertybase.h>
#include <dtCore/bitmaskactorproperty.h>
#include <dtCore/colorrgbaactorproperty.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/groupactorproperty.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/namedparameter.h>
#include <dtCore/propertymacros.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/propertycontaineractorproperty.h>

#include <dtUtil/log.h>

using namespace dtCore;
using namespace dtCore;
using namespace dtActors;

const dtUtil::RefString ExampleTestPropertyProxy::GROUPNAME("Example Test");

IMPLEMENT_ENUM(ExampleTestPropertyProxy::TestEnum);
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION1("My First Option");
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION2("Cool Stuff");
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION3("123");
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION4("Why a spoon cousin?  Because it'll hurt more.");
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION5("733T H@X0R !N D37T@3D!");
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION6("Mind what you have learned, save you it can");

///////////////////////////////////////////////////////////////////////////////
ExampleTestPropertyProxy::ExampleTestPropertyProxy()
  : mTestFloat()
  , mTestBool(false)
  , mTestDouble()
  , mTestInt()
  , mReadOnlyTestInt(5)
  , mTestLong()
  , mTestString()
  , mTestEnum(&TestEnum::OPTION1)
  , mTestPropertyContainer(new TestPropertyContainer)
  , mGroupParam(new dtCore::NamedGroupParameter("test"))
  , mStringArrayIndex()
  , mContainerArrayIndex()
  , mIntArrayIndex()
  , mArrayArrayIndex()
{
   SetClassName("dtCore::ExampleTestPropertyProxy");
   mTestPropertyContainer->BuildPropertyMap();
}

DT_IMPLEMENT_ACCESSOR(ExampleTestPropertyProxy, dtCore::RefPtr<TestPropertyContainer>, TestPropertyContainer)

void ExampleTestPropertyProxy::BuildPropertyMap()
{
   BaseClass::BuildPropertyMap();

   Object* obj = dynamic_cast<Object*>(GetDrawable());
   if (obj == NULL)
   {
      throw dtCore::InvalidActorException( "Actor should be type dtCore::Object", __FILE__, __LINE__);
   }

   typedef PropertyRegHelper<ExampleTestPropertyProxy&, ExampleTestPropertyProxy> PropRegHelperType;
   PropRegHelperType propRegHelper(*this, this, GROUPNAME);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestBool, "Test_Boolean", "Test Boolean", "Holds a test Boolean property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestInt, "Test_Int", "Test Int", "Holds a test Int property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME(ReadOnlyTestInt, "Test_Read_Only_Int", "Holds a test Read Only Int property",
            PropRegHelperType, propRegHelper);

   GetProperty("Test_Read_Only_Int")->SetReadOnly(true);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestLong, "Test_Long", "Test Long", "Holds a test Long property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestFloat, "Test_Float", "Test Float", "Holds a test Float property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestDouble, "Test_Double", "Test Double", "Holds a test Double property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestVec3, "Test_Vec3", "Test Vector3", "Holds a test Vector3 property",
            PropRegHelperType, propRegHelper);
   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestVec4, "Test_Vec4", "Test Vector4", "Holds a test Vector4 property",
            PropRegHelperType, propRegHelper);
   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestVec2, "Test_Vec2", "Test Vector2", "Holds a test Vector2 property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestVec2f, "Test_Vec2f", "Test Vector2f", "Holds a test Vector2f property",
            PropRegHelperType, propRegHelper);
   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestVec3f, "Test_Vec3f", "Test Vector3f", "Holds a test Vector3f property",
            PropRegHelperType, propRegHelper);
   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestVec4f, "Test_Vec4f", "Test Vector4f", "Holds a test Vector4f property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestVec2d, "Test_Vec2d", "Test Vector2d", "Holds a test Vector2d property",
            PropRegHelperType, propRegHelper);
   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestVec3d, "Test_Vec3d", "Test Vector3d", "Holds a test Vector3d property",
            PropRegHelperType, propRegHelper);
   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestVec4d, "Test_Vec4d", "Test Vector4d", "Holds a test Vector4d property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestString, "Test_String", "Test String", "Holds a test String property (unlimited length)",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestStringWithLength, "Test_String2", "Test String (max 10)",
            "Holds a test String property with a max length of 10",
            PropRegHelperType, propRegHelper);

   AddProperty(new BitMaskActorProperty("Test_Bitmask", "Test Bitmask",
      BitMaskActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestBitmask),
      BitMaskActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestBitmask),
      BitMaskActorProperty::GetMaskListFuncType(this, &ExampleTestPropertyProxy::GetBitMasks),
      "Holds a test Bitmask property", GROUPNAME));

   static_cast<dtCore::StringActorProperty*>(GetProperty("Test_String2"))->SetMaxLength(10);

   AddProperty(new ColorRgbaActorProperty("Test_Color", "Test Color",
      ColorRgbaActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestColor),
      ColorRgbaActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestColor),
      "Holds a test Color property", GROUPNAME));

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestEnum, "Test_Enum", "Test Enum",
            "Holds a test Enumeration property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_RESOURCE_PROPERTY_WITH_NAME(DataType::SOUND, TestSoundResource, "Test_Source_Resource", "Test Sound",
            "An example sound resource property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_RESOURCE_PROPERTY_WITH_NAME(DataType::TEXTURE, TestTextureResource, "Test_Texture_Resource", "Test Texture",
            "An example texture resource property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_ACTOR_ID_PROPERTY_WITH_NAME("dtCore::Transformable", TestActor, "Test_Actor", "Test Actor",
            "An example linked actor property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_LABEL(TestGameEvent, "Test Game Event",
            "Holds a test game event property",
            PropRegHelperType, propRegHelper);

   mGroupParam->AddParameter("FloatParam", dtCore::DataType::FLOAT);
   mGroupParam->AddParameter("IntParam", dtCore::DataType::INT);
   mGroupParam->AddParameter("StringParam", dtCore::DataType::STRING);

   DT_REGISTER_PROPERTY_WITH_LABEL(TestGroup, "Test Group Property",
            "Holds a test group property",
            PropRegHelperType, propRegHelper);

   mStringArray.push_back("First Element");
   mStringArray.push_back("Second Element");
   mStringArray.push_back("Third Element");
   mStringArray.push_back("Fourth Element");
   mStringArray.push_back("Fifth Element");
   mStringArray.push_back("Sixth Element");

   StringActorProperty* stringProp2 = new StringActorProperty(
      "ArrayString", "Array String",
      StringActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetStringArrayValue),
      StringActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetStringArrayValue),
      "Holds the strings used in the Array", GROUPNAME);

   ArrayActorPropertyBase* arrayStringProp = new ArrayActorProperty<std::string>(
      "TestStringArray", "Test String Array", "Holds a test array of Strings",
      ArrayActorProperty<std::string>::SetIndexFuncType(this, &ExampleTestPropertyProxy::StringArraySetIndex),
      ArrayActorProperty<std::string>::GetDefaultFuncType(this, &ExampleTestPropertyProxy::StringArrayGetDefault),
      ArrayActorProperty<std::string>::GetArrayFuncType(this, &ExampleTestPropertyProxy::StringArrayGetValue),
      ArrayActorProperty<std::string>::SetArrayFuncType(this, &ExampleTestPropertyProxy::StringArraySetValue),
      stringProp2, GROUPNAME);

   arrayStringProp->SetMinArraySize(2);
   arrayStringProp->SetMaxArraySize(10);

   AddProperty(arrayStringProp);

   std::string arrayString = arrayStringProp->ToString();
   arrayStringProp->FromString(arrayString);

   Vec3ActorProperty* vecContainerProp = new Vec3ActorProperty(
      "VectorProp", "Vector Prop",
      Vec3ActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetVecContainerValue),
      Vec3ActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetVecContainerValue),
      "Holds the vector used in the container", GROUPNAME);

   IntActorProperty* intContainerProp = new IntActorProperty(
      "IntProp", "Int Prop",
      IntActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetIntContainerValue),
      IntActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetIntContainerValue),
      "Holds the int used in the container", GROUPNAME);

   ContainerActorProperty* containerProp = new ContainerActorProperty(
      "ContainerProp", "Container", "Contains a structure of properties",
      GROUPNAME);
   containerProp->AddProperty(vecContainerProp);
   containerProp->AddProperty(intContainerProp);

   ArrayActorPropertyBase* arrayContainerProp = new ArrayActorProperty<ExampleTestPropertyProxy::testStruct>(
      "TestContainerArray", "Array of Containers", "Holds a test array of Containers",
      ArrayActorProperty<ExampleTestPropertyProxy::testStruct>::SetIndexFuncType(this, &ExampleTestPropertyProxy::ContainerArraySetIndex),
      ArrayActorProperty<ExampleTestPropertyProxy::testStruct>::GetDefaultFuncType(this, &ExampleTestPropertyProxy::ContainerArrayGetDefault),
      ArrayActorProperty<ExampleTestPropertyProxy::testStruct>::GetArrayFuncType(this, &ExampleTestPropertyProxy::ContainerArrayGetValue),
      ArrayActorProperty<ExampleTestPropertyProxy::testStruct>::SetArrayFuncType(this, &ExampleTestPropertyProxy::ContainerArraySetValue),
      containerProp, GROUPNAME, "", false);

   AddProperty(arrayContainerProp);

   arrayString = arrayContainerProp->ToString();
   arrayContainerProp->FromString(arrayString);

   std::vector<int> a;
   a.push_back(1);
   a.push_back(2);
   a.push_back(3);
   a.push_back(4);
   a.push_back(5);
   a.push_back(6);
   mArrayIntArray.push_back(a);
   mArrayIntArray.push_back(a);
   mArrayIntArray.push_back(a);
   mArrayIntArray.push_back(a);
   mArrayIntArray.push_back(a);

   IntActorProperty* intProp = new IntActorProperty(
      "ArrayInt", "Array Int",
      IntActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetIntArrayValue),
      IntActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetIntArrayValue),
      "Holds the int used in the double array", GROUPNAME);

   ArrayActorPropertyBase* arrayIntProp = new ArrayActorProperty<int>(
      "TestIntArray", "Test Int Array", "Holds an array of Ints",
      ArrayActorProperty<int>::SetIndexFuncType(this, &ExampleTestPropertyProxy::IntArraySetIndex),
      ArrayActorProperty<int>::GetDefaultFuncType(this, &ExampleTestPropertyProxy::IntArrayGetDefault),
      ArrayActorProperty<int>::GetArrayFuncType(this, &ExampleTestPropertyProxy::IntArrayGetValue),
      ArrayActorProperty<int>::SetArrayFuncType(this, &ExampleTestPropertyProxy::IntArraySetValue),
      intProp, GROUPNAME);

   ArrayActorPropertyBase* arrayArrayProp = new ArrayActorProperty<std::vector<int> >(
      "TestArrayArray", "Test Array of Arrays", "Holds an array of arrays",
      ArrayActorProperty<std::vector<int> >::SetIndexFuncType(this, &ExampleTestPropertyProxy::ArrayArraySetIndex),
      ArrayActorProperty<std::vector<int> >::GetDefaultFuncType(this, &ExampleTestPropertyProxy::ArrayArrayGetDefault),
      ArrayActorProperty<std::vector<int> >::GetArrayFuncType(this, &ExampleTestPropertyProxy::ArrayArrayGetValue),
      ArrayActorProperty<std::vector<int> >::SetArrayFuncType(this, &ExampleTestPropertyProxy::ArrayArraySetValue),
      arrayIntProp, GROUPNAME);

   AddProperty(arrayArrayProp);

   arrayString = arrayArrayProp->ToString();
   arrayArrayProp->FromString(arrayString);

   AddProperty(new dtCore::SimplePropertyContainerActorProperty<TestPropertyContainer>("TestPropertyContainer",
            "Test Property Container",
            dtCore::SimplePropertyContainerActorProperty<TestPropertyContainer>::SetFuncType(this, &ExampleTestPropertyProxy::SetTestPropertyContainer),
            dtCore::SimplePropertyContainerActorProperty<TestPropertyContainer>::GetFuncType(this, &ExampleTestPropertyProxy::GetTestPropertyContainer),
            "", GROUPNAME));
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::StringArraySetIndex(int index)
{
   mStringArrayIndex = index;
}

////////////////////////////////////////////////////////////////////////////////
std::string ExampleTestPropertyProxy::StringArrayGetDefault()
{
   return "Default";
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> ExampleTestPropertyProxy::StringArrayGetValue()
{
   return mStringArray;
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::StringArraySetValue(const std::vector<std::string>& value)
{
   mStringArray = value;
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::SetStringArrayValue(const std::string& value)
{
   if (mStringArrayIndex < (int)mStringArray.size())
   {
      mStringArray[mStringArrayIndex] = value;
   }
}

////////////////////////////////////////////////////////////////////////////////
std::string ExampleTestPropertyProxy::GetStringArrayValue()
{
   if (mStringArrayIndex < (int)mStringArray.size())
   {
      return mStringArray[mStringArrayIndex];
   }
   return "";
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::ContainerArraySetIndex(int index)
{
   mContainerArrayIndex = index;
}

////////////////////////////////////////////////////////////////////////////////
ExampleTestPropertyProxy::testStruct ExampleTestPropertyProxy::ContainerArrayGetDefault()
{
   testStruct test;
   test.vector.x() = 5.0f;
   test.vector.y() = 8.0f;
   test.vector.z() = 53.0f;
   test.value = 100;
   return test;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<ExampleTestPropertyProxy::testStruct> ExampleTestPropertyProxy::ContainerArrayGetValue()
{
   return mStructArray;
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::ContainerArraySetValue(const std::vector<ExampleTestPropertyProxy::testStruct>& value)
{
   mStructArray = value;
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::SetVecContainerValue(const osg::Vec3& value)
{
   if (mContainerArrayIndex < (int)mStructArray.size())
   {
      mStructArray[mContainerArrayIndex].vector = value;
   }
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 ExampleTestPropertyProxy::GetVecContainerValue()
{
   if (mContainerArrayIndex < (int)mStructArray.size())
   {
      return mStructArray[mContainerArrayIndex].vector;
   }
   return ContainerArrayGetDefault().vector;
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::SetIntContainerValue(int value)
{
   if (mContainerArrayIndex < (int)mStructArray.size())
   {
      mStructArray[mContainerArrayIndex].value = value;
   }
}

////////////////////////////////////////////////////////////////////////////////
int ExampleTestPropertyProxy::GetIntContainerValue()
{
   if (mContainerArrayIndex < (int)mStructArray.size())
   {
      return mStructArray[mContainerArrayIndex].value;
   }
   return ContainerArrayGetDefault().value;
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::IntArraySetIndex(int index)
{
   mIntArrayIndex = index;
}

////////////////////////////////////////////////////////////////////////////////
int ExampleTestPropertyProxy::IntArrayGetDefault()
{
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<int> ExampleTestPropertyProxy::IntArrayGetValue()
{
   return mArrayIntArray[mArrayArrayIndex];
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::IntArraySetValue(const std::vector<int>& value)
{
   mArrayIntArray[mArrayArrayIndex] = value;
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::ArrayArraySetIndex(int index)
{
   mArrayArrayIndex = index;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<int> ExampleTestPropertyProxy::ArrayArrayGetDefault()
{
   std::vector<int> def;
   return def;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::vector<int> > ExampleTestPropertyProxy::ArrayArrayGetValue()
{
   return mArrayIntArray;
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::ArrayArraySetValue(const std::vector<std::vector<int> >& value)
{
   mArrayIntArray = value;
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::SetIntArrayValue(int value)
{
   if (mArrayArrayIndex < (int)mArrayIntArray.size())
   {
      if (mIntArrayIndex < (int)mArrayIntArray[mArrayArrayIndex].size())
      {
         mArrayIntArray[mArrayArrayIndex][mIntArrayIndex] = value;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
int ExampleTestPropertyProxy::GetIntArrayValue()
{
   if (mArrayArrayIndex < (int)mArrayIntArray.size())
   {
      if (mIntArrayIndex < (int)mArrayIntArray[mArrayArrayIndex].size())
      {
         return mArrayIntArray[mArrayArrayIndex][mIntArrayIndex];
      }
   }

   return 0;
}

////////////////////////////////////////////////////////////////////////////////
void ExampleTestPropertyProxy::GetBitMasks(std::vector<std::string>& names, std::vector<unsigned int>& values)
{
   names.push_back("Bit_1");
   names.push_back("Bit_2");
   names.push_back("Bit_3");
   names.push_back("Bit_4");

   values.push_back(1);
   values.push_back(2);
   values.push_back(4);
   values.push_back(8);
}

////////////////////////////////////////////////////////////////////////////////

