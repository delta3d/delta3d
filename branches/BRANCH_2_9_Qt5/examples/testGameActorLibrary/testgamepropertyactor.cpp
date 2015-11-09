/* -*-c++-*-
* testGameActorLibrary - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
#include "testgamepropertyactor.h"

#include <dtCore/scene.h>
#include <dtCore/object.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/colorrgbaactorproperty.h>
#include <dtCore/doubleactorproperty.h>
#include <dtCore/enumactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/gameeventactorproperty.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/longactorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/propertycontaineractorproperty.h>

#include <dtGame/messagetype.h>

#include <dtUtil/log.h>

using namespace dtCore;
using namespace dtCore;

const std::string TestGamePropertyActor::GROUPNAME("Example Game Properties");

IMPLEMENT_ENUM(TestGamePropertyActor::TestEnum);
TestGamePropertyActor::TestEnum TestGamePropertyActor::TestEnum::OPTION1("My First Option");
TestGamePropertyActor::TestEnum TestGamePropertyActor::TestEnum::OPTION2("Cool Stuff");
TestGamePropertyActor::TestEnum TestGamePropertyActor::TestEnum::OPTION3("123");
TestGamePropertyActor::TestEnum TestGamePropertyActor::TestEnum::OPTION4("Why a spoon cousin?  Because it'll hurt more.");
TestGamePropertyActor::TestEnum TestGamePropertyActor::TestEnum::OPTION5("733T H@X0R !N D37T@3D!");
TestGamePropertyActor::TestEnum TestGamePropertyActor::TestEnum::OPTION6("Mind what you have learned, save you it can");


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TestGamePropertyActor::TestGamePropertyActor()
   : dtGame::GameActorProxy()
   , mTestPropertyContainer(new TestNestedPropertyContainer)
   , mInt(0)
   , mReadOnlyInt(5)
   , mFloat(0.0f)
   , mDouble(0.0)
   , mLong(0)
   , mBool(0)
   , mString("")
   , mEnum(&TestEnum::OPTION1)
   , mRegisterListeners(false)
   , mWasRemovedFromWorld(false)
{
   SetClassName("dtCore::TestGamePropertyActor");
}

//////////////////////////////////////////////////////////////////////////////
TestGamePropertyActor::~TestGamePropertyActor()
{
}

//////////////////////////////////////////////////////////////////////////////
void TestGamePropertyActor::OnTickLocal(const dtGame::TickMessage& tickMessage)
{
   //const dtGame::TickMessage &tick =
   //   static_cast<const dtGame::TickMessage&>(tickMessage);

   // increment a message counter
   mInt++;
}

//////////////////////////////////////////////////////////////////////////////
void TestGamePropertyActor::OnTickRemote(const dtGame::TickMessage& tickMessage)
{
   //const dtGame::TickMessage &tick =
   //   static_cast<const dtGame::TickMessage&>(tickMessage);

   // increment a message counter
   mInt++;
}

void TestGamePropertyActor::ProcessMessage(const dtGame::Message &tickMessage)
{
   //const dtGame::TickMessage &tick =
   //   static_cast<const dtGame::TickMessage&>(tickMessage);

   // increment a message counter
   mInt++;
}


//////////////////////////////////////////////////////////////////////////////
void TestGamePropertyActor::BuildPropertyMap()
{
   GameActorProxy::BuildPropertyMap();


   AddProperty(new BooleanActorProperty("Test_Boolean", "Test Boolean",
      BooleanActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestBool),
      BooleanActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestBool),
      "Holds a test Boolean property", GROUPNAME));

   AddProperty(new IntActorProperty("Test_Int", "Test Int",
      IntActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestInt),
      IntActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestInt),
      "Holds a test Int property", GROUPNAME));

   dtCore::IntActorProperty *i = new IntActorProperty("Test_Read_Only_Int", "Test_Read_Only_Int",
      IntActorProperty::SetFuncType(this, &TestGamePropertyActor::SetReadOnlyTestInt),
      IntActorProperty::GetFuncType(this, &TestGamePropertyActor::GetReadOnlyTestInt),
      "Holds a test Read Only Int property", GROUPNAME);
   i->SetReadOnly(true);
   AddProperty(i);

   AddProperty(new LongActorProperty("Test_Long", "Test Long",
      LongActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestLong),
      LongActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestLong),
      "Holds a test Long property", GROUPNAME));

   AddProperty(new FloatActorProperty("Test_Float", "Test Float",
      FloatActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestFloat),
      FloatActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestFloat),
      "Holds a test Float property", GROUPNAME));

   AddProperty(new DoubleActorProperty("Test_Double", "Test Double",
      DoubleActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestDouble),
      DoubleActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestDouble),
      "Holds a test Double property", GROUPNAME));

   AddProperty(new Vec3ActorProperty("Test_Vec3", "Test Vector3",
      Vec3ActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestVec3),
      Vec3ActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestVec3),
      "Holds a test Vector3 Property", GROUPNAME));

   AddProperty(new Vec2ActorProperty("Test_Vec2", "Test Vector2",
      Vec2ActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestVec2),
      Vec2ActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestVec2),
      "Holds a test Vector2 Property", GROUPNAME));

   AddProperty(new Vec4ActorProperty("Test_Vec4", "Test Vector4",
      Vec4ActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestVec4),
      Vec4ActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestVec4),
      "Holds a test Vector4 Property", GROUPNAME));

   AddProperty(new Vec3fActorProperty("Test_Vec3f", "Test Vector3f",
      Vec3fActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestVec3f),
      Vec3fActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestVec3f),
      "Holds a test Vector3f Property", GROUPNAME));

   AddProperty(new Vec2fActorProperty("Test_Vec2f", "Test Vector2f",
      Vec2fActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestVec2f),
      Vec2fActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestVec2f),
      "Holds a test Vector2f Property", GROUPNAME));

   AddProperty(new Vec4fActorProperty("Test_Vec4f", "Test Vector4f",
      Vec4fActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestVec4f),
      Vec4fActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestVec4f),
      "Holds a test Vector4f Property", GROUPNAME));

   AddProperty(new Vec3dActorProperty("Test_Vec3d", "Test Vector3d",
      Vec3dActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestVec3d),
      Vec3dActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestVec3d),
      "Holds a test Vector3d Property", GROUPNAME));

   AddProperty(new Vec2dActorProperty("Test_Vec2d", "Test Vector2d",
      Vec2dActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestVec2d),
      Vec2dActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestVec2d),
      "Holds a test Vector2d Property", GROUPNAME));

   AddProperty(new Vec4dActorProperty("Test_Vec4d", "Test Vector4d",
      Vec4dActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestVec4d),
      Vec4dActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestVec4d),
      "Holds a test Vector4d Property", GROUPNAME));

   AddProperty(new StringActorProperty("Test_String", "Test String",
      StringActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestString),
      StringActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestString),
      "Holds a test String property (unlimited length)", GROUPNAME));

   StringActorProperty* stringProp = new StringActorProperty("Test_String2", "Test String (max 10)",
      StringActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestStringWithLength),
      StringActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestStringWithLength),
      "Holds a test String property with a max length of 10", GROUPNAME);
   stringProp->SetMaxLength(10);
   AddProperty(stringProp);

   AddProperty(new ColorRgbaActorProperty("Test_Color", "Test Color",
      ColorRgbaActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestColor),
      ColorRgbaActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestColor),
      "Holds a test Color property", GROUPNAME));

   AddProperty(new EnumActorProperty<TestGamePropertyActor::TestEnum>("Test_Enum", "Test Enum",
      EnumActorProperty<TestGamePropertyActor::TestEnum>::SetFuncType(this, &TestGamePropertyActor::SetTestEnum),
      EnumActorProperty<TestGamePropertyActor::TestEnum>::GetFuncType(this, &TestGamePropertyActor::GetTestEnum),
      "Holds a test Enum property", GROUPNAME));

   AddProperty(new ActorIDActorProperty(*this, "Test_Actor_Id", "Test Actor Id",
      dtCore::ActorIDActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestActorId),
      dtCore::ActorIDActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestActorId),
      "dtCore::Transformable",
      "An example linked actor property", GROUPNAME));

   AddProperty(new GameEventActorProperty(*this, "TestGameEvent", "Test Game Event",
            GameEventActorProperty::SetFuncType(this, &TestGamePropertyActor::SetTestGameEvent),
            GameEventActorProperty::GetFuncType(this, &TestGamePropertyActor::GetTestGameEvent),
            "Holds a test game event property", GROUPNAME));

   AddProperty(new dtCore::SimplePropertyContainerActorProperty<TestNestedPropertyContainer>("TestPropertyContainer",
            "Test Property Container",
            dtCore::SimplePropertyContainerActorProperty<TestNestedPropertyContainer>::SetFuncType(this, &TestGamePropertyActor::SetTestPropertyContainer),
            dtCore::SimplePropertyContainerActorProperty<TestNestedPropertyContainer>::GetFuncType(this, &TestGamePropertyActor::GetTestPropertyContainer),
            "", GROUPNAME));
}

//////////////////////////////////////////////////////////////////////////////
void TestGamePropertyActor::CreateDrawable()
{
   SetDrawable(*new dtGame::GameActor(*this));
}

//////////////////////////////////////////////////////////////////////////////
void TestGamePropertyActor::OnEnteredWorld()
{
   if (mRegisterListeners)
   {
      //Register an invokable for Game Events...
      RegisterForMessages(dtGame::MessageType::INFO_GAME_EVENT);

      // Register an invokable for tick messages. Local or Remote only, not both!
      if (IsRemote())
      {
         RegisterForMessages(dtGame::MessageType::TICK_REMOTE, dtGame::GameActorProxy::TICK_REMOTE_INVOKABLE);
      }
      else
      {
         RegisterForMessages(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
      }
   }

   dtGame::GameActorProxy::OnEnteredWorld();
}

//////////////////////////////////////////////////////////////////////////////
void TestGamePropertyActor::OnRemovedFromWorld()
{
   mWasRemovedFromWorld = true;
}

DT_IMPLEMENT_ACCESSOR(TestGamePropertyActor, dtCore::RefPtr<TestNestedPropertyContainer>, TestPropertyContainer)
