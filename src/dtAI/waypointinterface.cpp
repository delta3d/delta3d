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
 * Bradley Anderegg
 */

#include <dtAI/waypointinterface.h>
#include <dtAI/waypointpropertycontainer.h>
#include <dtDAL/propertycontainer.h>
#include <dtDAL/enginepropertytypes.h>

#include <dtUtil/templateutility.h>
#include <sstream>

namespace dtAI
{
   WaypointID WaypointInterface::mIDCounter = 0;

   /////////////////////////////////////////////////////////////////////////////
   WaypointInterface::WaypointInterface(const dtDAL::ObjectType* wpt)
      : mID(mIDCounter++)
      , mWaypointType(wpt)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointInterface::~WaypointInterface()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointInterface::operator==(const WaypointInterface& pWay) const
   {
      return mID == pWay.GetID();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointInterface::operator!=(const WaypointInterface& pWay) const
   {
      return mID != pWay.GetID();
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointInterface::SetID(WaypointID pID)
   {
      mID = pID;

      //increment counter if necessary
      if(mIDCounter <= mID) mIDCounter = mID + 1;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAI::WaypointID WaypointInterface::GetID() const
   {
      return mID;
   }


   /////////////////////////////////////////////////////////////////////////////
   const dtDAL::ObjectType& WaypointInterface::GetWaypointType() const
   {
      return *mWaypointType;
   }
   
   //////////////////////////////////////////////////////////////////////////
   osg::Vec3 WaypointInterface::GetPosCopy() const
   {
      return GetPosition();
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointInterface::CreateProperties(WaypointPropertyBase& container )
   {
      static const dtUtil::RefString Property_WaypointID("WaypointID");
      static const dtUtil::RefString Property_WaypointPosition("WaypointPosition");
      static const dtUtil::RefString Desc_WaypointPosition("WaypointPosition");
      static const dtUtil::RefString Desc_WaypointID("The ID of the waypoint");
      static const dtUtil::RefString WaypointGroup("WaypointInterface");

      container.CreateProperty<osg::Vec3>(Property_WaypointPosition, Property_WaypointPosition, 
                  &WaypointInterface::GetPosCopy, &WaypointInterface::SetPosition, Desc_WaypointPosition, WaypointGroup);

      //we actually do not let you set the ID through the property, because we cannot guarantee it will never change.
      //This boils down to a read/write issue.
      container.CreateReadOnlyProperty<int>(Property_WaypointID, Property_WaypointID, 
                                                  &WaypointInterface::GetIDAsInt, Desc_WaypointID, WaypointGroup);

   }

   //////////////////////////////////////////////////////////////////////////
   std::string WaypointInterface::ToString() const
   {
      osg::Vec3 pos = GetPosition();
      std::ostringstream ss;
      ss << "ID: " << mID << ", Pos: (" << pos[0] << ", " << pos[1] << ", " << pos[2] << ")" << std::endl;

      return ss.str();
   }

   //////////////////////////////////////////////////////////////////////////
   int WaypointInterface::GetIDAsInt() const
   {
      //todo: we need an unsigned int actor property
      return GetID();
   }

} // namespace dtAI
