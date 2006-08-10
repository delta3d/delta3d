/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * @author David A. Guthrie
 */
#include <iostream>
#include "testhlaobject.h"
#include <dtDAL/enginepropertytypes.h>

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
   
   TestHLAObject *actor = dynamic_cast<TestHLAObject*> (GetActor());
   if(actor == NULL)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type TestHLAObject");
      return;
   }
   
   AddProperty(
      new dtDAL::Vec3ActorProperty("Last Known Translation", "Last Known Translation",
                                    dtDAL::MakeFunctor(*actor, &TestHLAObject::SetLastKnownTranslation),
                                    dtDAL::MakeFunctorRet(*actor, &TestHLAObject::GetLastKnownTranslation),
                                    "The last known correct position of this actor.  Used for remote actors only.", ""));
   
   AddProperty(
      new dtDAL::Vec3ActorProperty("Last Known Rotation", "Last Known Rotation",
                                    dtDAL::MakeFunctor(*this, &TestHLAObjectProxy::SetLastKnownRotation),
                                    dtDAL::MakeFunctorRet(*this, &TestHLAObjectProxy::GetLastKnownRotation),
                                    "The last known correct rotation of this actor.  Used for remote actors only.", ""));
   AddProperty(
               new dtDAL::EnumActorProperty<TestHLAObject::DamageStateEnum>("Damage State", "Damage State",
                                            dtDAL::MakeFunctor(*actor, &TestHLAObject::SetDamageState),
                                            dtDAL::MakeFunctorRet(*actor, &TestHLAObject::GetDamageState),
                                            "The general amount of damage sustained by the object.", ""));
   
}

void TestHLAObjectProxy::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
}

void TestHLAObjectProxy::CreateActor()
{
   mActor = new TestHLAObject(*this);
}

void TestHLAObjectProxy::SetLastKnownRotation(const osg::Vec3 &vec)
{
   TestHLAObject *e = dynamic_cast<TestHLAObject*> (GetActor());
   if(e == NULL)
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type TestHLAObject");
   
   e->SetLastKnownRotation(osg::Vec3(vec.z(), vec.x(), vec.y()));
}

osg::Vec3 TestHLAObjectProxy::GetLastKnownRotation() const
{
   const TestHLAObject *e = dynamic_cast<const TestHLAObject*> (GetActor());
   if(e == NULL)
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type TestHLAObject");
   
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

void TestHLAObject::SetDamageState(TestHLAObject::DamageStateEnum &damageState)
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
