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
#include <dtCore/actoridactorproperty.h>

#include <dtCore/deltadrawable.h>
#include <dtCore/datatype.h>
#include <dtCore/map.h>
#include <dtCore/project.h>
#include <dtCore/exceptionenum.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   ActorIDActorProperty::ActorIDActorProperty(BaseActorObject& actorProxy,
      const dtUtil::RefString& name,
      const dtUtil::RefString& label,
      SetFuncType Set,
      GetFuncType Get,
      const dtUtil::RefString& desiredActorClass,
      const dtUtil::RefString& desc,
      const dtUtil::RefString& groupName)
      : ActorProperty(DataType::ACTOR, name, label, desc, groupName)
      , SetIdFunctor(Set)
      , GetIdFunctor(Get)
      , mDesiredActorClass(desiredActorClass)
      , mShowPrototypes(false)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   ActorIDActorProperty::ActorIDActorProperty(
      const dtUtil::RefString& name,
      const dtUtil::RefString& label,
      SetFuncType Set,
      GetFuncType Get,
      const dtUtil::RefString& desiredActorClass,
      const dtUtil::RefString& desc,
      const dtUtil::RefString& groupName)
      : ActorProperty(DataType::ACTOR, name, label, desc, groupName)
      , SetIdFunctor(Set)
      , GetIdFunctor(Get)
      , mDesiredActorClass(desiredActorClass)
      , mShowPrototypes(false)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ActorIDActorProperty::CopyFrom(const ActorProperty& otherProp)
   {
      if (GetDataType() != otherProp.GetDataType())
      {
         LOG_ERROR("Property types are incompatible. Cannot make copy.");
         return;
      }

      const ActorIDActorProperty* prop =
         static_cast<const ActorIDActorProperty* >(&otherProp);

      if (prop != NULL)
      {
         SetValue(prop->GetValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ActorIDActorProperty::SetValue(const dtCore::UniqueId& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("SetValue has been called on a property that is read only.");
         return;
      }

      SetIdFunctor(value);
   }

   ////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId ActorIDActorProperty::GetValue() const
   {
      return GetIdFunctor();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::BaseActorObject* ActorIDActorProperty::GetActor()
   {
      dtCore::UniqueId idValue = GetValue();
      if (idValue.ToString().empty()) return NULL;

      try
      {
         dtCore::Map* map = Project::GetInstance().GetMapForActor(idValue);
         if (map)
         {
            BaseActorObject* proxyValue = map->GetProxyById(idValue);
            if (proxyValue)
            {
               return proxyValue;
            }
         }

         // If we haven't found the actor as part of a loaded map, search
         // the hard way through all actor base instances instead.
         int count = BaseActorObject::GetInstanceCount();
         for (int index = 0; index < count; ++index)
         {
            BaseActorObject* proxyValue = BaseActorObject::GetInstance(index);
            if (proxyValue && proxyValue->GetId() == idValue)
            {
               return proxyValue;
            }
         }
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
            __FUNCTION__, __LINE__, "Error setting ActorIDActorProperty.  Setting property %s with string value failed. Error Message %s.",
            GetName().c_str(), ex.What().c_str());
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject* ActorIDActorProperty::GetActor() const
   {
      dtCore::UniqueId idValue = GetValue();
      if (idValue.ToString().empty()) return NULL;

      try
      {
         dtCore::Map* map = Project::GetInstance().GetMapForActor(idValue);
         if (map)
         {
            BaseActorObject* proxyValue = map->GetProxyById(idValue);
            if (proxyValue)
            {
               return proxyValue;
            }
         }

         int count = BaseActorObject::GetInstanceCount();
         for (int index = 0; index < count; ++index)
         {
            BaseActorObject* proxyValue = BaseActorObject::GetInstance(index);
            if (proxyValue && proxyValue->GetId() == idValue)
            {
               return proxyValue;
            }
         }
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
            __FUNCTION__, __LINE__, "Error setting ActorIDActorProperty.  Setting property %s with string value failed. Error Message %s.",
            GetName().c_str(), ex.What().c_str());
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool ActorIDActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      if (value.empty() || value == "NULL")
      {
         dtCore::UniqueId id(false);

         SetValue(id);
         return true;
      }

      dtCore::UniqueId newIdValue(value);
      SetValue(newIdValue);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string ActorIDActorProperty::ToString() const
   {
      return GetValue().ToString();
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ActorIDActorProperty::GetValueString() const
   {
      const BaseActorObject* actor = GetActor();
      if (actor)
      {
         return actor->GetName();
      }

      if (!GetValue().ToString().empty())
      {
         return "Unknown";
      }

      return "None";
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& ActorIDActorProperty::GetDesiredActorClass() const
   {
      return mDesiredActorClass;
   }
   ////////////////////////////////////////////////////////////////////////////
}

