/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2010, MOVES Institute, Alion Science and Technology Inc.
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
*/

#ifndef DELTA_ACTORCOMPONENTCONTAINER_H
#define DELTA_ACTORCOMPONENTCONTAINER_H

#include <dtCore/export.h>
#include <dtCore/baseactorobject.h>
#include <dtCore/actortype.h>
#include <dtUtil/breakoverride.h>
#include <dtUtil/deprecationmgr.h>

#include <dtUtil/referencedinterface.h>

#include <algorithm>
#include <map>

namespace dtCore
{
   class BaseActorObject;

   /**
    * A pure virtual container for ActorComponents.
    * Use multiple inheritance to add this to any actor like class.
    * A simple component system for modularizing game actor functionality.
    * Each actor has a number of components that can be retrieved by type.
    */
   class ActorComponentContainer
   {
   public:
      ActorComponentContainer() {}

      /**
       * Get all components matching this type string
       * @param type The type-string of the ActorComponent to get
       * @return the selected ActorComponents (will be empty if not found)
       */
      virtual void GetComponents(dtCore::ActorTypePtr type, dtCore::ActorPtrVector& outComponents) const = 0;

      /**
       * Fill the vector with all the actor components.
       */
      virtual void GetAllComponents(dtCore::ActorPtrVector& toFill) = 0;

      /**
       * Add an ActorComponent. Only one ActorComponent of a given type can be added.
       * @param component The ActorComponent to try to add
       */
      virtual void AddComponent(dtCore::BaseActorObject& component) = 0;

      /**
       * Iterator for traversing the structure of the actor.
       */
      class ActorIterator : public osg::Referenced
      {
      public:
         ActorIterator() {}

         virtual BaseActorObject* operator* () const = 0;
         virtual ActorIterator& operator++ () = 0;

         virtual bool IsAtEnd() const = 0;

      protected:
         virtual ~ActorIterator() {}
      };

      /**
       * Returns an abstract iterator for traversing the structure of the actor.
       */
      virtual dtCore::RefPtr<ActorIterator> GetIterator() { return NULL; };

      /**
       * TEMPORARY method to determine if an object has a parent.
       */
      virtual dtCore::BaseActorObject* GetParentBaseActor() const = 0;
      virtual bool SetParentBaseActor(dtCore::BaseActorObject* parent) = 0;

   protected:
      virtual ~ActorComponentContainer() {}

   private:
      ActorComponentContainer(const dtCore::ActorComponentContainer&) {}
      ActorComponentContainer& operator= (const dtCore::ActorComponentContainer&) { return *this; }
   };

}

#endif /* ACTORCOMPONENTCONTAINER_H_ */
