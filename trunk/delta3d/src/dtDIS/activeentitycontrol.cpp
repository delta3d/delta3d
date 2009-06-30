#include <dtDIS/activeentitycontrol.h>
#include <dtDAL/actorproxy.h>

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
