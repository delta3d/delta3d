/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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
 * Bradley Anderegg 03/16/2006
 */

#include <dtAI/npcevent.h>

namespace dtAI
{
   
   IMPLEMENT_ENUM(NPCEvent);

   NPCEvent::NPCEvent(const std::string& name) : dtUtil::Enumeration(name)
   {
      
   }

   NPCEvent::~NPCEvent() {}

   const NPCEvent NPCEvent::NPC_EVENT_SPAWN("NPC_EVENT_SPAWN");
   const NPCEvent NPCEvent::NPC_EVENT_DIE("NPC_EVENT_DIE");
   const NPCEvent NPCEvent::NPC_EVENT_SLEEP("NPC_EVENT_SLEEP");


}//namespace dtAI
