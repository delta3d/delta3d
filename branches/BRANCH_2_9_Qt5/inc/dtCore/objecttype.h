/* -*-c++-*-
 * Delta3D
 * Copyright 2007-2008, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * david
 */

#ifndef DELTA_OBJECTTYPE
#define DELTA_OBJECTTYPE

#include <dtCore/export.h>
#include <string>
#include <osg/Referenced>
#include <dtCore/refptr.h>
#include <dtCore/namedparameter.h>
#include <dtUtil/hashmap.h>
#include <iosfwd>

namespace dtCore
{
   class DT_CORE_EXPORT ObjectType: public osg::Referenced
   {
   public:
      explicit ObjectType(const std::string& name,
                          const std::string& category="nocategory",
                          const std::string& desc="",
                          const ObjectType* parentType = NULL);

      static std::pair<std::string, std::string > ParseNameAndCategory(const std::string& fullName);

      /**
       * Simple less than comparison function for the ObjectFactory.
       * We cannot use the default comparison function because we
       * store smart pointers in the factory.  This implies that
       * the ActorType will not be the key, but the memory address
       * of the ActorType will be the key.  Therefore, this comparison
       * function exists so that the ActorType smart pointer is
       * dereferenced before the comparison.
       */
      struct RefPtrComp
      {
         bool operator()(const dtCore::RefPtr<const ObjectType>& id1,const dtCore::RefPtr<const ObjectType>& id2) const
         {
            return (*id1) < (*id2);
         }
      };

      /**
       * Sets the name for this actor type.
       */
      void SetName(const std::string& name);

      /**
       * Gets the name currently assigned to this actor type.
       */
      const std::string& GetName() const;

      /**
       * Sets the category for this actor type.
       */
      void SetCategory(const std::string& category);

      /**
       * Gets the category given to this actor type.
       */
      const std::string& GetCategory() const;

      /**
       * Sets the description for this actor type.
       */
      void SetDescription(const std::string& desc);

      /**
       * Gets the description given to this actor type.
       */
      const std::string& GetDescription() const;

      /**
       * Gets the uniqueId string which was generated for this actor type.
       */
      const std::string& GetUniqueId() const;

      /**
       * Gets the parent or "super" type of this actor type.
       */
      const ObjectType* GetParentType() const;

      /**
       * Based on this actor types super type hierarchy, this method determines whether
       * or not this type is a descendent or equal to the specified actor type.
       * @return True if a descendent or equal, false otherwise.
       */
      bool InstanceOf(const ObjectType& rhs) const;

      /**
       * Helper method which is the same as the other InstanceOf method, only this one
       * wraps the creation of the actor type.
       * @param category The category of the actor type in question.
       * @param name The name of the actor type in question.
       * @return True if a descendent or equal, false otherwise.
       * @see InstanceOf(const ActorType &rhs)
       */
      bool InstanceOf(const std::string& category, const std::string& name) const;

      /**
       * Determines whether this type is an instance based on the full name
       * @param fullName The combined category.name
       * @return True if a descendent or equal, false otherwise.
       * @see InstanceOf(const ActorType &rhs)
       */
      bool InstanceOf(const std::string& fullName) const;

      /**
       * Less-than comparison of the actor type's uniqueId strings.
       */
      bool operator<(const ObjectType& rhs) const;

      /**
       * Equality test of the actor type's uniqueId strings.
       */
      bool operator==(const ObjectType& rhs) const;

      /**
       * Inequality test of the actor type's uniqueId strings.
       */
      bool operator!=(const ObjectType& rhs) const;

      ///Get the fully qualified string representation for this ActorType.
      std::string GetFullName() const;

      /**
       * Returns whether a default exists.
       *
       * @param[in]  propName  The property.
       */
      bool DefaultExists(const dtUtil::RefString& propName) const;

      /**
       * Retrieves the default value of a property.
       *
       * @param[in]  propName  The property.
       *
       * @return     The default value (or NULL if none exists).
       */
      const NamedParameter* GetDefaultValue(const dtUtil::RefString& propName) const;

      /**
       * Sets the default value of a given property.
       *
       * @param[in]  propName      The property.
       * @param[in]  defaultValue  The default value of the property.
       */
      void SetDefaultValue(const dtUtil::RefString& propName, NamedParameter& defaultValue);

      /// This is used to see if the defaults have been initialized.
      bool DefaultsEmpty() const { return mDefaultValues.empty(); }

   protected:
      //Object can only be deleted through the ref_ptr interface.
      virtual ~ObjectType();

      /**
       * Creates a unique id for this ObjectType. Currently, the implementation
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
      const dtCore::RefPtr<const ObjectType> mParentType;

      typedef dtUtil::HashMap<dtUtil::RefString, dtCore::RefPtr<NamedParameter> > ValMap;
      ValMap mDefaultValues;
   };

   ///Provide a method for printing the actor type to a stream.
   DT_CORE_EXPORT std::ostream& operator<<(std::ostream& os, const ObjectType& objectType);

}

#endif /* OBJECTTYPE_H_ */
