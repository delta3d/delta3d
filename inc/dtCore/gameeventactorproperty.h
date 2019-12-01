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

#ifndef GAME_EVENT_ACTOR_PROPERTY
#define GAME_EVENT_ACTOR_PROPERTY

#include <dtCore/actorproxy.h>
#include <dtCore/export.h>
#include <dtCore/gameevent.h>
#include <dtCore/genericactorproperty.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a game event property.
    *
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT GameEventActorProperty
      : public GenericActorProperty<GameEvent*, GameEvent*>
   {
      public:
         typedef GenericActorProperty<GameEvent*, GameEvent*> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         GameEventActorProperty(BaseActorObject& actorProxy,
                                const dtUtil::RefString& name, const dtUtil::RefString& label,
                                SetFuncType set, GetFuncType get,
                                const dtUtil::RefString& desc = "",
                                const dtUtil::RefString& groupName = "");

         GameEventActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                                SetFuncType set, GetFuncType get,
                                const dtUtil::RefString& desc = "",
                                const dtUtil::RefString& groupName = "");

         /**
          * Sets the value of this property using the given string.
          * @param value The string representing the game event.  This string contains
          *   unique id of the game event.  This is used to look up the actual event
          *   which then gets referenced by this property.
          * @return True if the value could be parsed and the specified event was found, false otherwise.
          */
         virtual bool FromString(const std::string& value);

         /**
          * Gets a string version of the game event data.
          * @return A string containing the unique id of the game event.
          * @see #FromString
         */
         virtual const std::string ToString() const;

         /**
          * Retrieves a human readable version of the property's value.
          */
         virtual std::string GetValueString() const;

      protected:
         virtual ~GameEventActorProperty() { }

      private:
         BaseActorObject* mProxy;
   };
}

#endif //GAME_EVENT_ACTOR_PROPERTY
