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
 * David Guthrie
 * William E. Johnson II
 * Eric Heine
 */

#ifndef ACTOR_ID_ACTOR_PROPERTY
#define ACTOR_ID_ACTOR_PROPERTY

#include <dtCore/uniqueid.h>
#include <dtCore/actorproperty.h>
#include <dtCore/actorproxy.h>
#include <dtCore/export.h>
#include <dtUtil/functor.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   /**
   * This actor property represents an actor
   */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT ActorIDActorProperty : public ActorProperty
   {
   public:
      typedef dtUtil::Functor<void, TYPELIST_1(const dtCore::UniqueId&)> SetFuncType;
      typedef dtUtil::Functor<dtCore::UniqueId, TYPELIST_0()> GetFuncType;

      /**
       * Constructor.
       *
       * @param[in]  actorProxy         The actor proxy that belongs to the map.
       * @param[in]  name               The name of the property.
       * @param[in]  label              The displayed name of the property.
       * @param[in]  Set                Set callback functor.
       * @param[in]  Get                Get callback functor.
       * @param[in]  MapFunctor         A callback functor to retrieve the map.
       * @param[in]  desiredActorClass  This is the actor class type that
       *                                will be valid for this property.
       * @param[in]  desc               Description of the property.
       * @param[in]  groupName          The group category to place this property.
       */
      ActorIDActorProperty(BaseActorObject& actorProxy,
         const dtUtil::RefString& name,
         const dtUtil::RefString& label,
         SetFuncType Set,
         GetFuncType Get,
         const dtUtil::RefString& desiredActorClass = "",
         const dtUtil::RefString& desc = "",
         const dtUtil::RefString& groupName = "");

      ActorIDActorProperty(
         const dtUtil::RefString& name,
         const dtUtil::RefString& label,
         SetFuncType Set,
         GetFuncType Get,
         const dtUtil::RefString& desiredActorClass = "",
         const dtUtil::RefString& desc = "",
         const dtUtil::RefString& groupName = "");

      /**
      * Copies an ActorActorProperty value to this one from the property
      * specified. This method fails if otherProp is not an ActorActorProperty.
      * @param otherProp The property to copy the value from.
      */
      virtual void CopyFrom(const ActorProperty& otherProp);

      /**
      * Sets the value of this property by calling the set functor
      * assigned to this property.
      * Hack for the resource class
      * @param value the value to set or NULL to clear it.  The passed in pointer is
      * not stored.  The values are extracted and stored in a separate object.
      */
      void SetValue(const dtCore::UniqueId& value);

      /**
      * Gets the value proxy assiged to this property.
      * Hack for the resource class
      * @return the currently set BaseActorObject for this property.
      */
      dtCore::UniqueId GetValue() const;

      /**
       *	Gets and Sets whether this property shows prototype actors.
       */
      void SetShowPrototypes(bool enabled) {mShowPrototypes = enabled;}
      bool GetShowPrototypes() const {return mShowPrototypes;}

      /**
      * Gets the actor proxy that this property is representing
      * @return The actor proxy.
      */
      dtCore::BaseActorObject* GetActor();

      /**
      * Gets the actor proxy that this property is representing
      * @return The actor proxy.
      */
      const dtCore::BaseActorObject* GetActor() const;

      /**
      * Sets the value of the property based on a string.
      * The string should be the both the unique id and the display string separated by a comma.
      * @note Returns false it the property is read only
      * @param value the value to set.
      * @return false if the value does not reference an existing proxy or the proxy is of the wrong class.
      */
      virtual bool FromString(const std::string& value);

      /**
      * @return a string version of the data.  This value can be used when calling FromString.
      * @see #FromString
      */
      virtual const std::string ToString() const;

      /**
       * Retrieves a human readable version of the property's value.
       */
      virtual std::string GetValueString() const;

      /**
      * @return the class of proxy this expects so that the UI can filter the list.
      */
      const std::string& GetDesiredActorClass() const;

   private:
      SetFuncType SetIdFunctor;
      GetFuncType GetIdFunctor;
      dtUtil::RefString mDesiredActorClass;
      bool mShowPrototypes;

   protected:
      virtual ~ActorIDActorProperty() { }
   };
}

#endif //ACTOR_ID_ACTOR_PROPERTY
