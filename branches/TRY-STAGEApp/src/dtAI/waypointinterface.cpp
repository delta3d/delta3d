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


namespace dtAI
{
   WaypointID WaypointInterface::mIDCounter = 0;

   /////////////////////////////////////////////////////////////////////////////
   WaypointInterface::WaypointInterface()
      : mID(mIDCounter++)
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
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAI::WaypointID WaypointInterface::GetID() const
   {
      return mID;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointInterface::CreateProperties(WaypointPropertyBase& container )
   {
      static const dtUtil::RefString Property_WaypointID("WaypointID");
      static const dtUtil::RefString Property_WaypointPosition("WaypointPosition");
      static const dtUtil::RefString Desc_WaypointPosition("WaypointPosition");
      static const dtUtil::RefString Desc_WaypointID("The ID of the waypoint");
      static const dtUtil::RefString WaypointGroup("WaypointInterface");

      //////////////////////////////////////////////////////////////////////////
      typedef WaypointPropertyContainer<WaypointInterface>::Command0<unsigned, unsigned(WaypointInterface::*)() const> GetIDCommand;

      GetIDCommand* getIDCmd = new GetIDCommand(container, &WaypointInterface::GetID);
      container.AddCommand(getIDCmd);
      
      //////////////////////////////////////////////////////////////////////////
      typedef WaypointPropertyContainer<WaypointInterface>::Command1<void, void (WaypointInterface::*)(const osg::Vec3&), const osg::Vec3&> SetPosCommand;

      SetPosCommand* setPosCmd = new SetPosCommand(container, &WaypointInterface::SetPosition);
      container.AddCommand(setPosCmd);

      //////////////////////////////////////////////////////////////////////////
      typedef WaypointPropertyContainer<WaypointInterface>::Command0<const osg::Vec3&, const osg::Vec3& (WaypointInterface::*)() const> GetPosCommand;

      GetPosCommand* getPosCmd = new GetPosCommand(container, &WaypointInterface::GetPosition);
      container.AddCommand(getPosCmd);


      //////////////////////////////////////////////////////////////////////////
      dtDAL::IntActorProperty* prop = new dtDAL::IntActorProperty(Property_WaypointID, Property_WaypointID, 
                                       dtDAL::IntActorProperty::SetFuncType(),
                                       dtDAL::IntActorProperty::GetFuncType(getIDCmd, &GetIDCommand::Invoke),
                                       Desc_WaypointID, WaypointGroup);
      prop->SetReadOnly(true);
      container.AddProperty(prop);


      //////////////////////////////////////////////////////////////////////////
      container.AddProperty(new dtDAL::Vec3ActorProperty(Property_WaypointPosition, Property_WaypointPosition,
                        dtDAL::Vec3ActorProperty::SetFuncType(setPosCmd, &SetPosCommand::Invoke),
                        dtDAL::Vec3ActorProperty::GetFuncType(getPosCmd, &GetPosCommand::Invoke),
                        Desc_WaypointPosition, WaypointGroup));
   }


} // namespace dtAI
