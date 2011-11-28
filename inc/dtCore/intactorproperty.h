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

#ifndef INT_ACTOR_PROPERTY
#define INT_ACTOR_PROPERTY

#include <dtCore/export.h>
#include <dtCore/genericactorproperty.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a signed integer data member.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT IntActorProperty : public GenericActorProperty<int, int>
   {
      public:
         typedef GenericActorProperty<int, int> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         IntActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                          SetFuncType set, GetFuncType get,
                          const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "");

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
         virtual ~IntActorProperty() { }
   };
}

#endif //INT_ACTOR_PROPERTY
