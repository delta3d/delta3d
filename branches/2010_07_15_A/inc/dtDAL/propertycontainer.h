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
#include <dtDAL/export.h>
#include <dtDAL/actorproperty.h>
#include <osg/Referenced>

#include <map>
#include <vector>

namespace dtDAL
{
   class DT_DAL_EXPORT PropertyContainer: public osg::Referenced
   {
   public:
      PropertyContainer();

      /**
       * Gets a list of the properties currently registered for this
       * actor proxy.
       */
      void GetPropertyList(std::vector<ActorProperty *>& propList);

      /**
       * Gets a const list of the properties currently registered for this
       * actor proxy.
       */
      void GetPropertyList(std::vector<const ActorProperty *>& propList) const;

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
      virtual dtCore::RefPtr<ActorProperty> GetDeprecatedProperty(const std::string& name);

      /**
       * Copies the property values from the passed in property container to it's own properties
       */
      void CopyPropertiesFrom(const PropertyContainer& copyFrom);

   protected:
      virtual ~PropertyContainer();

      void RemoveProperty(const std::string& nameToRemove);

   private:
      typedef std::map<dtUtil::RefString, dtCore::RefPtr<ActorProperty> > PropertyMapType;
      typedef std::vector<dtCore::RefPtr<ActorProperty> > PropertyVectorType;

      ///Map of properties.
      PropertyMapType mPropertyMap;

      ///vector of properties (for order).
      PropertyVectorType mProperties;

   };

}

#endif /* PROPERTYCONTAINER_H_ */
