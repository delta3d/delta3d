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
 * David A. Guthrie
 */

#include "testhlaobject.h"

#include <dtDAL/enumactorproperty.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/resourceactorproperty.h>
#include <dtDAL/vectoractorproperties.h>

////////////////////////////////////////////////////////////////////
// Proxy Code
////////////////////////////////////////////////////////////////////
TestHLAObjectProxy::TestHLAObjectProxy()
{
   SetClassName("TestHLAObject");
}

TestHLAObjectProxy::~TestHLAObjectProxy()
{
}

void TestHLAObjectProxy::BuildPropertyMap()
{
   dtGame::GameActorProxy::BuildPropertyMap();

   TestHLAObject* actor = dynamic_cast<TestHLAObject*> (GetActor());
   if (actor == NULL)
   {
      throw dtDAL::InvalidActorException( "Actor should be type TestHLAObject", __FILE__, __LINE__);
   }

   AddProperty(
            new dtDAL::Vec3ActorProperty("Last Known Translation", "Last Known Translation",
                     dtDAL::Vec3ActorProperty::SetFuncType(actor, &TestHLAObject::SetLastKnownTranslation),
                     dtDAL::Vec3ActorProperty::GetFuncType(actor, &TestHLAObject::GetLastKnownTranslation),
                     "The last known correct position of this actor.  Used for remote actors only.", ""));

   AddProperty(
            new dtDAL::Vec3ActorProperty("Last Known Rotation", "Last Known Rotation",
                     dtDAL::Vec3ActorProperty::SetFuncType(this, &TestHLAObjectProxy::SetLastKnownRotation),
                     dtDAL::Vec3ActorProperty::GetFuncType(this, &TestHLAObjectProxy::GetLastKnownRotation),
                     "The last known correct rotation of this actor.  Used for remote actors only.", ""));
   AddProperty(
            new dtDAL::EnumActorProperty<TestHLAObject::DamageStateEnum>("Damage State", "Damage State",
                     dtDAL::EnumActorProperty<TestHLAObject::DamageStateEnum>::SetFuncType(actor, &TestHLAObject::SetDamageState),
                     dtDAL::EnumActorProperty<TestHLAObject::DamageStateEnum>::GetFuncType(actor, &TestHLAObject::GetDamageState),
                     "The general amount of damage sustained by the object.", ""));

   AddProperty(
            new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::STATIC_MESH,
                     "Mesh", "Mesh",
                     dtDAL::ResourceActorProperty::SetFuncType(actor, &TestHLAObject::TestLoadTheMesh),
                     "", ""));

}

void TestHLAObjectProxy::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
}

void TestHLAObjectProxy::CreateActor()
{
   SetActor(*new TestHLAObject(*this));
}

void TestHLAObjectProxy::SetLastKnownRotation(const osg::Vec3& vec)
{
   TestHLAObject* e = dynamic_cast<TestHLAObject*> (GetActor());
   if (e == NULL)
   {
      throw dtDAL::InvalidActorException(
      "Actor should be type TestHLAObject", __FILE__, __LINE__);
   }

   e->SetLastKnownRotation(osg::Vec3(vec.z(), vec.x(), vec.y()));
}

osg::Vec3 TestHLAObjectProxy::GetLastKnownRotation() const
{
   const TestHLAObject* e = dynamic_cast<const TestHLAObject*> (GetActor());
   if (e == NULL)
   {
      throw dtDAL::InvalidActorException(
      "Actor should be type TestHLAObject", __FILE__, __LINE__);
   }

   const osg::Vec3& result = e->GetLastKnownRotation();
   return osg::Vec3(result.y(), result.z(), result.x());
}

////////////////////////////////////////////////////////////////////
// Actor Code
////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(TestHLAObject::DamageStateEnum);
TestHLAObject::DamageStateEnum TestHLAObject::DamageStateEnum::NO_DAMAGE("No Damage");
TestHLAObject::DamageStateEnum TestHLAObject::DamageStateEnum::DAMAGED("Damaged");
TestHLAObject::DamageStateEnum TestHLAObject::DamageStateEnum::DESTROYED("Destroyed");

TestHLAObject::TestHLAObject(dtGame::GameActorProxy& proxy): dtGame::GameActor(proxy),
                                                             mDeadReckoningHelper(new dtGame::DeadReckoningHelper),
                                                             mDamageState(&TestHLAObject::DamageStateEnum::NO_DAMAGE)
{
}

TestHLAObject::~TestHLAObject()
{

}

void TestHLAObject::SetDamageState(TestHLAObject::DamageStateEnum& damageState)
{
   mDamageState = &damageState;
}

TestHLAObject::DamageStateEnum& TestHLAObject::GetDamageState() const
{
   return *mDamageState;
}

void TestHLAObject::SetLastKnownTranslation(const osg::Vec3& vec)
{
   mDeadReckoningHelper->SetLastKnownTranslation(vec);
}

void TestHLAObject::SetLastKnownRotation(const osg::Vec3& vec)
{
   mDeadReckoningHelper->SetLastKnownRotation(vec);
}

void TestHLAObject::TestLoadTheMesh(const std::string& value)
{}
