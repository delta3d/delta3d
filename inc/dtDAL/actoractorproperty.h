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

#ifndef ACTOR_ACTOR_PROPERTY
#define ACTOR_ACTOR_PROPERTY

#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/datatype.h>
#include <dtDAL/export.h>
#include <dtUtil/functor.h>

namespace dtDAL
{
   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents an actor
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT ActorActorProperty : public ActorProperty
   {
      public:
         typedef dtUtil::Functor<void, TYPELIST_1(BaseActorObject*)> SetFuncType;
         typedef dtUtil::Functor<dtCore::DeltaDrawable*, TYPELIST_0()> GetFuncType;
         ActorActorProperty(BaseActorObject& actorProxy,
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
         void SetValue(BaseActorObject* value);

         /**
          * Gets the value proxy assiged to this property.
          * Hack for the resource class
          * @return the currently set BaseActorObject for this property.
          */
         BaseActorObject* GetValue() const;

         /**
          * Gets the drawable that this property is representing
          * @return The actor
          */
         dtCore::DeltaDrawable* GetRealActor();

         /**
          * Gets the drawable that this property is representing
          * @return The actor
          */
         const dtCore::DeltaDrawable* GetRealActor() const;
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
          * @return the class of proxy this expects so that the UI can filter the list.
          */
         const std::string& GetDesiredActorClass() const;

      private:
         BaseActorObject* mProxy;
         SetFuncType SetPropFunctor;
         GetFuncType GetActorFunctor;
         dtUtil::RefString mDesiredActorClass;

      protected:
         virtual ~ActorActorProperty() { }
   };
}

#endif //ACTOR_ACTOR_PROPERTY
