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
 * Matthew W. Campbell
 * David Guthrie
 * William E. Johnson II
 * Eric Heine
 */

#ifndef STRING_ACTOR_PROPERTY
#define STRING_ACTOR_PROPERTY

#include <dtCore/export.h>
#include <dtCore/genericactorproperty.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a string data member.  Where as the
    * property has a maximum string length, by default, the string has no
    * maximum length.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT StringActorProperty : public GenericActorProperty<const std::string&,
                                                                         std::string>
   {
      public:
         typedef GenericActorProperty<const std::string&, std::string> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         StringActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "",
                           const dtUtil::RefString& groupName = "");

         /**
          * Sets the maximum length of strings stored in this property.
          * @note Strings longer than this value are truncated.
          * @param length Max length.
          */
         void SetMaxLength(unsigned int length);

         /**
          * Gets the maximum length of strings stored in this property.
          * @return The maximum length of strings stored in this property.
          */
         unsigned int GetMaxLength() const;

         /**
          * Overloaded set value method so that the new string value can be
          * truncated if it is longer than the maximum length allowed.
          * @param str The new string value to set on this property.
          */
         void SetValue(const std::string& str);

         /**
          * Does the same thing as SetValue.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);

         /**
          * @return the same as GetValue.
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~StringActorProperty() { }

         ///Maximum length the string for this string property can be.
         unsigned int mMaxLength;
      };
}

#endif //STRING_ACTOR_PROPERTY
