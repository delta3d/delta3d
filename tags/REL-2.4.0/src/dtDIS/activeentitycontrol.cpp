#include <dtDIS/activeentitycontrol.h>
#include <dtDAL/actorproxy.h>

using namespace dtDIS;

bool ActiveEntityControl::AddEntity(const DIS::EntityID& eid, const dtCore::UniqueId& id)
{
   bool inserted = mActorToEntityMap.insert(ActorToEntityMap::value_type(id,eid)).second;
   if (inserted)
   {
      inserted = mEntityToActorMap.insert(EntityToActorMap::value_type(eid, id)).second;
      if (!inserted)
      {
         // keep it in sync with the other container
         mActorToEntityMap.erase(id);
      }

      return inserted;
   }

   return false;
}

bool ActiveEntityControl::RemoveEntity(const DIS::EntityID& eid, const dtCore::UniqueId& id)
{
   bool pub = mActorToEntityMap.erase(id) > 0;
   bool act = mEntityToActorMap.erase(eid) > 0;
   return(pub && act);
}

const DIS::EntityID* ActiveEntityControl::GetEntity(const dtCore::UniqueId& uid) const
{
   ActorToEntityMap::const_iterator iter = mActorToEntityMap.find(uid);
   if (iter != mActorToEntityMap.end())
   {
      return (&(iter->second));
   }

   return NULL;
}

const dtCore::UniqueId* ActiveEntityControl::GetActor(const DIS::EntityID& eid) const
{
   EntityToActorMap::const_iterator iter = mEntityToActorMap.find(eid);
   if (iter != mEntityToActorMap.end())
   {
      return( &(iter->second) );
   }

   return NULL;
}

void ActiveEntityControl::ClearAll()
{
   mActorToEntityMap.clear();
}
