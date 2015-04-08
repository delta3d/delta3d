/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * Jeff P. Houde
 */

#include <prefix/dtcoreprefix.h>
#include <dtCore/stringselectoractorproperty.h>

#include <dtCore/datatype.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////////
   StringSelectorActorProperty::StringSelectorActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      GetListFuncType getList,
      const dtUtil::RefString& desc /*= ""*/, const dtUtil::RefString& groupName /*= ""*/, bool editable)
      : BaseClass(name, label, set, get, desc, groupName)
      , mEditable(editable)
      , mGetListFunc(getList)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> StringSelectorActorProperty::GetList() const
   {
      return mGetListFunc();
   }

   ////////////////////////////////////////////////////////////////////////////////
}

