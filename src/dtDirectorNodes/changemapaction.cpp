/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Eric R. Heine
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/changemapaction.h>

#include <dtCore/stringactorproperty.h>
#include <dtDirector/director.h>
#include <dtGame/gamemanager.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   ChangeMapAction::ChangeMapAction()
      : ActionNode()
   {
      AddAuthor("Eric R. Heine");
   }

   ////////////////////////////////////////////////////////////////////////////////
   ChangeMapAction::~ChangeMapAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ChangeMapAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::StringActorProperty* mapProp = new dtCore::StringActorProperty(
         "Map", "Map(s)",
         dtCore::StringActorProperty::SetFuncType(this, &ChangeMapAction::SetMap),
         dtCore::StringActorProperty::GetFuncType(this, &ChangeMapAction::GetMap),
         "The map(s) to change to.");
      AddProperty(mapProp);
      mValues.push_back(ValueLink(this, mapProp, false, true));
   }

   //////////////////////////////////////////////////////////////////////////
   bool ChangeMapAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      // Get list of map names to change to
      dtGame::GameManager::NameVector mapNames;
      int count = GetPropertyCount("Map");
      for (int index = 0; index < count; index++)
      {
         std::string map = GetString("Map", index);
         if (!map.empty())
         {
            mapNames.push_back(map);
         }
      }

      GetDirector()->GetGameManager()->ChangeMapSet(mapNames);

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   //////////////////////////////////////////////////////////////////////////
   void ChangeMapAction::SetMap(const std::string& value)
   {
      mMap = value;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& ChangeMapAction::GetMap()
   {
      return mMap;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& ChangeMapAction::GetName()
   {
      return mMap;
   }
}

////////////////////////////////////////////////////////////////////////////////
