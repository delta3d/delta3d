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

#include <map>
#include <dtDIS/entityidcompare.h>     // for typedef

namespace dtCore
{
   class UniqueId;
}

namespace dtDAL
{
   class ActorProxy;
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
            return( lhs.getCategory()   < rhs.getCategory()
                 || lhs.getCountry()    < rhs.getCountry()
                 || lhs.getDomain()     < rhs.getDomain()
                 || lhs.getEntityKind() < rhs.getEntityKind()
                 || lhs.getExtra()      < rhs.getExtra()
                 || lhs.getSpecific()   < rhs.getSpecific()
                 || lhs.getSubcategory()< rhs.getSubcategory() );
         }
      };
   } // end namespace details
   ///@endcond

   ///\brief provides a single point for associating known entities & actors.
   ///
   /// Provides quick look-ups given either an entity or an actor
   /// by keeping 2 maps in sync.
   struct DT_DIS_EXPORT ActiveEntityControl
   {
   public:
      /// relate an EntityID with an Actor
      /// @return 'false' when any relation previously existed; 'true' if the relation was added.
      bool AddEntity(const DIS::EntityID& eid, const dtDAL::ActorProxy* proxy);

      /// remove a relation for the EntityID and Actor
      /// @return 'false' when no relation previously existed; 'true' if the relation was removed.
      bool RemoveEntity(const DIS::EntityID& eid, const dtDAL::ActorProxy* proxy);

      /// finds the associated Entity
      /// @return NULL when the proxy has no matching EntityID
      //const DIS::EntityID* GetEntity(const dtDAL::ActorProxy* proxy);
      const DIS::EntityID* GetEntity(const dtCore::UniqueId& uid) const;

      /// finds the associated Actor
      /// @return NULL when the EntityID has no matching Actor
      const dtDAL::ActorProxy* GetActor(const DIS::EntityID& eid) const;

      /// remove all state data.
      void ClearAll();

   private:
      typedef std::map<dtCore::UniqueId,DIS::EntityID> ActorEntityMap;
      ActorEntityMap mPublishedActors;

      typedef std::map<DIS::EntityID,dtCore::RefPtr<const dtDAL::ActorProxy>,details::EntityIDCompare> EntityActorMap;
      EntityActorMap mActiveEntities;
   };

   ///\brief a structure to maintain the one-to-one
   /// relationship between the DIS::EntityID and the dtDAL::ActorType.
   struct DT_DIS_EXPORT ActorMapConfig
   {
      /// Adds a mapping from the DIS::EntityID to the dtDAL::ActorType if no mapping exists.
      /// @return true if no previous mapping existed.  false if a mapping existed already.
      bool AddActorMapping(const DIS::EntityType& eid, dtDAL::ActorType* at);

      /// Takes the one to one mapping from the container for the supplied key.
      /// @param eid the key to the mapping.
      /// @return true if eid was found in the map.  false if it was not found.
      bool RemoveActorMapping(const DIS::EntityType& eid);

      /// Introduces the ActorType mapped to the EntityID.
      /// @param toWrite Overwritten with the instance of the mapped ActorType.
      /// @param eid The key being stored.
      /// @return true if eid was found in the map.  false if it was not found.
      bool GetMappedActor(const DIS::EntityType& eid, dtDAL::ActorType*& toWrite);

   private:
      /// a convenience typedef
      typedef dtCore::RefPtr<dtDAL::ActorType> RefActorType;

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
      bool GetMappedResource(const DIS::EntityType& eid, const dtDAL::ResourceDescriptor*& toWrite) const;

   private:
      typedef std::map<DIS::EntityType,dtDAL::ResourceDescriptor,details::EntityTypeCompare> ResourceMap;

      ResourceMap mMap;
   };

   ///\brief the information needed to connect to the DIS network.
   struct ConnectionData
   {                         // example values
      unsigned int port;     // 1258
      std::string ip;        // "234.235.236.237"
      std::string plug_dir;  // "plugins"
      unsigned char exercise_id; ///< the ID for the local simulation client
      unsigned int MTU;      // 1500
   };

   ///\brief The data to be shared among plugins.
   ///\todo it would be good to have a file that maps the 'appearance' values to an asset handle,
   /// rather than full actor "types" just to change the asset.
   class DT_DIS_EXPORT SharedState
   {
   public:
      SharedState();
      ~SharedState();

      ActorMapConfig& GetActorMap();
      const ActorMapConfig& GetActorMap() const;

      ResourceMapConfig& GetResourceMap();
      const ResourceMapConfig& GetResourceMap() const;

      ActiveEntityControl& GetActiveEntityControl();
      const ActiveEntityControl& GetActiveEntityControl() const;

      void SetConnectionData(const ConnectionData& data);
      const ConnectionData& GetConnectionData() const;

   private:
      ActorMapConfig mActorMapConfig;
      ResourceMapConfig mResourceMapConfig;
      ActiveEntityControl mActiveEntityControl;
      ConnectionData mConnectionData;
   };
}

#endif // __DELTA_DTDIS_SHARED_STATE_H__
