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
 * @author Matthew W. Campbell
 */
#ifndef DELTA_ACTORTYPE
#define DELTA_ACTORTYPE

#include <string>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <dtCore/refptr.h>
#include <ostream>
#include "dtDAL/export.h"

namespace dtDAL 
{

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
   class DT_DAL_EXPORT ActorType : public osg::Referenced 
   {   
      public:
         
         /**
          * Simple less than comparison function for the ObjectFactory.
          * We cannot use the default comparison function because we
          * store smart pointers in the factory.  This implies that
          * the ActorType will not be the key, but the memory address
          * of the ActorType will be the key.  Therefore, this comparison
          * function exists so that the ActorType smart pointer is
          * deferenced before the comparison.
          */
         struct RefPtrComp 
         {
            bool operator()(const osg::ref_ptr<ActorType> &id1, const osg::ref_ptr<ActorType> &id2) const
            {
               return (*id1) < (*id2);
            }
               
            bool operator()(const dtCore::RefPtr<ActorType> &id1,const dtCore::RefPtr<ActorType> &id2) const
            {
               return (*id1) < (*id2);
            }
         };

         /**
          * Constructs a new actor type object.
          */
         ActorType(const std::string &name, const std::string &category="nocategory",
                  const std::string &desc="", const ActorType *parentType = NULL) : 
            mName(name), mCategory(category), mDescription(desc), mParentType(parentType)
         {
            GenerateUniqueId();
         }

         /**
          * Sets the name for this actor type.
          */
         void SetName(const std::string &name) 
         {
            mName = name;
            GenerateUniqueId();
         }

         /**
          * Gets the name currently assigned to this actor type.
          */
         const std::string &GetName() const { return mName; }

         /**
          * Sets the category for this actor type.
          */
         void SetCategory(const std::string &category) 
         {
            mCategory = category;
            GenerateUniqueId();
         }

         /**
          * Gets the category given to this actor type.
          */
         const std::string &GetCategory() const { return mCategory; }

         /**
          * Sets the description for this actor type.
          */
         void SetDescription(const std::string &desc) { mDescription = desc; }

         /**
          * Gets the description given to this actor type.
          */
         const std::string &GetDescription() const { return mDescription; }

         /**
          * Gets the uniqueId string which was generated for this actor type.
          */
         const std::string &GetUniqueId() const { return mUniqueId; }
            
         /**
          * Gets the parent or "super" type of this actor type.
          */
         const ActorType *GetParentActorType() const { return mParentType.get(); }
            
         /**
          * Based on this actor types super type hierarchy, this method determines whether
          * or not this type is a descendent or equal to the specified actor type.
          * @return True if a descendent or equal, false otherwise.
          */
         bool InstanceOf(const ActorType &rhs);

         /**
          * Helper method which is the same as the other InstanceOf method, only this one
          * wraps the creation of the actor type.
          * @param category The category of the actor type in question.
          * @param name The name of the actor type in question.
          * @return True if a descendent or equal, false otherwise.
          * @see InstanceOf(const ActorType &rhs)
          */
         bool InstanceOf(const std::string &category, const std::string &name);
            
         /**
          * Less-than comparison of the actor type's uniqueId strings.
          */
         bool operator<(const ActorType &rhs) const
         {
            return (mUniqueId < rhs.mUniqueId);
         }

         /**
          * Equality test of the actor type's uniqueId strings.
          */
         bool operator==(const ActorType &rhs) const 
         {
            return (mUniqueId == rhs.mUniqueId);
         }

         /**
          * Inequality test of the actor type's uniqueId strings.
          */
         bool operator!=(const ActorType &rhs) const 
         {
            return (mUniqueId != rhs.mUniqueId);
         }

      protected:
         
         //Object can only be deleted through the ref_ptr interface.
         ~ActorType() { }

         /**
          * Creates a unique id for this ActorType. Currently, the implementation
          * merely concatenates the name and category.  Probably should hash this
          * and store a hash id instead.
          */
         void GenerateUniqueId();

      private:
         std::string mName;
         std::string mCategory;
         std::string mDescription;

         ///UniqueId for this actor type.
         std::string mUniqueId;
            
         ///Parent of this actor type.  Null indicates there is no super type to this one.
         const dtCore::RefPtr<const ActorType> mParentType;

         ///Provide a method for printing the actor type to a stream.
         friend std::ostream& operator<<(std::ostream& os,const ActorType &actorType)
         {
            os << actorType.GetCategory() + "." + actorType.GetName();
            return os;
         }

         ///Provide a method for printing the actor type smart pointer to a stream.
         friend std::ostream &operator<<(std::ostream &os,const osg::ref_ptr<ActorType> &actorType)
         {
            os << actorType->GetCategory() + "." + actorType->GetName();
            return os;
         }
   };
}

#endif
