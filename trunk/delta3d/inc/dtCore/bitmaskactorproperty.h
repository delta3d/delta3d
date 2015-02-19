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

#ifndef BIT_MASK_ACTOR_PROPERTY
#define BIT_MASK_ACTOR_PROPERTY

#include <dtCore/export.h>
#include <dtCore/genericactorproperty.h>

#include <vector>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a bit mask data member.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT BitMaskActorProperty : public GenericActorProperty<unsigned int, unsigned int>
   {
      public:

         typedef GenericActorProperty<unsigned int, unsigned int> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;
         typedef dtUtil::Functor<void, TYPELIST_2(std::vector<std::string>&, std::vector<unsigned int>&)> GetMaskListFuncType;

         BitMaskActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                          SetFuncType set, GetFuncType get, GetMaskListFuncType getMaskList,
                          const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "");

         /**
          * Returns the available strings to select from.
          *
          * @param[in]  names   A list of mask names.
          * @param[in]  values  A list of mask values.
          */
         void GetMaskList(std::vector<std::string>& names, std::vector<unsigned int>& values) const;

         /**
          * Sets the value of the property based on a string.
          * The string should be an integer value as a string.  This will set the value to 0 if it's not valid.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);

         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~BitMaskActorProperty() { }

         GetMaskListFuncType mGetMaskListFunc;
   };
}

#endif //INT_ACTOR_PROPERTY
