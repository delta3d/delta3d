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

#include <dtCore/enumactorproperty.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/vectoractorproperties.h>

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

   TestHLAObject* actor = dynamic_cast<TestHLAObject*> (GetDrawable());
   if (actor == NULL)
   {
      throw dtCore::InvalidActorException( "Actor should be type TestHLAObject", __FILE__, __LINE__);
   }

   AddProperty(
            new dtCore::EnumActorProperty<TestHLAObject::DamageStateEnum>("Damage State", "Damage State",
                     dtCore::EnumActorProperty<TestHLAObject::DamageStateEnum>::SetFuncType(actor, &TestHLAObject::SetDamageState),
                     dtCore::EnumActorProperty<TestHLAObject::DamageStateEnum>::GetFuncType(actor, &TestHLAObject::GetDamageState),
                     "The general amount of damage sustained by the object.", ""));

   AddProperty(
            new dtCore::ResourceActorProperty(dtCore::DataType::STATIC_MESH,
                     "Mesh", "Mesh",
                     dtCore::ResourceActorProperty::SetFuncType(actor, &TestHLAObject::TestLoadTheMesh),
                     "", ""));

}

void TestHLAObjectProxy::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
}

void TestHLAObjectProxy::BuildActorComponents()
{
   dtGame::GameActorProxy::BuildActorComponents();
   AddComponent(*new dtGame::DeadReckoningActorComponent);
}

void TestHLAObjectProxy::CreateDrawable()
{
   SetDrawable(*new TestHLAObject(*this));
}


////////////////////////////////////////////////////////////////////
// Actor Code
////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(TestHLAObject::DamageStateEnum);
TestHLAObject::DamageStateEnum TestHLAObject::DamageStateEnum::NO_DAMAGE("No Damage");
TestHLAObject::DamageStateEnum TestHLAObject::DamageStateEnum::DAMAGED("Damaged");
TestHLAObject::DamageStateEnum TestHLAObject::DamageStateEnum::DESTROYED("Destroyed");

TestHLAObject::TestHLAObject(dtGame::GameActorProxy& parent): dtGame::GameActor(parent),
                                                             mDeadReckoningActorComponent(new dtGame::DeadReckoningActorComponent),
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

void TestHLAObject::TestLoadTheMesh(const std::string& value)
{}
