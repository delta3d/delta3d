/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2009, MOVES Institute
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
* @author Choco (forum handle)
* @author David Guthrie
* @author Curtiss Murphy
*/

#ifndef DELTA_ACTOR_COMPONENT_H
#define DELTA_ACTOR_COMPONENT_H

#include <dtCore/export.h>
#include <dtCore/baseactor.h>
#include <dtCore/actortype.h>
#include <dtCore/observerptr.h>
#include <dtCore/sigslot.h>
#include <dtUtil/refstring.h>
#include <dtUtil/getsetmacros.h>
#include <typeinfo>  //for bad_cast

namespace dtCore
{
   /**
    * Abstract interface class for ActorComponent. ActorComponent can be retrieved from 
    * an actor by type. ActorComponent have to register to receive any GameActor messages.
    * ActorComponent implementing this interface have to provide a value
    * for ACType to identify the ActorComponent type.
    * When the ActorComponent is added to an actor, it receives a
    * reference to the actor.
    *
    * Note - Actor Components can reference each other, but you should not check for another
    * actor component until the OnEnteredWorld() method. This gives each actor component a chance
    * to initialize and get properties from the map regardless of order.
    */
   class DT_CORE_EXPORT ActorComponent : public dtCore::BaseActor, public sigslot::has_slots<>
   {

   public:

      typedef dtCore::BaseActor BaseClass;

      /**
       *  String used to identify component.
       */
      typedef dtCore::RefPtr<const dtCore::ActorType> ACType;

      ///b All derived actor component types must pass this or a descendant as a parent type.
      static const ACType BaseActorComponentType;

      /**
       * Constructor.
       * @param type Type string to identify component class
       */
      ActorComponent(ACType type);

      /**
       * Get type string of this component
       * @return The type string of this ActorComponent
       */
      ACType GetType() const;

      /**
       * Called when the parent actor enters the "world".
       */
      virtual void OnEnteredWorld();

      /** 
       * Called when the parent actor leaves the "world".
       */
      virtual void OnRemovedFromWorld();

      /**
       * Get the actor that owns this component. Cast to given pointer type.
       * Usage example:
       * @code 
       * MyComponentBaseActor* parent;
       * mycomponent->GetParent(parent);
       * @endcode
       */
      template <typename T_ParentPtr>
      void GetParentAs(T_ParentPtr& parentPtr) const
      {
         if (GetParent() == NULL)
         {
            parentPtr = NULL;
         }
         else
         {   
            parentPtr = dynamic_cast<T_ParentPtr>(GetParent());
            if (parentPtr == NULL)
            {
               throw std::bad_cast();
            }
         }
      }

      template <typename T_Parent>
      T_Parent* GetParentAs() const
      {
         T_Parent* parent = NULL;
         GetParentAs(parent);
         return parent;
      }

      /** 
       * Called when this actor gets added to another actor.
       * Override for custom behavior.
       * @param actor Parent actor this actor has been added to.
       */
      virtual void OnAddedToActor(dtCore::BaseActor& actor);

      /**
       * Called when this removed is removed from its parent actor.
       * Override for custom behavior.
       * @param actor Parent actor this actor was removed from.
       */
      virtual void OnRemovedFromActor(dtCore::BaseActor& actor);

      // TEMP:
      void AddPropertiesToRootActor();

      // TEMP:
      void RemovePropertiesFromRootActor();

   protected:

      virtual ~ActorComponent();

      virtual bool IsPlaceable() const { return false; }
   };
}
#endif // DELTA_ACTOR_COMPONENT_H
