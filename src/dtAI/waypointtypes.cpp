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

#include <dtAI/waypointtypes.h>
#include <dtAI/waypointpropertycontainer.h>
#include <dtDAL/propertycontainer.h>
#include <dtDAL/enginepropertytypes.h>

#include <dtUtil/templateutility.h>
#include <sstream>

namespace dtAI
{
   /////////////////////////////////////////////////////////////////////////////
   // WaypointTypes
   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::ObjectType> WaypointTypes::DEFAULT_WAYPOINT(new dtDAL::ObjectType("DefaultWaypoint", "EngineWaypointTypes"));
   dtCore::RefPtr<dtDAL::ObjectType> WaypointTypes::NAMED_WAYPOINT(new dtDAL::ObjectType("NamedWaypoint", "EngineWaypointTypes"));
   dtCore::RefPtr<dtDAL::ObjectType> WaypointTypes::TACTICAL_WAYPOINT(new dtDAL::ObjectType("TacticalWaypoint", "EngineWaypointTypes", "", WaypointTypes::NAMED_WAYPOINT.get()));
   dtCore::RefPtr<dtDAL::ObjectType> WaypointTypes::WAYPOINT_COLLECTION(new dtDAL::ObjectType("WaypointCollection", "EngineWaypointTypes"))  ;

   /////////////////////////////////////////////////////////////////////////////
   //NamedWaypoint
   /////////////////////////////////////////////////////////////////////////////
   NamedWaypoint::NamedWaypoint()
      : BaseClass(WaypointTypes::NAMED_WAYPOINT.get())
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   NamedWaypoint::NamedWaypoint( const osg::Vec3& pos )
      : BaseClass(WaypointTypes::NAMED_WAYPOINT.get())
      , mPosition(pos)
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   NamedWaypoint::NamedWaypoint( const dtUtil::RefString& name )
      : BaseClass(WaypointTypes::NAMED_WAYPOINT.get())
      , mName(name)
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   NamedWaypoint::NamedWaypoint( const osg::Vec3& pos, const dtUtil::RefString& name )
      : BaseClass(WaypointTypes::NAMED_WAYPOINT.get())
      , mName(name)
      , mPosition(pos)
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   NamedWaypoint::NamedWaypoint( const dtDAL::ObjectType* ot )
      : BaseClass(ot)
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   NamedWaypoint::NamedWaypoint( const dtDAL::ObjectType* ot, const osg::Vec3& pos )
      : BaseClass(ot)
      , mPosition(pos)
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   NamedWaypoint::NamedWaypoint( const dtDAL::ObjectType* ot, const dtUtil::RefString& name )
     : BaseClass(ot)
     , mName(name)
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   NamedWaypoint::NamedWaypoint( const dtDAL::ObjectType* ot, const osg::Vec3& pos, const dtUtil::RefString& name )
      : BaseClass(ot)
      , mName(name)
      , mPosition(pos)
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   NamedWaypoint::~NamedWaypoint()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   std::string NamedWaypoint::ToString() const
   {
      return mName.Get();
   }

