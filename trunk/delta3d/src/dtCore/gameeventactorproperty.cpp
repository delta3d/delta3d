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
#include <dtCore/gameeventactorproperty.h>

#include <dtCore/datatype.h>
#include <dtCore/map.h>
#include <dtCore/project.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////////
   GameEventActorProperty::GameEventActorProperty(BaseActorObject& actorProxy,
      const dtUtil::RefString& name, const dtUtil::RefString& label,
      SetFuncType set, GetFuncType get, const dtUtil::RefString& desc /*= ""*/,
      const dtUtil::RefString& groupName /*= ""*/)
      : BaseClass(DataType::GAME_EVENT, name, label, set, get, desc, groupName)
      , mProxy(&actorProxy)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   GameEventActorProperty::GameEventActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      const dtUtil::RefString& desc /*= ""*/, const dtUtil::RefString& groupName /*= ""*/)
      : BaseClass(DataType::GAME_EVENT, name, label, set, get, desc, groupName)
      , mProxy(NULL)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   bool GameEventActorProperty::FromString(const std::string& value)
   {
      dtCore::UniqueId id = dtCore::UniqueId(value);
      GameEvent *event = NULL;

      if (mProxy && Project::GetInstance().IsContextValid())
      {
         Map* map = Project::GetInstance().GetMapForActorProxy(*mProxy);
         if (map)
         {
            event = map->GetEventManager().FindEvent(id);
         }
      }
      else
      {
         event = Project::GetInstance().GetGameEvent(id);
      }

      SetValue(event);
      return (event != NULL) ? true : false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string GameEventActorProperty::ToString() const
   {
      return GetValue() == NULL ? "" : GetValue()->GetUniqueId().ToString();
   }

   //////////////////////////////////////////////////////////////////////////
   std::string GameEventActorProperty::GetValueString() const
   {
      GameEvent* gameEvent = GetValue();
      if (gameEvent)
      {
         return gameEvent->GetName();
      }

      return "None";
   }

   //////////////////////////////////////////////////////////////////////////
}

