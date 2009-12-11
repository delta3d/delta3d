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
      throw dtUtil::Exception(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Object", __FILE__, __LINE__);

   AddProperty(new BooleanActorProperty("Test_Boolean", "Test Boolean",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestBool),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestBool),
      "Holds a test Boolean property", GROUPNAME));

   AddProperty(new IntActorProperty("Test_Int", "Test Int",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestInt),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestInt),
      "Holds a test Int property", GROUPNAME));

   dtDAL::IntActorProperty *i = new IntActorProperty("Test_Read_Only_Int", "Test_Read_Only_Int",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetReadOnlyTestInt),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetReadOnlyTestInt),
      "Holds a test Read Only Int property", GROUPNAME);
   i->SetReadOnly(true);

   AddProperty(i);

   AddProperty(new LongActorProperty("Test_Long", "Test Long",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestLong),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestLong),
      "Holds a test Long property", GROUPNAME));

   AddProperty(new FloatActorProperty("Test_Float", "Test Float",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestFloat),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestFloat),
      "Holds a test Float property", GROUPNAME));

   AddProperty(new DoubleActorProperty("Test_Double", "Test Double",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestDouble),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestDouble),
      "Holds a test Double property", GROUPNAME));

   AddProperty(new Vec3ActorProperty("Test_Vec3", "Test Vector3",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestVec3),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestVec3),
      "Holds a test Vector3 Property", GROUPNAME));

   AddProperty(new Vec2ActorProperty("Test_Vec2", "Test Vector2",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestVec2),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestVec2),
      "Holds a test Vector2 Property", GROUPNAME));

   AddProperty(new Vec4ActorProperty("Test_Vec4", "Test Vector4",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestVec4),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestVec4),
      "Holds a test Vector4 Property", GROUPNAME));

   AddProperty(new Vec3fActorProperty("Test_Vec3f", "Test Vector3f",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestVec3f),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestVec3f),
      "Holds a test Vector3f Property", GROUPNAME));

   AddProperty(new Vec2fActorProperty("Test_Vec2f", "Test Vector2f",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestVec2f),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestVec2f),
      "Holds a test Vector2f Property", GROUPNAME));

   AddProperty(new Vec4fActorProperty("Test_Vec4f", "Test Vector4f",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestVec4f),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestVec4f),
      "Holds a test Vector4f Property", GROUPNAME));

   AddProperty(new Vec3dActorProperty("Test_Vec3d", "Test Vector3d",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestVec3d),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestVec3d),
      "Holds a test Vector3d Property", GROUPNAME));

   AddProperty(new Vec2dActorProperty("Test_Vec2d", "Test Vector2d",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestVec2d),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestVec2d),
      "Holds a test Vector2d Property", GROUPNAME));

   AddProperty(new Vec4dActorProperty("Test_Vec4d", "Test Vector4d",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestVec4d),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestVec4d),
      "Holds a test Vector4d Property", GROUPNAME));

   AddProperty(new StringActorProperty("Test_String", "Test String",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestString),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestString),
      "Holds a test String property (unlimited length)", GROUPNAME));

   StringActorProperty *stringProp = new StringActorProperty("Test_String2", "Test String (max 10)",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestStringWithLength),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestStringWithLength),
      "Holds a test String property with a max length of 10", GROUPNAME);
   stringProp->SetMaxLength(10);
   AddProperty(stringProp);

   AddProperty(new ColorRgbaActorProperty("Test_Color", "Test Color",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestColor),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestColor),
      "Holds a test Color property", GROUPNAME));

   AddProperty(new EnumActorProperty<TestEnum>("Test_Enum", "Test Enum",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestEnum),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestEnum),
      "Holds a test Enum property", GROUPNAME));

   AddProperty(new ResourceActorProperty(*this, DataType::SOUND, "Test_Sound_Resource", "Test Sound",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetSoundResourceName),
      "An example sound resource property", GROUPNAME));

   AddProperty(new ResourceActorProperty(*this, DataType::TEXTURE, "Test_Texture_Resource", "Texture",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTextureResourceName),
      "An example texture resource property", GROUPNAME));

   AddProperty(new ActorActorProperty(*this, "Test_Actor", "Test Actor",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestActor),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestActor),
      "dtCore::Transformable",
      "An example linked actor property", GROUPNAME));

   AddProperty(new GameEventActorProperty(*this, "TestGameEvent", "Test Game Event",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestGameEvent),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestGameEvent),
      "Holds a test game event property", GROUPNAME));

   mGroupParam->AddParameter("FloatParam", dtDAL::DataType::FLOAT);
   mGroupParam->AddParameter("IntParam", dtDAL::DataType::INT);
   mGroupParam->AddParameter("StringParam", dtDAL::DataType::STRING);

   AddProperty(new GroupActorProperty("TestGroup", "Test Group Property",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestGroup),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestGroup),
      "Holds a test group", GROUPNAME));

   mStringArray.push_back("First Element");
   mStringArray.push_back("Second Element");
   mStringArray.push_back("Third Element");
   mStringArray.push_back("Fourth Element");
   mStringArray.push_back("Fifth Element");
   mStringArray.push_back("Sixth Element");

   StringActorProperty* stringProp2 = new StringActorProperty(
      "ArrayString", "Array String",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetStringArrayValue),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetStringArrayValue),
      "Holds the strings used in the Array", GROUPNAME);

   ArrayActorPropertyBase* arrayStringProp = new ArrayActorProperty<std::string>(
      "TestStringArray", "Test String Array", "Holds a test array of Strings",
      MakeFunctor(*this, &ExampleTestPropertyProxy::StringArraySetIndex),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::StringArrayGetDefault),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::StringArrayGetValue),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::StringArraySetValue),
      stringProp2, GROUPNAME);

   arrayStringProp->SetMinArraySize(2);
   arrayStringProp->SetMaxArraySize(10);

   AddProperty(arrayStringProp);

   std::string arrayString = arrayStringProp->ToString();
   arrayStringProp->FromString(arrayString);

   Vec3ActorProperty* vecContainerProp = new Vec3ActorProperty(
      "VectorProp", "Vector Prop",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetVecContainerValue),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetVecContainerValue),
      "Holds the vector used in the container", GROUPNAME);

   IntActorProperty* intContainerProp = new IntActorProperty(
      "IntProp", "Int Prop",
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetIntContainerValue),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetIntContainerValue),
      "Holds the int used in the container", GROUPNAME);

   ContainerActorProperty* containerProp = new ContainerActorProperty(
      "ContainerProp", "Container", "Contains a structure of properties",
      GROUPNAME);
   containerProp->AddProperty(vecContainerProp);
   containerProp->AddProperty(intContainerProp);

   ArrayActorPropertyBase* arrayContainerProp = new ArrayActorProperty<ExampleTestPropertyProxy::testStruct>(
      "TestContainerArray", "Array of Containers", "Holds a test array of Containers",
      MakeFunctor(*this, &ExampleTestPropertyProxy::ContainerArraySetIndex),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::ContainerArrayGetDefault),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::ContainerArrayGetValue),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::ContainerArraySetValue),
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
      MakeFunctor(*this, &ExampleTestPropertyProxy::SetIntArrayValue),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetIntArrayValue),
      "Holds the int used in the double array", GROUPNAME);

   ArrayActorPropertyBase* arrayIntProp = new ArrayActorProperty<int>(
      "TestIntArray", "Test Int Array", "Holds an array of Ints",
      MakeFunctor(*this, &ExampleTestPropertyProxy::IntArraySetIndex),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::IntArrayGetDefault),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::IntArrayGetValue),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::IntArraySetValue),
      intProp, GROUPNAME);

   ArrayActorPropertyBase* arrayArrayProp = new ArrayActorProperty<std::vector<int> >(
      "TestArrayArray", "Test Array of Arrays", "Holds an array of arrays",
      MakeFunctor(*this, &ExampleTestPropertyProxy::ArrayArraySetIndex),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::ArrayArrayGetDefault),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::ArrayArrayGetValue),
      MakeFunctorRet(*this, &ExampleTestPropertyProxy::ArrayArraySetValue),
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

