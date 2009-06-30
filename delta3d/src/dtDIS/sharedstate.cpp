#include <dtDIS/sharedstate.h>
#include <dtDAL/actorproxy.h>

#include <cstddef>                   // for NULL

using namespace dtDIS;



bool ActorMapConfig::AddActorMapping(const DIS::EntityType& eid, const dtDAL::ActorType* at)
{
   return( mMap.insert( ActorMap::value_type(eid,at) ).second );
}

bool ActorMapConfig::RemoveActorMapping(const DIS::EntityType& eid)
{
   return( mMap.erase( eid )>0 );
}

bool ActorMapConfig::GetMappedActor(const DIS::EntityType& eid, const dtDAL::ActorType*& toWrite)
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
   //TODO Should read and process a DIS xml configuration file
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

