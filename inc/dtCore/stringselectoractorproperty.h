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

#ifndef STRING_SELECTOR_ACTOR_PROPERTY
#define STRING_SELECTOR_ACTOR_PROPERTY

#include <dtCore/export.h>
#include <dtCore/stringactorproperty.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a string data member.  Where as the
    * property has a maximum string length, by default, the string has no
    * maximum length.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT StringSelectorActorProperty : public StringActorProperty
   {
      public:
         typedef StringActorProperty BaseClass;
         typedef dtUtil::Functor<std::vector<std::string>, TYPELIST_0()> GetListFuncType;

         StringSelectorActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           GetListFuncType getList,
                           const dtUtil::RefString& desc = "",
                           const dtUtil::RefString& groupName = "", bool editable = false);

         /**
          * Returns the available strings to select from.
          */
         std::vector<std::string> GetList() const;

         /**
          * Retrieves whether this property can be editing with
          * custom strings, or if it is forced to only use the
          * string selector.
          */
         bool IsEditable() const {return mEditable;}

      protected:
         virtual ~StringSelectorActorProperty() { }

         bool            mEditable;
         GetListFuncType mGetListFunc;
      };
}

#endif //STRING_SELECTOR_ACTOR_PROPERTY
