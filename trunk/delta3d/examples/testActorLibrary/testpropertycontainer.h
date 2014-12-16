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

#ifndef TESTPROPERTYCONTAINER_H_
#define TESTPROPERTYCONTAINER_H_

#include <dtCore/propertycontainer.h>
#include <dtUtil/getsetmacros.h>
/*
 * Example simple property container.  For use with the property container property example.
 */
class TestPropertyContainer : public dtCore::PropertyContainer
{
public:
   TestPropertyContainer();

   static dtCore::RefPtr<dtCore::ObjectType> TYPE;

   /*override*/ const dtCore::ObjectType& GetObjectType() const;
public:

   // This is not in the base class, it's just there as a pattern.
   virtual void BuildPropertyMap();

   DT_DECLARE_ACCESSOR(float, TestFloat)
   DT_DECLARE_ACCESSOR(bool, TestBool)
   DT_DECLARE_ACCESSOR(double, TestDouble)
   DT_DECLARE_ACCESSOR(int, TestInt)
   DT_DECLARE_ACCESSOR(int, ReadOnlyTestInt)

private:
   virtual ~TestPropertyContainer();
};

#endif /* TESTPROPERTYCONTAINER_H_ */
