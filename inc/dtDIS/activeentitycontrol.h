#ifndef activeentitycontrol_h__
#define activeentitycontrol_h__

#include <dtDIS/dtdisexport.h>
#include <dtDIS/entityidcompare.h>
#include <DIS/EntityID.h>
#include <dtCore/uniqueid.h>
#include <dtCore/refptr.h>
#include <map>


namespace dtDIS
{

   ///\brief provides a single point for associating known entities & actors.
   ///
   /// Provides quick look-ups given either an entity or an actor
   /// by keeping 2 maps in sync.
   struct DT_DIS_EXPORT ActiveEntityControl
   {
   public:
      /// relate an EntityID with an Actor
      /// @return 'false' when any relation previously existed; 'true' if the relation was added.
      bool AddEntity(const DIS::EntityID& eid, const dtCore::UniqueId& id);

      /// remove a relation for the EntityID and Actor
      /// @return 'false' when no relation previously existed; 'true' if the relation was removed.
      bool RemoveEntity(const DIS::EntityID& eid, const dtCore::UniqueId& id);

      /// finds the associated Entity
      /// @return NULL when the proxy has no matching EntityID
      //const DIS::EntityID* GetEntity(const dtDAL::ActorProxy* proxy);
      const DIS::EntityID* GetEntity(const dtCore::UniqueId& uid) const;

      /// finds the associated Actor
      /// @return NULL when the EntityID has no matching Actor
      const dtCore::UniqueId* GetActor(const DIS::EntityID& eid) const;

      /// remove all state data.
      void ClearAll();

   private:
      typedef std::map<dtCore::UniqueId,DIS::EntityID> ActorToEntityMap;
      ActorToEntityMap mActorToEntityMap;

      typedef std::map<DIS::EntityID,dtCore::UniqueId,details::EntityIDCompare> EntityToActorMap;
      EntityToActorMap mEntityToActorMap;
   };
}
#endif // activeentitycontrol_h__
