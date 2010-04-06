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
   WaypointInterface* AIPluginInterface::CreateWaypoint(const osg::Vec3& pos, const dtDAL::ObjectType& type)
   {
      WaypointInterface* newWaypoint = CreateNoInsert(type);
      newWaypoint->SetPosition(pos);
      InsertWaypoint(newWaypoint);
      return newWaypoint;
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   WaypointInterface* AIPluginInterface::CreateNoInsert(const dtDAL::ObjectType& type)
   {
      return mFactory->CreateObject(&type);
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   WaypointInterface* AIPluginInterface::CreateWaypointNoDuplicates(const osg::Vec3& pos, float radius, const dtDAL::ObjectType& type)
   {
      WaypointInterface* result = GetClosestWaypoint(pos, radius);

      if(result == NULL)
      {         
         result = CreateWaypoint(pos, type);
      }      

      return result;
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

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   const dtDAL::ObjectType* AIPluginInterface::GetWaypointTypeByName( const std::string& name ) const
   {
      typedef std::vector<dtCore::RefPtr<const dtDAL::ObjectType> > ObjectTypeArray;

      ObjectTypeArray waypointTypes;
      GetSupportedWaypointTypes(waypointTypes);

      ObjectTypeArray::iterator ob_iter = waypointTypes.begin();
      ObjectTypeArray::iterator ob_iterEnd = waypointTypes.end();

      for(;ob_iter != ob_iterEnd; ++ob_iter)
      {
         if((**ob_iter).GetName() == name)
         {
            return (*ob_iter).get();
         }
      }

      return NULL;
   }
}
