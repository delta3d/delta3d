/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2009 Alion Science and Technology
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
* Bradley Anderegg
*/


#include <dtAI/aiplugininterface.h>


namespace dtAI
{
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   AIPluginInterface::AIPluginInterface()
   {
      mFactory = new WaypointFactory();
      mPropertyCache = new WaypointPropertyCache();
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   AIPluginInterface::~AIPluginInterface()
   {

   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   WaypointInterface* AIPluginInterface::CreateWaypoint(const dtDAL::ObjectType& type) const
   {
      return mFactory->CreateObject(&type);
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   bool AIPluginInterface::IsWaypointTypeSupported(dtCore::RefPtr<const dtDAL::ObjectType> type) const
   {
      return mFactory->IsTypeSupported(type);
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void AIPluginInterface::GetSupportedWaypointTypes(std::vector<dtCore::RefPtr<const dtDAL::ObjectType> >& actors) const
   {
      mFactory->GetSupportedTypes(actors);
   }   
}
