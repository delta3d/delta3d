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
#include "testgamepropertyproxy.h"
#include <dtDAL/enginepropertytypes.h>
#include <dtActors/deltaobjectactorproxy.h>
#include <dtCore/scene.h>
#include <dtCore/object.h>
#include <dtUtil/log.h>
#include <sstream>

using namespace dtCore;
using namespace dtDAL;
using namespace dtActors;

const std::string TestGamePropertyProxy::GROUPNAME("Example Game Properties");

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
TestGamePropertyActor::TestGamePropertyActor(dtGame::GameActorProxy& proxy):
   dtGame::GameActor(proxy),
   mInt(0),
   mReadOnlyInt(5),
   mFloat(0.0f),
   mDouble(0.0),
   mLong(0),
   mBool(0),
   mString(""),
   mEnum(&TestEnum::OPTION1)
{
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
//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TestGamePropertyProxy::TestGamePropertyProxy() :
   mRegisterListeners(false),
   mWasRemovedFromWorld(false)
{
   //static int count = 0;
   //std::ostringstream ss;

   SetClassName("dtCore::TestGamePropertyProxy");
}

//////////////////////////////////////////////////////////////////////////////
void TestGamePropertyProxy::BuildPropertyMap()
{
   GameActorProxy::BuildPropertyMap();

   TestGamePropertyActor& actor = static_cast<TestGamePropertyActor &>(GetGameActor());

   AddProperty(new BooleanActorProperty("Test_Boolean", "Test Boolean",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestBool),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestBool),
      "Holds a test Boolean property", GROUPNAME));

   AddProperty(new IntActorProperty("Test_Int", "Test Int",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestInt),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestInt),
      "Holds a test Int property", GROUPNAME));

   dtDAL::IntActorProperty *i = new IntActorProperty("Test_Read_Only_Int", "Test_Read_Only_Int",
      MakeFunctor(actor, &TestGamePropertyActor::SetReadOnlyTestInt),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetReadOnlyTestInt),
      "Holds a test Read Only Int property", GROUPNAME);
   i->SetReadOnly(true);
   AddProperty(i);

   AddProperty(new LongActorProperty("Test_Long", "Test Long",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestLong),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestLong),
      "Holds a test Long property", GROUPNAME));

   AddProperty(new FloatActorProperty("Test_Float", "Test Float",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestFloat),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestFloat),
      "Holds a test Float property", GROUPNAME));

   AddProperty(new DoubleActorProperty("Test_Double", "Test Double",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestDouble),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestDouble),
      "Holds a test Double property", GROUPNAME));

   AddProperty(new Vec3ActorProperty("Test_Vec3", "Test Vector3",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestVec3),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestVec3),
      "Holds a test Vector3 Property", GROUPNAME));

   AddProperty(new Vec2ActorProperty("Test_Vec2", "Test Vector2",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestVec2),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestVec2),
      "Holds a test Vector2 Property", GROUPNAME));

   AddProperty(new Vec4ActorProperty("Test_Vec4", "Test Vector4",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestVec4),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestVec4),
      "Holds a test Vector4 Property", GROUPNAME));

   AddProperty(new Vec3fActorProperty("Test_Vec3f", "Test Vector3f",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestVec3f),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestVec3f),
      "Holds a test Vector3f Property", GROUPNAME));

   AddProperty(new Vec2fActorProperty("Test_Vec2f", "Test Vector2f",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestVec2f),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestVec2f),
      "Holds a test Vector2f Property", GROUPNAME));

   AddProperty(new Vec4fActorProperty("Test_Vec4f", "Test Vector4f",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestVec4f),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestVec4f),
      "Holds a test Vector4f Property", GROUPNAME));

   AddProperty(new Vec3dActorProperty("Test_Vec3d", "Test Vector3d",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestVec3d),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestVec3d),
      "Holds a test Vector3d Property", GROUPNAME));

   AddProperty(new Vec2dActorProperty("Test_Vec2d", "Test Vector2d",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestVec2d),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestVec2d),
      "Holds a test Vector2d Property", GROUPNAME));

   AddProperty(new Vec4dActorProperty("Test_Vec4d", "Test Vector4d",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestVec4d),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestVec4d),
      "Holds a test Vector4d Property", GROUPNAME));

   AddProperty(new StringActorProperty("Test_String", "Test String",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestString),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestString),
      "Holds a test String property (unlimited length)", GROUPNAME));

   StringActorProperty *stringProp = new StringActorProperty("Test_String2", "Test String (max 10)",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestStringWithLength),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestStringWithLength),
      "Holds a test String property with a max length of 10", GROUPNAME);
   stringProp->SetMaxLength(10);
   AddProperty(stringProp);

   AddProperty(new ColorRgbaActorProperty("Test_Color", "Test Color",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestColor),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestColor),
      "Holds a test Color property", GROUPNAME));

   AddProperty(new EnumActorProperty<TestGamePropertyActor::TestEnum>("Test_Enum", "Test Enum",
      MakeFunctor(actor, &TestGamePropertyActor::SetTestEnum),
      MakeFunctorRet(actor, &TestGamePropertyActor::GetTestEnum),
      "Holds a test Enum property", GROUPNAME));

   //AddProperty(new ResourceActorProperty(*this, DataType::SOUND, "Test_Sound_Resource", "Test Sound",
   //   MakeFunctor(actor, &TestGamePropertyActor::SetSoundResourceName),
   //   "An example sound resource property", GROUPNAME));

   //AddProperty(new ResourceActorProperty(*this, DataType::TEXTURE, "Test_Texture_Resource", "Texture",
   //   MakeFunctor(actor, &TestGamePropertyActor::SetTextureResourceName),
   //   "An example texture resource property", GROUPNAME));

   AddProperty(new ActorIDActorProperty(*this, "Test_Actor_Id", "Test Actor Id",
      dtDAL::ActorIDActorProperty::SetFuncType(&actor, &TestGamePropertyActor::SetTestActorId),
      dtDAL::ActorIDActorProperty::GetFuncType(&actor, &TestGamePropertyActor::GetTestActorId),
      "dtCore::Transformable",
      "An example linked actor property", GROUPNAME));

   AddProperty(new GameEventActorProperty(*this, "TestGameEvent", "Test Game Event",
               MakeFunctor(actor, &TestGamePropertyActor::SetTestGameEvent),
               MakeFunctorRet(actor, &TestGamePropertyActor::GetTestGameEvent),
               "Holds a test game event property", GROUPNAME));
}

//////////////////////////////////////////////////////////////////////////////
void TestGamePropertyProxy::CreateActor()
{
   SetActor(*new TestGamePropertyActor(*this));
}

//////////////////////////////////////////////////////////////////////////////
void TestGamePropertyProxy::OnEnteredWorld()
{
   if (mRegisterListeners)
   {
      //Register an invokable for Game Events...
      RegisterForMessages(dtGame::MessageType::INFO_GAME_EVENT);

      // Register an invokable for tick messages. Local or Remote only, not both!
      if (IsRemote())
         RegisterForMessages(dtGame::MessageType::TICK_REMOTE, dtGame::GameActorProxy::TICK_REMOTE_INVOKABLE);
      else
         RegisterForMessages(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
   }

   dtGame::GameActorProxy::OnEnteredWorld();
}

//////////////////////////////////////////////////////////////////////////////
void TestGamePropertyProxy::OnRemovedFromWorld()
{
   mWasRemovedFromWorld = true;
}
