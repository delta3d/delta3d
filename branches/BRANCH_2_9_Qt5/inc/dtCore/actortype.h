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

#include <dtCore/export.h>
#include <dtCore/objecttype.h>
#include <dtUtil/refstring.h>
#include <set>

namespace dtCore
{

   class BaseActorObject;

   class SharedClassInfo: public osg::Referenced
   {
   public:
      typedef std::set<dtUtil::RefString> ClassHierarchyType;

      void SetClassName(const dtUtil::RefString& name);

      const dtUtil::RefString& GetClassName() const;

      /**
       * Returns if one object is an instance of another
       * Instead of using this functionality, use the similar set on the actor type.
       * @param name The name of the object to check against
       * @return true if it is, false if not
       */
      bool IsInstanceOf(const dtUtil::RefString& name) const;

      /// Set of class names
      ClassHierarchyType mClassHierarchy;
   private:
      dtUtil::RefString mClassName;
   };

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
   class DT_CORE_EXPORT ActorType : public ObjectType
   {
   public:

      /**
       * Constructs a new actor type object.
       */
      explicit ActorType(const std::string& name,
                         const std::string& category="nocategory",
                         const std::string& desc="",
                         const ActorType* parentType = NULL);


      /**
       * Gets the parent or "super" type of this actor type.
       */
      const ActorType* GetParentActorType() const;

      SharedClassInfo& GetSharedClassInfo() const;
      void MergeSharedClassInfo(SharedClassInfo& clsInfo) const;

   protected:

      //Object can only be deleted through the ref_ptr interface.
      virtual ~ActorType();

   private:
      mutable dtCore::RefPtr<SharedClassInfo> mClassInfo;
   };

   typedef dtCore::RefPtr<const ActorType> ActorTypePtr;
   typedef std::vector<ActorTypePtr> ActorTypeVec;
}

#endif
