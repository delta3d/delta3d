/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * Matthew W. Campbell
 */
#ifndef DELTA_ACTORTYPE
#define DELTA_ACTORTYPE

#include <dtDAL/export.h>
#include <dtDAL/objecttype.h>

namespace dtDAL
{

   class BaseActorObject;

   /**
    * This class is more or less a simple data class that has information
    * describing a particular type of Actor.  Actor types contain a name
    * category and description.
    * @note
    *   ActorType objects must be unique, thereby implying no two
    *   actor types will have the same name and category.
    * @note
    *   Categories of an actor type are represented in a hierarchial dot
    *   notation.  For example, vehicles.trucks.BouncyTruck
    * @note
    *   ActorType objects have a notion of a type hierarchy.  Therefore, when
    *   creating a new actor type, its parent type must be specified.  This allows
    *   queries to be made against the actor type to dermine if it is an "instance"
    *   of another type.
    */
   class DT_DAL_EXPORT ActorType : public ObjectType
   {
   public:

      typedef BaseActorObject CreateType;

      /**
       * Constructs a new actor type object.
       */
      ActorType(const std::string& name,
               const std::string& category="nocategory",
               const std::string& desc="",
               const ActorType* parentType = NULL);


      /**
       * Gets the parent or "super" type of this actor type.
       */
      const ActorType* GetParentActorType() const;


   protected:

      //Object can only be deleted through the ref_ptr interface.
      virtual ~ActorType();

   private:
   };
}

#endif
