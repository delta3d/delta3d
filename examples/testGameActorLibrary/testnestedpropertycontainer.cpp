/* -*-c++-*-
 * Delta3D
 * Copyright 2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#include "testnestedpropertycontainer.h"
#include <dtCore/propertymacros.h>
#include <dtCore/arrayactorpropertycomplex.h>
#include <dtCore/propertycontaineractorproperty.h>

/////////////////////////////////////////////////////
TestNestedPropertyContainer::TestNestedPropertyContainer()
   : mTestFloat(3.7f)
   , mTestBool(true)
   , mTestDouble(46.22)
   , mTestInt(6)
   , mReadOnlyTestInt(19)
{
   BuildPropertyMap();
}

/////////////////////////////////////////////////////
TestNestedPropertyContainer::~TestNestedPropertyContainer()
{
}

dtCore::RefPtr<dtCore::ObjectType> TestNestedPropertyContainer::TYPE(new dtCore::ObjectType("TestNestedPropertyContainer", "dtExample"));

/*override*/ const dtCore::ObjectType& TestNestedPropertyContainer::GetObjectType() const { return *TYPE; }

/////////////////////////////////////////////////////
void TestNestedPropertyContainer::BuildPropertyMap()
{
   static const dtUtil::RefString GROUPNAME("TestNestedPropertyContainer");
   typedef dtCore::PropertyRegHelper<TestNestedPropertyContainer&, TestNestedPropertyContainer> PropRegHelperType;
   PropRegHelperType propRegHelper(*this, this, GROUPNAME);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestFloat, "Test_Float", "Test Float", "Holds a test Float property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestDouble, "Test_Double", "Test Double", "Holds a test Double property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestBool, "Test_Boolean", "Test Boolean", "Holds a test Boolean property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TestInt, "Test_Int", "Test Int", "Holds a test Int property",
            PropRegHelperType, propRegHelper);

   DT_REGISTER_PROPERTY_WITH_NAME(ReadOnlyTestInt, "Test_Read_Only_Int", "Holds a test Read Only Int property",
            PropRegHelperType, propRegHelper);

   GetProperty("Test_Read_Only_Int")->SetReadOnly(true);


   typedef dtCore::ArrayActorPropertyComplex<dtCore::RefPtr<TestNestedPropertyContainer> > NestedArrayPropType;
   dtCore::RefPtr<NestedArrayPropType> arrayProp =
            new NestedArrayPropType
               ("TestNestedContainerArray", "TestNestedContainerArray",
                NestedArrayPropType::SetFuncType(this, &TestNestedPropertyContainer::SetNestedPropContainer),
                NestedArrayPropType::GetFuncType(this, &TestNestedPropertyContainer::GetNestedPropContainer),
                NestedArrayPropType::GetSizeFuncType(this, &TestNestedPropertyContainer::GetNumNestedPropContainers),
                NestedArrayPropType::InsertFuncType(this, &TestNestedPropertyContainer::InsertNewNestedPropContainer),
                NestedArrayPropType::RemoveFuncType(this, &TestNestedPropertyContainer::RemoveNestedPropContainer),
                "Tests the array with a property container by having a container that refers back to itself recursively.",
                GROUPNAME
               );

   dtCore::RefPtr<dtCore::BasePropertyContainerActorProperty> propContainerProp =
            new dtCore::SimplePropertyContainerActorProperty<TestNestedPropertyContainer>("NestedPropContainer",
                     "Nested Property Container",
                     dtCore::SimplePropertyContainerActorProperty<TestNestedPropertyContainer>::SetFuncType(arrayProp.get(), &NestedArrayPropType::SetCurrentValue),
                     dtCore::SimplePropertyContainerActorProperty<TestNestedPropertyContainer>::GetFuncType(arrayProp.get(), &NestedArrayPropType::GetCurrentValue),
                     "", GROUPNAME);

   arrayProp->SetArrayProperty(*propContainerProp);
   AddProperty(arrayProp);

}

/////////////////////////////////////////////////////
DT_IMPLEMENT_ACCESSOR(TestNestedPropertyContainer, float, TestFloat)
DT_IMPLEMENT_ACCESSOR(TestNestedPropertyContainer, bool, TestBool)
DT_IMPLEMENT_ACCESSOR(TestNestedPropertyContainer, double, TestDouble)
DT_IMPLEMENT_ACCESSOR(TestNestedPropertyContainer, int, TestInt)
DT_IMPLEMENT_ACCESSOR(TestNestedPropertyContainer, int, ReadOnlyTestInt)

//////////////////////////////////////////////////////////////////////////////
void TestNestedPropertyContainer::SetNestedPropContainer(int idx, TestNestedPropertyContainer* pc)
{
   mNestedContainers[idx] = pc;
}

//////////////////////////////////////////////////////////////////////////////
TestNestedPropertyContainer* TestNestedPropertyContainer::GetNestedPropContainer(int idx)
{
   return mNestedContainers[idx];
}

//////////////////////////////////////////////////////////////////////////////
void TestNestedPropertyContainer::InsertNewNestedPropContainer(int idx)
{
   mNestedContainers.insert(mNestedContainers.begin() + idx, new TestNestedPropertyContainer());
}

//////////////////////////////////////////////////////////////////////////////
void TestNestedPropertyContainer::RemoveNestedPropContainer(int idx)
{
   mNestedContainers.erase(mNestedContainers.begin() + idx);
}

//////////////////////////////////////////////////////////////////////////////
size_t TestNestedPropertyContainer::GetNumNestedPropContainers() const
{
   return mNestedContainers.size();
}

