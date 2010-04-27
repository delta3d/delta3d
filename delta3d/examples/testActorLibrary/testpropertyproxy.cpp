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
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/resourceactorproperty.h>
#include <dtDAL/namedparameter.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/arrayactorproperty.h>
#include <dtDAL/arrayactorpropertybase.h>
#include <dtDAL/containeractorproperty.h>
#include <dtCore/scene.h>
#include <dtCore/object.h>
#include <dtUtil/log.h>
#include <sstream>

using namespace dtCore;
using namespace dtDAL;
using namespace dtActors;

const std::string ExampleTestPropertyProxy::GROUPNAME("Example Test");

IMPLEMENT_ENUM(ExampleTestPropertyProxy::TestEnum);
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION1("My First Option");
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION2("Cool Stuff");
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION3("123");
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION4("Why a spoon cousin?  Because it'll hurt more.");
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION5("733T H@X0R !N D37T@3D!");
ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION6("Mind what you have learned, save you it can");

///////////////////////////////////////////////////////////////////////////////
ExampleTestPropertyProxy::ExampleTestPropertyProxy()
  : mInt(0), mReadOnlyInt(5), mFloat(0.0), mDouble(0.0), mLong(0), mBool(0),
  mString(""), mEnum(&TestEnum::OPTION1), mGroupParam(new dtDAL::NamedGroupParameter("test")),
  mStringArrayIndex(0), mIntArrayIndex(0), mArrayArrayIndex(0)
{
   SetClassName("dtCore::ExampleTestPropertyProxy");
}

