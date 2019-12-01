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
 *
 * David Guthrie
 */

#ifndef DELTA_PROPERTYCONTAINER
#define DELTA_PROPERTYCONTAINER

#include <dtUtil/refstring.h>
#include <dtCore/refptr.h>
#include <dtCore/export.h>
#include <dtCore/actorproperty.h>
#include <dtCore/objecttype.h>
#include <osg/Referenced>
#include <dtUtil/breakoverride.h>

#include <map>
#include <vector>
#include <algorithm>

namespace dtCore
{
   class DT_CORE_EXPORT PropertyContainer: public osg::Referenced
   {
   public:
      typedef std::vector<ActorProperty *> PropertyVector;
      typedef std::vector<const ActorProperty *> PropertyConstVector;
      typedef std::vector<RefPtr<ActorProperty> > PropertyPtrVector;

      PropertyContainer();

      ///@return the type of this object.
      virtual const ObjectType& GetObjectType() const = 0;
   public:
      /**
       * Initializes the default values of this actor.
       */
      void InitDefaults();

      /**
       * Gets a list of the properties currently registered for this
       * actor proxy.
       */
      void GetPropertyList(PropertyVector& propList);

      /**
       * Gets a const list of the properties currently registered for this
       * actor proxy.
       */
      void GetPropertyList(PropertyConstVector& propList) const;

      /**
       * Method to test if a property instance is contained within this container.
       * @param prop Property object to find in the container.
       * @return TRUE if the property instance matches a reference in this container.
       */
      bool HasProperty(ActorProperty& prop) const;

      /**
       * Adds a new property to the this proxy's list of properties.
       * This really should not be public.
       * @note
       *      Properties must have unique names, therefore, if a property
       *      is added that who's name collides with another, the property
       *      is not added and an error message is logged.
       */
      void AddProperty(ActorProperty* newProp);

      /**
       * Removes a property from this property container by name.
       */
      void RemoveProperty(const std::string& nameToRemove);

      /**
       * Removes a property from this property container.
       */
      void RemoveProperty(ActorProperty* newProp);

      /**
       * Inserts a new property into the container at a given index.
       *
       * @param[in]  newProp  The property to insert.
       * @param[in]  index    The index to insert.
       */
      void InsertProperty(ActorProperty* newProp, int index);

      /**
       * Gets a property of the requested name.
       *
       * @param name Name of the property to retrieve.
       * @return A pointer to the property object or NULL if it
       * is not found.
       */
      ActorProperty* GetProperty(const std::string& name);

      /**
       * Templated version of GetProperty (non-const) that auto casts the property to the desired type.
       * Warning: this uses a dynamic_cast, so you are able to shoot yourself in the foot
       * if you pass in the wrong type of ActorProperty.
       */
      template<class PropertyType>
      void GetProperty(const std::string& name, PropertyType*& property)
      {
         property = dynamic_cast<PropertyType*>(GetProperty(name));
      }

      /**
       * Gets a property of the requested name. (const version)
       * @param name Name of the property to retrieve.
       * @return A pointer to the property object or NULL if it
       * is not found.
       */
      const ActorProperty* GetProperty(const std::string& name) const;

      /// Perform the given action for each property.
      template <typename UnaryFunctor>
      void ForEachProperty(UnaryFunctor func);

      /**
       * This function queries the proxy with any properties not
       * found in the property list. If a property was previously
       * removed from the proxy, but is still important to load,
       * then this function should return a temporary property of
       * the appropriate type to be used when loading the map.
       *
       * @param[in]  name  The name of the property queried for.
       *
       * @return           A temporary property, or NULL if
       *                   none is needed.
       */
      virtual RefPtr<ActorProperty> GetDeprecatedProperty(const std::string& name);

      /**
       * Checks if a given property has a default value.
       *
       * @param[in]  prop  The property.
       *
       * @return     True if a default value exists for the given property.
       */
      virtual bool DoesDefaultExist(const ActorProperty& prop) const;

      /**
       * Checks if a given property is already set to its default value.
       *
       * @param[in]  prop  The property.
       *
       * @return     True if the given property is already set to default.
       */
      virtual bool IsPropertyDefault(const ActorProperty& prop) const;

      /**
       * Resets the value of the given property to its default value.
       *
       * @param[in]   prop  The property to reset.
       */
      virtual void ResetProperty(ActorProperty& prop);

      /**
       * Checks if a given property should be saved out to file data.
       * The flags on a property can be used to control this, so this shouldn't be needed unless you need to do
       * something custom.
       *
       * @param[in]  prop  The property.
       *
       * @return     True if the given property should be saved.
       */
      virtual bool ShouldPropertySave(const ActorProperty& prop) const;

      /**
       * Copies the property values from the passed in property container to it's own properties
       * @param copyMetadata copies the metadata from the properties.  If the objects are the same type, you probably want to do this
       *                     if not, you probably don't.
       */
      virtual void CopyPropertiesFrom(const PropertyContainer& copyFrom, bool copyMetadata = true);

      /**
       * @return the number of properties in this container.
       */
      unsigned GetNumProperties() const;

   protected:

      virtual ~PropertyContainer();

      BREAK_OVERRIDE(GetDefaultPropertyKey() const); // removed 12/2014
   private:

      typedef std::map<dtUtil::RefString, RefPtr<ActorProperty> > PropertyMapType;
      typedef std::vector<RefPtr<ActorProperty> > PropertyVectorType;

      ///Map of properties.
      PropertyMapType mPropertyMap;

      ///vector of properties (for order).
      PropertyVectorType mProperties;
   };

   typedef RefPtr<PropertyContainer> PropertyContainerPtr;
   typedef std::vector<PropertyContainer*> PropContPtrVector;
   typedef std::vector<PropertyContainerPtr> PropContRefPtrVector;

   template <typename UnaryFunctor>
   inline void PropertyContainer::ForEachProperty(UnaryFunctor func)
   {
      std::for_each(mProperties.begin(), mProperties.end(), func);
   }

}

#endif /* PROPERTYCONTAINER_H_ */