   //////////////////////////////////////////////////////////////////////////
   void NamedWaypoint::SetName( const std::string& name )
   {
      mName = name;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& NamedWaypoint::GetName() const
   {
      return mName.Get();
   }

   //////////////////////////////////////////////////////////////////////////
   std::string NamedWaypoint::GetNameCopy() const
   {
      return mName.Get();
   }

   //////////////////////////////////////////////////////////////////////////
   const osg::Vec3& NamedWaypoint::GetPosition() const
   {
      return mPosition;
   }

   //////////////////////////////////////////////////////////////////////////
   void NamedWaypoint::SetPosition( const osg::Vec3& pVec )
   {
      mPosition = pVec;
   }

   /////////////////////////////////////////////////////////////////////////////
   void NamedWaypoint::CreateProperties(WaypointPropertyBase& container )
   {
      BaseClass::CreateProperties(container);

      static const dtUtil::RefString Property_WaypointName("WaypointName");
      static const dtUtil::RefString Desc_WaypointName("Name of Waypoint");
      static const dtUtil::RefString WaypointGroup("NamedWaypoint");

      container.CreateProperty<std::string>(Property_WaypointName, Property_WaypointName, 
         &NamedWaypoint::GetNameCopy, &NamedWaypoint::SetName, Desc_WaypointName, WaypointGroup);
   }


   //////////////////////////////////////////////////////////////////////////
   //Tactical Waypoint
   //////////////////////////////////////////////////////////////////////////
   TacticalWaypoint::TacticalWaypoint()
      : BaseClass(WaypointTypes::TACTICAL_WAYPOINT.get())
   {

   }

   //////////////////////////////////////////////////////////////////////////
   TacticalWaypoint::TacticalWaypoint( const osg::Vec3& pos )
      : BaseClass(WaypointTypes::TACTICAL_WAYPOINT.get(), pos)
   {

   }

   //////////////////////////////////////////////////////////////////////////
   TacticalWaypoint::TacticalWaypoint( const dtUtil::RefString& name )
      : BaseClass(WaypointTypes::TACTICAL_WAYPOINT.get(), name)
   {

   }

   //////////////////////////////////////////////////////////////////////////
   TacticalWaypoint::TacticalWaypoint( const osg::Vec3& pos, const dtUtil::RefString& name )
      : BaseClass(WaypointTypes::TACTICAL_WAYPOINT.get(), pos, name)
   {
      
   }

   //////////////////////////////////////////////////////////////////////////
   TacticalWaypoint::~TacticalWaypoint()
   {

   }

   //////////////////////////////////////////////////////////////////////////
   void TacticalWaypoint::SetCover( bool b )
   {
      mCover = b;
   }

   //////////////////////////////////////////////////////////////////////////
   bool TacticalWaypoint::GetCover() const
   {
      return mCover;
   }

   //////////////////////////////////////////////////////////////////////////
   void TacticalWaypoint::SetCoverAngle( const osg::Vec3& angle )
   {
      mCoverAngle = angle;
   }

   //////////////////////////////////////////////////////////////////////////
   const osg::Vec3& TacticalWaypoint::GetCoverAngle() const
   {
      return mCoverAngle;
   }
   
   //////////////////////////////////////////////////////////////////////////
   osg::Vec3 TacticalWaypoint::GetCoverAngleCopy() const
   {
      return mCoverAngle;
   }

   //////////////////////////////////////////////////////////////////////////
   void TacticalWaypoint::SetSniper( bool b )
   {
      mSniper = b;
   }

   //////////////////////////////////////////////////////////////////////////
   bool TacticalWaypoint::GetSniper() const
   {
      return mSniper;
   }

   //////////////////////////////////////////////////////////////////////////
   void TacticalWaypoint::SetSniperAngle( const osg::Vec3& angle )
   {
      mSniperAngle = angle;
   }

   //////////////////////////////////////////////////////////////////////////
   const osg::Vec3& TacticalWaypoint::GetSniperAngle() const
   {
      return mSniperAngle;
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec3 TacticalWaypoint::GetSniperAngleCopy() const
   {
      return mSniperAngle;
   }

   //////////////////////////////////////////////////////////////////////////
   void TacticalWaypoint::CreateProperties( WaypointPropertyBase& container )
   {
      BaseClass::CreateProperties(container);

      static const dtUtil::RefString Property_SniperPoint("SniperPoint");
      static const dtUtil::RefString Property_SniperAngle("SniperAngle");

      static const dtUtil::RefString Property_CoverPoint("CoverPoint");
      static const dtUtil::RefString Property_CoverAngle("CoverAngle");

      static const dtUtil::RefString WaypointGroup("TacticalWaypoint");

      container.CreateProperty<bool>(Property_SniperPoint, Property_SniperPoint, 
         &TacticalWaypoint::GetSniper, &TacticalWaypoint::SetSniper, Property_SniperPoint, WaypointGroup);

      container.CreateProperty<osg::Vec3>(Property_SniperAngle, Property_SniperAngle, 
         &TacticalWaypoint::GetSniperAngleCopy, &TacticalWaypoint::SetSniperAngle, Property_SniperAngle, WaypointGroup);

      container.CreateProperty<bool>(Property_CoverPoint, Property_CoverPoint, 
         &TacticalWaypoint::GetCover, &TacticalWaypoint::SetCover, Property_CoverPoint, WaypointGroup);

      container.CreateProperty<osg::Vec3>(Property_CoverAngle, Property_CoverAngle, 
         &TacticalWaypoint::GetCoverAngleCopy, &TacticalWaypoint::SetCoverAngle, Property_CoverAngle, WaypointGroup);
   }

} // namespace dtAI