void ExampleTestPropertyProxy::BuildPropertyMap() 
{
   DeltaObjectActorProxy::BuildPropertyMap();

   Object *obj = dynamic_cast<Object*>(GetActor());
   if (obj == NULL)
      throw dtDAL::InvalidActorException( "Actor should be type dtCore::Object", __FILE__, __LINE__);

   AddProperty(new BooleanActorProperty("Test_Boolean", "Test Boolean",
      BooleanActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestBool),
      BooleanActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestBool),
      "Holds a test Boolean property", GROUPNAME));

   AddProperty(new IntActorProperty("Test_Int", "Test Int",
      IntActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestInt),
      IntActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestInt),
      "Holds a test Int property", GROUPNAME));

   dtDAL::IntActorProperty *i = new IntActorProperty("Test_Read_Only_Int", "Test_Read_Only_Int",
      IntActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetReadOnlyTestInt),
      IntActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetReadOnlyTestInt),
      "Holds a test Read Only Int property", GROUPNAME);
   i->SetReadOnly(true);

   AddProperty(i);

   AddProperty(new LongActorProperty("Test_Long", "Test Long",
      LongActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestLong),
      LongActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestLong),
      "Holds a test Long property", GROUPNAME));

   AddProperty(new FloatActorProperty("Test_Float", "Test Float",
      FloatActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestFloat),
      FloatActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestFloat),
      "Holds a test Float property", GROUPNAME));

   AddProperty(new DoubleActorProperty("Test_Double", "Test Double",
      DoubleActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestDouble),
      DoubleActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestDouble),
      "Holds a test Double property", GROUPNAME));

   AddProperty(new Vec3ActorProperty("Test_Vec3", "Test Vector3",
      Vec3ActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestVec3),
      Vec3ActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestVec3),
      "Holds a test Vector3 Property", GROUPNAME));

   AddProperty(new Vec2ActorProperty("Test_Vec2", "Test Vector2",
      Vec2ActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestVec2),
      Vec2ActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestVec2),
      "Holds a test Vector2 Property", GROUPNAME));

   AddProperty(new Vec4ActorProperty("Test_Vec4", "Test Vector4",
      Vec4ActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestVec4),
      Vec4ActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestVec4),
      "Holds a test Vector4 Property", GROUPNAME));

   AddProperty(new Vec3fActorProperty("Test_Vec3f", "Test Vector3f",
      Vec3fActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestVec3f),
      Vec3fActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestVec3f),
      "Holds a test Vector3f Property", GROUPNAME));

   AddProperty(new Vec2fActorProperty("Test_Vec2f", "Test Vector2f",
      Vec2fActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestVec2f),
      Vec2fActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestVec2f),
      "Holds a test Vector2f Property", GROUPNAME));

   AddProperty(new Vec4fActorProperty("Test_Vec4f", "Test Vector4f",
      Vec4fActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestVec4f),
      Vec4fActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestVec4f),
      "Holds a test Vector4f Property", GROUPNAME));

   AddProperty(new Vec3dActorProperty("Test_Vec3d", "Test Vector3d",
      Vec3dActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestVec3d),
      Vec3dActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestVec3d),
      "Holds a test Vector3d Property", GROUPNAME));

   AddProperty(new Vec2dActorProperty("Test_Vec2d", "Test Vector2d",
      Vec2dActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestVec2d),
      Vec2dActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestVec2d),
      "Holds a test Vector2d Property", GROUPNAME));

   AddProperty(new Vec4dActorProperty("Test_Vec4d", "Test Vector4d",
      Vec4dActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestVec4d),
      Vec4dActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestVec4d),
      "Holds a test Vector4d Property", GROUPNAME));

   AddProperty(new StringActorProperty("Test_String", "Test String",
      StringActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestString),
      StringActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestString),
      "Holds a test String property (unlimited length)", GROUPNAME));

   StringActorProperty* stringProp = new StringActorProperty("Test_String2", "Test String (max 10)",
      StringActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestStringWithLength),
      StringActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestStringWithLength),
      "Holds a test String property with a max length of 10", GROUPNAME);
   stringProp->SetMaxLength(10);
   AddProperty(stringProp);

   AddProperty(new ColorRgbaActorProperty("Test_Color", "Test Color",
      ColorRgbaActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestColor),
      ColorRgbaActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestColor),
      "Holds a test Color property", GROUPNAME));

   AddProperty(new EnumActorProperty<TestEnum>("Test_Enum", "Test Enum",
      EnumActorProperty<TestEnum>::SetFuncType(this, &ExampleTestPropertyProxy::SetTestEnum),
      EnumActorProperty<TestEnum>::GetFuncType(this, &ExampleTestPropertyProxy::GetTestEnum),
      "Holds a test Enum property", GROUPNAME));

   AddProperty(new ResourceActorProperty(*this, DataType::SOUND, "Test_Sound_Resource", "Test Sound",
      ResourceActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetSoundResourceName),
      "An example sound resource property", GROUPNAME));

   AddProperty(new ResourceActorProperty(*this, DataType::TEXTURE, "Test_Texture_Resource", "Texture",
      ResourceActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTextureResourceName),
      "An example texture resource property", GROUPNAME));

   AddProperty(new ActorActorProperty(*this, "Test_Actor", "Test Actor",
      ActorActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestActor),
      ActorActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestActor),
      "dtCore::Transformable",
      "An example linked actor property", GROUPNAME));

   AddProperty(new GameEventActorProperty(*this, "TestGameEvent", "Test Game Event",
      GameEventActorProperty::SetFuncType(this, &ExampleTestPropertyProxy::SetTestGameEvent),
      GameEventActorProperty::GetFuncType(this, &ExampleTestPropertyProxy::GetTestGameEvent),
      "Holds a test game event property", GROUPNAME));

   mGroupParam->AddParameter("FloatParam", dtDAL::DataType::FLOAT);
   mGroupParam->AddParameter("IntParam", dtDAL::DataType::INT);
   mGroupParam->AddParameter("StringParam", dtDAL::DataType::STRING);

   AddProperty(new GroupActorProperty("TestGroup", "Test Group Property",
      GroupActorProperty::SetFunctorType(this, &ExampleTestPropertyProxy::SetTestGroup),
      GroupActorProperty::GetFunctorType(this, &ExampleTestPropertyProxy::GetTestGroup),
      "Holds a test group", GROUPNAME));

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

