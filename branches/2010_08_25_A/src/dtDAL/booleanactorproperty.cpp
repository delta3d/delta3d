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
 * Eric Heine
 */
#include <prefix/dtdalprefix.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/datatype.h>
#include <dtUtil/stringutils.h>

namespace dtDAL
{
   ////////////////////////////////////////////////////////////////////////////////
   BooleanActorProperty::BooleanActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      const dtUtil::RefString& desc /*= ""*/, const dtUtil::RefString& groupName /*= ""*/)
      : BaseClass(DataType::BOOLEAN, name, label, set, get, desc, groupName)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   bool BooleanActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      bool result = dtUtil::ToType<bool>(value);

      SetValue(result);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string BooleanActorProperty::ToString() const
   {
      const char* result = GetValue() ? "true" : "false";
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////
}
