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

#include "testpropertycontainer.h"
#include <dtCore/propertymacros.h>

/////////////////////////////////////////////////////
TestPropertyContainer::TestPropertyContainer()
: mTestFloat(3.7f)
, mTestBool(true)
, mTestDouble(46.22)
, mTestInt(6)
, mReadOnlyTestInt(19)
{
}

dtCore::RefPtr<dtCore::ObjectType> TestPropertyContainer::TYPE(new dtCore::ObjectType("TestPropertyContainer", "dtExample"));

/*override*/ const dtCore::ObjectType& TestPropertyContainer::GetObjectType() const { return *TYPE; }

/////////////////////////////////////////////////////
TestPropertyContainer::~TestPropertyContainer()
{
}

/////////////////////////////////////////////////////
void TestPropertyContainer::BuildPropertyMap()
{
   static const dtUtil::RefString GROUPNAME("TestPropertyContainer");
   typedef dtCore::PropertyRegHelper<TestPropertyContainer&, TestPropertyContainer> PropRegHelperType;
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

   InitDefaults();
}

/////////////////////////////////////////////////////
DT_IMPLEMENT_ACCESSOR(TestPropertyContainer, float, TestFloat)
DT_IMPLEMENT_ACCESSOR(TestPropertyContainer, bool, TestBool)
DT_IMPLEMENT_ACCESSOR(TestPropertyContainer, double, TestDouble)
DT_IMPLEMENT_ACCESSOR(TestPropertyContainer, int, TestInt)
DT_IMPLEMENT_ACCESSOR(TestPropertyContainer, int, ReadOnlyTestInt)

