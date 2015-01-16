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
 */

#ifndef DELTA_NAMED_GAME_EVENT_PARAMETER
#define DELTA_NAMED_GAME_EVENT_PARAMETER

#include <dtCore/namedgenericparameter.h>

#include <dtCore/uniqueid.h>

namespace dtCore
{
   /**
    * @class GameEventMessageParameter
    * The GameEventMessageParameter stores a unique id that can be used to get the associated
    * game event from the EventManager.
    * @see #dtCore::GameEventManager
    */
   class DT_CORE_EXPORT NamedGameEventParameter : public NamedGenericParameter<dtCore::UniqueId>
   {
      public:
         NamedGameEventParameter(const dtUtil::RefString& name,
                                   const dtCore::UniqueId& defaultValue = dtCore::UniqueId(""),
                                   bool isList = false);
         virtual const std::string ToString() const;
         virtual bool FromString(const std::string& value);

         virtual void SetFromProperty(const dtCore::ActorProperty& property);
         virtual void ApplyValueToProperty(dtCore::ActorProperty& property) const;

      protected:
         virtual ~NamedGameEventParameter();
   };
}

#endif //DELTA_NAMED_GAME_EVENT_PARAMETER
