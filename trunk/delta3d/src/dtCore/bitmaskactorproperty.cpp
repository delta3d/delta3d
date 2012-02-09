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
#include <dtCore/bitmaskactorproperty.h>

#include <dtCore/datatype.h>
#include <dtUtil/stringutils.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////////
   BitMaskActorProperty::BitMaskActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      GetMaskListFuncType getMaskList,
      const dtUtil::RefString& desc /*= ""*/, const dtUtil::RefString& groupName /*= ""*/)
      : BaseClass(DataType::BIT_MASK, name, label, set, get, desc, groupName)
      , mGetMaskListFunc(getMaskList)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void BitMaskActorProperty::GetMaskList(std::vector<std::string>& names, std::vector<unsigned int>& values) const
   {
      mGetMaskListFunc(names, values);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool BitMaskActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      unsigned int i = dtUtil::ToType<unsigned int>(value);
      SetValue(i);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string BitMaskActorProperty::ToString() const
   {
      std::string value = dtUtil::ToString(GetValue());
      return value;
   }

   ////////////////////////////////////////////////////////////////////////////
}

