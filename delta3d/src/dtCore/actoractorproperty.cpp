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
 * Eric Heine
 */
#include <prefix/dtcoreprefix.h>
#include <dtCore/actoractorproperty.h>
#include <dtCore/map.h>
#include <dtCore/project.h>
#include <dtCore/exceptionenum.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   ActorActorProperty::ActorActorProperty(BaseActorObject& actorProxy,
                     const dtUtil::RefString& name,
                     const dtUtil::RefString& label,
                     SetFuncType Set,
                     GetFuncType Get,
                     const dtUtil::RefString& desiredActorClass,
                     const dtUtil::RefString& desc,
                     const dtUtil::RefString& groupName)
      : ActorProperty(DataType::ACTOR, name, label, desc, groupName)
      , mProxy(&actorProxy)
      , SetPropFunctor(Set)
      , mDesiredActorClass(desiredActorClass)
      , mLastValue()
      , mShowPrototypes(false)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ActorActorProperty::CopyFrom(const ActorProperty& otherProp)
   {
      if (GetDataType() != otherProp.GetDataType())
      {
         LOG_ERROR("Property types are incompatible. Cannot make copy.");
         return;
      }

      const ActorActorProperty* prop =
         dynamic_cast<const ActorActorProperty* >(&otherProp);

      if (prop != NULL)
      {
         SetValue(prop->GetValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ActorActorProperty::SetValue(BaseActorObject* value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("SetValue has been called on a property that is read only.");
         return;
      }

      mLastValue = value;
      SetPropFunctor(value);
   }

   ////////////////////////////////////////////////////////////////////////////
   BaseActorObject* ActorActorProperty::GetValue() const
   {
      return mLastValue;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool ActorActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      if (value.empty() || value == "NULL")
      {
         SetValue(NULL);
         return true;
      }

      dtCore::UniqueId newIdValue(value);
      try
      {
         Map* map = Project::GetInstance().GetMapForActor(*mProxy);

         if (map == NULL)
         {
            dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_INFO,
                                                                           __FUNCTION__, __LINE__, "Actor does not exist in a map.  Setting property %s with string value failed.",
                                                                           GetName().c_str());
            return false;
         }

         BaseActorObject* newProxyValue = map->GetProxyById(newIdValue);
         if (newProxyValue == NULL)
         {
            dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_INFO,
                                                                           __FUNCTION__, __LINE__, "Actor with ID %s not found.  Setting property %s with string value failed.",
                                                                           value.c_str(), GetName().c_str());
            return false;
         }

         SetValue(newProxyValue);
         return true;
      }
      catch (const dtCore::ProjectInvalidContextException& ex)
      {
         dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_WARNING,
            __FUNCTION__, __LINE__, "Project context is not set, unable to lookup actors.  Setting property %s with string value failed. Error Message %s.",
            GetName().c_str(), ex.What().c_str());

      }
      catch (const dtUtil::Exception& ex)
      {
         dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_WARNING,
            __FUNCTION__, __LINE__, "Error setting ActorActorProperty.  Setting property %s with string value failed. Error Message %s.",
            GetName().c_str(), ex.What().c_str());
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string ActorActorProperty::ToString() const
   {
      return GetValue() == NULL ? "" : GetValue()->GetId().ToString();
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& ActorActorProperty::GetDesiredActorClass() const
   {
      return mDesiredActorClass;
   }

   ////////////////////////////////////////////////////////////////////////////

}
