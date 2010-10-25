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

#include <DIS/EntityID.h>              // for mapped dependency type.
#include <DIS/EntityType.h>            // for mapped dependency type.
#include <dtDAL/actortype.h>           // for mapped dependency type.
#include <dtDAL/resourcedescriptor.h>  // for mapped type
#include <dtCore/uniqueid.h>           // for mapped dependency type.
#include <dtUtil/coordinates.h>

#include <map>
#include <dtDIS/entityidcompare.h>     // for typedef
#include <dtDIS/activeentitycontrol.h>

namespace dtCore
{
   class UniqueId;
}

namespace dtDIS
{
   ///@cond DOXYGEN_SHOULD_SKIP_THIS
   namespace details
   {
      /// the model of how to compare DIS::EntityType instances.
      struct EntityTypeCompare
      {
         bool operator ()(const DIS::EntityType& lhs, const DIS::EntityType& rhs) const
         {
            if (lhs.getCategory() != rhs.getCategory())
               return lhs.getCategory() < rhs.getCategory();
            else if (lhs.getCountry() != rhs.getCountry())
               return lhs.getCountry() < rhs.getCountry();
            else if (lhs.getDomain() != rhs.getDomain())
               return lhs.getDomain() < rhs.getDomain();
            else if (lhs.getEntityKind() != rhs.getEntityKind())
               return lhs.getEntityKind() < rhs.getEntityKind();
            else if (lhs.getExtra() != rhs.getExtra())
               return lhs.getExtra() < rhs.getExtra();
            else if (lhs.getSpecific() != rhs.getSpecific())
               return lhs.getSpecific() < rhs.getSpecific();
            else if (lhs.getSubcategory() != rhs.getSubcategory())
               return lhs.getSubcategory() < rhs.getSubcategory();           
            else 
               return false;
         }
      };
   } // end namespace details
   ///@endcond


   ///\brief a structure to maintain the one-to-one
   /// relationship between the DIS::EntityID and the dtDAL::ActorType.
   struct DT_DIS_EXPORT ActorMapConfig
   {
      /// Adds a mapping from the DIS::EntityID to the dtDAL::ActorType if no mapping exists.
      /// @return true if no previous mapping existed.  false if a mapping existed already.
      bool AddActorMapping(const DIS::EntityType& eid, const dtDAL::ActorType* at);

      /// Takes the one to one mapping from the container for the supplied key.
      /// @param eid the key to the mapping.
      /// @return true if eid was found in the map.  false if it was not found.
      bool RemoveActorMapping(const DIS::EntityType& eid);

      /// Introduces the ActorType mapped to the EntityID.
      /// @param toWrite Overwritten with the instance of the mapped ActorType.
      /// @param eid The key being stored.
      /// @return true if eid was found in the map.  false if it was not found.
      bool GetMappedActor(const DIS::EntityType& eid, const dtDAL::ActorType*& toWrite);

   private:
      /// a convenience typedef
      typedef dtCore::RefPtr<const dtDAL::ActorType> RefActorType;

      /// a convenience typedef
      typedef std::map<DIS::EntityType,RefActorType,details::EntityTypeCompare> ActorMap;

      /// The controlled data.
      ActorMap mMap;
   };

   ///\brief a structure to maintain the relationship between the DIS::EntityID and the resources available.
   struct DT_DIS_EXPORT ResourceMapConfig
   {
   public:
      /// Adds a mapping from the DIS::EntityID to the resource identifier if no mapping exists.
      /// @return true if no previous mapping existed.  false if a mapping existed already.
      bool AddResourceMapping(const DIS::EntityType& eid, const dtDAL::ResourceDescriptor& resource);

      /// Takes the one to one mapping from the container for the supplied key.
      /// @param eid the key to the mapping.
      /// @return true if eid was found and removed from the map.  false if it was not removed.
      bool RemoveResourceMapping(const DIS::EntityType& eid);

      /// Introduces the ActorType mapped to the EntityID.
      /// @param toWrite Overwritten with the instance of the mapped resource identifier.
      /// @param eid The key being stored.
      /// @return true if eid was found in the map.  false if it was not found.
      bool GetMappedResource(const DIS::EntityType& eid, dtDAL::ResourceDescriptor& toWrite) const;

   private:
      typedef std::map<DIS::EntityType,dtDAL::ResourceDescriptor,details::EntityTypeCompare> ResourceMap;

      ResourceMap mMap;
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
         unsigned int mtu = 1500
         )
         :port(port)
         ,ip(url)
         , plug_dir(plugins)
         , exercise_id(ex_id)
         , site_id(site)
         , application_id(app_id)
         , MTU(mtu)
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
   };

   ///\brief The data to be shared among plugins.
   ///\todo it would be good to have a file that maps the 'appearance' values to an asset handle,
   /// rather than full actor "types" just to change the asset.
   class DT_DIS_EXPORT SharedState
   {
   public:
      SharedState(const std::string& connectionXMLFile = "",
                  const std::string& entityMappingXMLFile = "");
      ~SharedState();

      ActorMapConfig& GetActorMap();
      const ActorMapConfig& GetActorMap() const;

      ResourceMapConfig& GetResourceMap();
      const ResourceMapConfig& GetResourceMap() const;

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

   private:
      ActorMapConfig mActorMapConfig;
      ResourceMapConfig mResourceMapConfig;
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
