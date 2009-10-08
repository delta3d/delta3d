/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * Jeffrey Houde
 */
#ifndef DELTA_CONTAINER_ACTOR_PROPERTY_BASE
#define DELTA_CONTAINER_ACTOR_PROPERTY_BASE

#include <string>
#include <vector>
#include <dtDAL/actorproperty.h>
#include <dtDAL/export.h>
#include <dtCore/refptr.h>

#include <dtUtil/log.h>

#include <dtUtil/macros.h>

namespace dtDAL
{
   /**
   * @brief An actor property that contains a structure of other actor property Objects.
   *
   * This actor property allows you to contain a number of other actor properties within itself.
   */
   class DT_DAL_EXPORT ContainerActorProperty : public ActorProperty
   {
   public:

      /**
      * Creates a new container actor property.
      * @param[in]  name            The name used to access this property.
      * @param[in]  label           A more human readable name to use when displaying the property.
      * @param[in]  desc            A longer description of the property.
      * @param[in]  groupName       The property grouping to use when editing properties visually.
      * @param[in]  editorType      A string specifying what type of editor to use for the data in this group.  These
      *                             will specify a UI to use in STAGE.
      * @param[in]  readOnly        True if this property should not be editable.  Defaults to false.
      */
      ContainerActorProperty(const std::string& name,
                              const std::string& label,
                              const std::string& desc,
                              const std::string& groupName,
                              const std::string& editorType = "",
                              bool readOnly = false);

      /**
      * Sets the value of the property based on a string.
      * @note This method will attempt to use the data to set the value, but it may return false if the data
      * could not be used.
      * @param value the value to set.
      * @return true if the string was usable to set the value, false if not.
      */
      virtual bool FromString(const std::string& value);

      /**
      * Reads the next token form the given string data.
      * This will also remove the token from the data string
      * and return you the token (with the open and close characters removed).
      * The beginning of the data string must always begin with
      * an opening character or this will cause problems.
      *
      * @param[in]  data  The string data.
      *
      * @return            The first token from the string data.
      */
      std::string TakeToken(std::string& data);

      /**
      * @return a string version of the data.  This value can be used when calling SetStringValue.
      * @see #SetStringValue
      */
      virtual const std::string ToString() const;

      /**
      * This is overridden to make handle the fact that the get method returns a refptr.
      * @param otherProp The property to copy from.
      */
      virtual void CopyFrom(const ActorProperty& otherProp);

      /**
      * Adds a property into the container.
      *
      * @param[in]  property  The property to add.
      */
      void AddProperty(ActorProperty* property);

      /**
      * Retrieves a property in the container.
      *
      * @param[in]  The index to the property to retrieve.
      */
      ActorProperty* GetProperty(int index);
      const ActorProperty* GetProperty(int index) const;

      /**
      * Retrieves the current property (used in save/load).
      */
      ActorProperty* GetCurrentProperty();
      const ActorProperty* GetCurrentProperty() const;

      /**
      * Gets the current property index.
      */
      int GetCurrentPropertyIndex() const;

      /**
      * Sets the current property index.
      *
      * @param[in]  index  The new index.
      */
      void SetCurrentPropertyIndex(int index);

      /**
      * Retrieves the number of properties contained.
      *
      * @return  The total number of properties within the container.
      */
      int GetPropertyCount() const;

   protected:

      /**
      * Deconstructor.
      */
      virtual ~ContainerActorProperty();

      /// The property that each array index uses.
      std::vector<dtCore::RefPtr<ActorProperty> > mProperties;

      int mCurrentIndex;
   };
}
#endif
