/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_SHARED_STATE_H__
#define __DELTA_DTDIS_SHARED_STATE_H__

#include <dtDIS/dtdisexport.h>         // for export symbols

#include <DIS/EntityType.h>            // for mapped dependency type.
#include <dtCore/actortype.h>           // for mapped dependency type.
#include <dtCore/resourcedescriptor.h>  // for mapped type
#include <dtCore/uniqueid.h>           // for mapped dependency type.
#include <dtUtil/coordinates.h>
#include <dtUtil/deprecationmgr.h>

#include <map>
#include <dtDIS/entityidcompare.h>     // for typedef
#include <dtDIS/activeentitycontrol.h>
#include <dtDIS/entitymap.h>
#include <dtUtil/mswinmacros.h>

#if defined (DELTA_WIN32)
   #pragma warning(default:4996) //ensure the deprecated compile warning isn't disabled
#endif

namespace dtCore
{
   class UniqueId;
}

namespace dtDIS
{

   ///Deprecated 3/17/2011  Use dtDIS::EntityMap instead
   struct DT_DIS_EXPORT ActorMapConfig
   {
      ActorMapConfig(EntityMap* entityMap)
         :mEntityMap(entityMap)
      {         
      }

      /// Adds a mapping from the DIS::EntityID to the dtCore::ActorType if no mapping exists.
      /// Deprecated 7/29/2011
      /// @return true if no previous mapping existed.  false if a mapping existed already.
      bool DEPRECATE_FUNC AddActorMapping(const DIS::EntityType& eid, const dtCore::ActorType* at);

      /// Takes the one to one mapping from the container for the supplied key.
      /// Deprecated 3/17/2011
      /// @param eid the key to the mapping.
      /// @return true if eid was found in the map.  false if it was not found.
      bool DEPRECATE_FUNC RemoveActorMapping(const DIS::EntityType& eid);

      /// Introduces the ActorType mapped to the EntityID.
      /// Deprecated 7/29/2011
      /// @param toWrite Overwritten with the instance of the mapped ActorType.
      /// @param eid The key being stored.
      /// @return true if eid was found in the map.  false if it was not found.
      bool DEPRECATE_FUNC GetMappedActor(const DIS::EntityType& eid, const dtCore::ActorType*& toWrite);

   private:
      EntityMap* mEntityMap;
   };

   ///Deprecated 3/17/2011  Use dtDIS::EntityMap instead
   struct DT_DIS_EXPORT ResourceMapConfig
   {
   public:
      ResourceMapConfig(EntityMap* entityMap):
         mEntityMap(entityMap)
      {       
      }

      /// Adds a mapping from the DIS::EntityID to the resource identifier if no mapping exists.
         /// Deprecated 7/29/2011
      /// @return true if no previous mapping existed.  false if a mapping existed already.
      bool DEPRECATE_FUNC AddResourceMapping(const DIS::EntityType& eid, const dtCore::ResourceDescriptor& resource);

      /// Takes the one to one mapping from the container for the supplied key.
      /// Deprecated 3/17/2011
      /// @param eid the key to the mapping.
      /// @return true if eid was found and removed from the map.  false if it was not removed.
      bool DEPRECATE_FUNC RemoveResourceMapping(const DIS::EntityType& eid);

      /// Introduces the ActorType mapped to the EntityID.
      /// Deprecated 7/29/2011
      /// @param toWrite Overwritten with the instance of the mapped resource identifier.
      /// @param eid The key being stored.
      /// @return true if eid was found in the map.  false if it was not found.
      bool DEPRECATE_FUNC GetMappedResource(const DIS::EntityType& eid, dtCore::ResourceDescriptor& toWrite) const;

   private:
      EntityMap* mEntityMap;

   };

   ///\brief the information needed to connect to the DIS network.
   struct ConnectionData
   {
      ConnectionData(unsigned int p = 62040,
         const std::string& url = "239.1.2.3",
         const std::string& plugins = "../../dtDIS_trunk/bin/plugins",
         unsigned char ex_id = 1,
         unsigned short site = 1,
         unsigned short app_id = 1,
         unsigned int mtu = 1500,
         bool broadcast = false
         )
         :port(port)
         ,ip(url)
         , plug_dir(plugins)
         , exercise_id(ex_id)
         , site_id(site)
         , application_id(app_id)
         , MTU(mtu)
         , use_broadcast(broadcast)
      {
      }

      // example values
      unsigned int port;     // 1258
      std::string ip;        // "234.235.236.237"
      std::string plug_dir;  // "plugins"
      unsigned char exercise_id; ///< the ID for the local simulation client
      unsigned short site_id; ///<The ID of the sending site
      unsigned short application_id; ///<the ID of the sending application
      unsigned int MTU;      // 1500
      bool use_broadcast;    ///< Connect to a broadcast network (if false, connect via multicast)
   };

   ///\brief The data to be shared among plugins.
   class DT_DIS_EXPORT SharedState
   {
   public:
      SharedState(const std::string& connectionXMLFile = "",
                  const std::string& entityMappingXMLFile = "");
      ~SharedState();

      EntityMap& GetEntityMap();
      const EntityMap& GetEntityMap() const;

      ActiveEntityControl& GetActiveEntityControl();
      const ActiveEntityControl& GetActiveEntityControl() const;

      void SetConnectionData(const ConnectionData& data);
      const ConnectionData& GetConnectionData() const;

      void SetSiteID(unsigned short ID);
      unsigned short GetSiteID() const;

      void SetApplicationID(unsigned short ID);
      unsigned short GetApplicationID() const; 

      void SetCoordinateConverter(const dtUtil::Coordinates& coordConverter);
      const dtUtil::Coordinates& GetCoordinateConverter() const;
      dtUtil::Coordinates& GetCoordinateConverter();

      ///Deprecated 3/17/2011
      DEPRECATE_FUNC ActorMapConfig& GetActorMap();
      ///Deprecated 3/17/2011
      DEPRECATE_FUNC const ActorMapConfig& GetActorMap() const;
      ///Deprecated 3/17/2011
      DEPRECATE_FUNC ResourceMapConfig& GetResourceMap();
      ///Deprecated 3/17/2011
      DEPRECATE_FUNC const ResourceMapConfig& GetResourceMap() const;

   private:
      ActorMapConfig mActorMapConfig; ///>Deprecated
      ResourceMapConfig mResourceMapConfig; ///>Deprecated
      EntityMap mEntityTypeMap;
      ActiveEntityControl mActiveEntityControl;
      ConnectionData mConnectionData;
      unsigned short mSiteID;       ///<For outgoing DIS packets
      unsigned short mApplicationID;///<For outgoing DIS packets
      dtUtil::Coordinates mCoordConverter;

      void ParseConnectionData(const std::string& file);
      void ParseEntityMappingData(const std::string& file);
   };
}

#endif // __DELTA_DTDIS_SHARED_STATE_H__
