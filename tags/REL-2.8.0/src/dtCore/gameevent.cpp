/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 */
#include <prefix/dtcoreprefix.h>
#include <dtCore/gameevent.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   GameEvent::GameEvent()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameEvent::GameEvent(const std::string& name, const dtUtil::RefString& desc) : dtCore::Base(name),
      mDescription(desc)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool GameEvent::operator==(const GameEvent& toCompare) const
   {
      return GetUniqueId() == toCompare.GetUniqueId();
   }

   ///////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(GameEvent, dtUtil::RefString, Description);

}
