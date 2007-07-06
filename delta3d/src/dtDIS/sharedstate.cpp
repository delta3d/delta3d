#include <dtDIS/sharedstate.h>
#include <dtDAL/actorproxy.h>

#include <cstddef>                   // for NULL

using namespace dtDIS;

bool ActiveEntityControl::AddEntity(const DIS::EntityID& eid, const dtDAL::ActorProxy* proxy)
{
   //bool published = mPublishedActors.insert( ActorEntityMap::value_type(proxy,eid).second;
   bool published = mPublishedActors.insert( ActorEntityMap::value_type(proxy->GetId(),eid) ).second;
   if( published )
   {
      EntityActorMap::value_type vp(eid,proxy);
      bool activated = mActiveEntities.insert( vp ).second;
      if( !activated )
      {
         // keep it in sync with the other container
         mPublishedActors.erase( proxy->GetId() );
      }

      return activated;
   }

   return false;
}

bool ActiveEntityControl::RemoveEntity(const DIS::EntityID& eid, const dtDAL::ActorProxy* proxy)
{
   bool pub = mPublishedActors.erase(proxy->GetId()) > 0;
   bool act = mActiveEntities.erase(eid) > 0;
   return( pub && act );
}

const DIS::EntityID* ActiveEntityControl::GetEntity(const dtCore::UniqueId& uid) const
{
   ActorEntityMap::const_iterator iter = mPublishedActors.find( uid );
   if( iter != mPublishedActors.end() )
   {
      return( &(iter->second) );
   }

   return NULL;
}

const dtDAL::ActorProxy* ActiveEntityControl::GetActor(const DIS::EntityID& eid) const
{
   EntityActorMap::const_iterator iter = mActiveEntities.find( eid );
   if( iter != mActiveEntities.end() )
   {
      return( (iter->second).get() );
   }

   return NULL;
}

void ActiveEntityControl::ClearAll()
{
   mActiveEntities.clear();
   mPublishedActors.clear();
}


bool ActorMapConfig::AddActorMapping(const DIS::EntityType& eid, dtDAL::ActorType* at)
{
   return( mMap.insert( ActorMap::value_type(eid,at) ).second );
}

bool ActorMapConfig::RemoveActorMapping(const DIS::EntityType& eid)
{
   return( mMap.erase( eid )>0 );
}

bool ActorMapConfig::GetMappedActor(const DIS::EntityType& eid, dtDAL::ActorType*& toWrite)
{
   ActorMap::iterator iter = mMap.find( eid );
   if( iter != mMap.end() )
   {
      toWrite = iter->second.get();
      return true;
   }

   //toWrite = NULL;
   return false;
}


bool ResourceMapConfig::AddResourceMapping(const DIS::EntityType& eid, const dtDAL::ResourceDescriptor& resource)
{
   return( mMap.insert( ResourceMap::value_type(eid,resource) ).second );
}

bool ResourceMapConfig::RemoveResourceMapping(const DIS::EntityType& eid)
{
   return( mMap.erase( eid )>0 );
}

bool ResourceMapConfig::GetMappedResource(const DIS::EntityType& eid, const dtDAL::ResourceDescriptor*& toWrite) const
{
   ResourceMap::const_iterator iter = mMap.find( eid );
   if( iter != mMap.end() )
   {
      toWrite = &(iter->second);
      return true;
   }

   //toWrite = NULL;
   return false;
}



SharedState::SharedState()
   : mActorMapConfig()
   , mResourceMapConfig()
   , mActiveEntityControl()
   , mConnectionData()
{
}

SharedState::~SharedState()
{
}

ActorMapConfig& SharedState::GetActorMap()
{
   return mActorMapConfig;
}

const ActorMapConfig& SharedState::GetActorMap() const
{
   return mActorMapConfig;
}

ResourceMapConfig& SharedState::GetResourceMap()
{
   return mResourceMapConfig;
}

const ResourceMapConfig& SharedState::GetResourceMap() const
{
   return mResourceMapConfig;
}

ActiveEntityControl& SharedState::GetActiveEntityControl()
{
   return mActiveEntityControl;
}

const ActiveEntityControl& SharedState::GetActiveEntityControl() const
{
   return mActiveEntityControl;
}

void SharedState::SetConnectionData(const ConnectionData& data)
{
   mConnectionData = data;
}

const ConnectionData& SharedState::GetConnectionData() const
{
   return mConnectionData;
}

