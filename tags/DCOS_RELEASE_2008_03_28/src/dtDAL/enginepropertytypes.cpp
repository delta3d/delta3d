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
 * David Guthrie
 */
#include <prefix/dtdalprefix-src.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/project.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/map.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/gameeventmanager.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>

#include <osg/io_utils>

namespace dtDAL
{

   ////////////////////////////////////////////////////////////////////////////
   bool ActorActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
         return false;

      if (value.empty() || value == "NULL")
      {
         SetValue(NULL);
         return true;
      }

      dtCore::UniqueId newIdValue = value;
      try
      {
         Map* map = Project::GetInstance().GetMapForActorProxy(*mProxy);

         if (map == NULL)
         {
            dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_INFO,
                                                                           __FUNCTION__, __LINE__, "Actor does not exist in a map.  Setting property %s with string value failed.",
                                                                           GetName().c_str());
            return false;
         }

         ActorProxy* newProxyValue = map->GetProxyById(newIdValue);
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
      catch (const dtUtil::Exception& ex)
      {
         if (ex.TypeEnum() == ExceptionEnum::ProjectInvalidContext)
         {
            dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_WARNING,
                                                                           __FUNCTION__, __LINE__, "Project context is not set, unable to lookup actors.  Setting property %s with string value failed. Error Message %s.",
                                                                           GetName().c_str(), ex.What().c_str());
         }
         else
         {
            dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_WARNING,
                                                                           __FUNCTION__, __LINE__, "Error setting ActorActorProperty.  Setting property %s with string value failed. Error Message %s.",
                                                                           GetName().c_str(), ex.What().c_str());
         }

      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string ActorActorProperty::ToString() const
   {
      return GetValue() == NULL ? "" : GetValue()->GetId().ToString();
   }

   ////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* ActorActorProperty::GetRealActor()
   {
      return GetActorFunctor();
   }

   ////////////////////////////////////////////////////////////////////////////
   const dtCore::DeltaDrawable* ActorActorProperty::GetRealActor() const
   {
      return GetActorFunctor();
   }

   ////////////////////////////////////////////////////////////////////////////
   void ActorActorProperty::SetValue(ActorProxy* value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("SetValue has been called on a property that is read only.");
         return;
      }

      SetPropFunctor(value);
   }

   ////////////////////////////////////////////////////////////////////////////
   ActorProxy* ActorActorProperty::GetValue() const
   {
      return mProxy->GetLinkedActor(GetName());
   }

   ////////////////////////////////////////////////////////////////////////////
   bool GameEventActorProperty::FromString(const std::string& value)
   {
      GameEvent *event = GameEventManager::GetInstance().FindEvent(dtCore::UniqueId(value));
      if(event == NULL)
      {
         Map *map = Project::GetInstance().GetMapForActorProxy(*mProxy);
         if(map != NULL)
            event = map->GetEventManager().FindEvent(dtCore::UniqueId(value));
      }

      SetValue(event);
      return (event != NULL) ? true : false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string GameEventActorProperty::ToString() const
   {
      return GetValue() == NULL ? "" : GetValue()->GetUniqueId().ToString();
   }

   ////////////////////////////////////////////////////////////////////////////
   void ResourceActorProperty::SetValue(ResourceDescriptor* value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("SetValue has been called on a property that is read only.");
         return;
      }

      mProxy->SetResource(GetName(), value);
      if (value == NULL)
         SetPropFunctor("");
      else
      {
         try
         {
            std::string path = Project::GetInstance().GetResourcePath(*value);
            if (dtUtil::Log::GetInstance("EnginePropertyTypes.h").IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               dtUtil::Log::GetInstance("EnginePropertyTypes.h").LogMessage(dtUtil::Log::LOG_DEBUG,
                                                                            __FUNCTION__, __LINE__,
                                                                            "Path to resource is: %s",
                                                                            path.c_str());
            SetPropFunctor(path);
         }
         catch(const dtUtil::Exception& ex)
         {
            mProxy->SetResource(GetName(), NULL);
            SetPropFunctor("");
            dtUtil::Log::GetInstance("EnginePropertyTypes.h").LogMessage(dtUtil::Log::LOG_WARNING,
                                                                         __FUNCTION__, __LINE__, "Resource %s not found.  Setting property %s to NULL. Error Message %s.",
                                                                         value->GetResourceIdentifier().c_str(), GetName().c_str(), ex.What().c_str());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   ResourceDescriptor* ResourceActorProperty::GetValue() const
   {
      return mProxy->GetResource(GetName());
   }

   ////////////////////////////////////////////////////////////////////////////
   bool ResourceActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      bool result = true;
      if (value.empty() || value == "NULL")
      {
         SetValue(NULL);
      }
      else
      {
         std::vector<std::string> tokens;
         dtUtil::StringTokenizer<dtUtil::IsSlash> stok;

#ifdef _MSC_VER
         stok = stok; // Silence unsed variable warning in MSVC.
#endif

         stok.tokenize(tokens, value);

         std::string displayName;
         std::string identifier;

         if (tokens.size() == 2)
         {
            displayName = tokens[0];
            identifier = tokens[1];
         }
         else
         {
            //assume the value is a descriptor and use it for both the
            //data and the display name.
            displayName = tokens[0];
            identifier = tokens[0];
         }

         dtUtil::trim(identifier);
         dtUtil::trim(displayName);

         dtDAL::ResourceDescriptor descriptor(displayName, identifier);
         SetValue(&descriptor);

      }

      return result;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string ResourceActorProperty::ToString() const
   {
      ResourceDescriptor* r = GetValue();
      if (r == NULL)
         return "";
      else
         return r->GetDisplayName() + "/" + r->GetResourceIdentifier();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool IntActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      std::istringstream stream;
      stream.str(value);
      int i;
      stream >> i;
      SetValue(i);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string IntActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool BooleanActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      bool result = dtUtil::ToType<bool>(value);

      SetValue(result);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string BooleanActorProperty::ToString() const
   {
      const char* result = GetValue() ? "true" : "false";
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool FloatActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      std::istringstream stream;
      stream.precision(GetNumberPrecision());
      stream.str(value);
      float i;
      stream >> i;
      SetValue(i);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string FloatActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool DoubleActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      std::istringstream stream;
      stream.precision(GetNumberPrecision());
      stream.str(value);
      double i;
      stream >> i;
      SetValue(i);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string DoubleActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool LongActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      std::istringstream stream;
      stream.precision(GetNumberPrecision());
      stream.str(value);
      long i;
      stream >> i;
      SetValue(i);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string LongActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec2ActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec2 newValue;

      if (dtUtil::ParseVec<osg::Vec2>(value, newValue, 2))
      {
         SetValue(newValue);
         return true;
      }
      else return false;

   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec2ActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec2fActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec2f newValue;

      if (dtUtil::ParseVec<osg::Vec2f>(value, newValue, 2))
      {
         SetValue(newValue);
         return true;
      }
      else return false;

   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec2fActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec2dActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec2d newValue;

      if (dtUtil::ParseVec<osg::Vec2d>(value, newValue, 2))
      {
         SetValue(newValue);
         return true;
      }
      else return false;

   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec2dActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec3ActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec3 newValue;

      if (dtUtil::ParseVec<osg::Vec3>(value, newValue, 3))
      {
         SetValue(newValue);
         return true;
      }
      else return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec3ActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec3fActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec3f newValue;

      if (dtUtil::ParseVec<osg::Vec3f>(value, newValue, 3))
      {
         SetValue(newValue);
         return true;
      }
      else return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec3fActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec3dActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec3d newValue;

      if (dtUtil::ParseVec<osg::Vec3d>(value, newValue, 3))
      {
         SetValue(newValue);
         return true;
      }
      else return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec3dActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec4ActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec4 newValue;

      if (dtUtil::ParseVec<osg::Vec4>(value, newValue, 4))
      {
         SetValue(newValue);
         return true;
      }
      else return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec4ActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec4fActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec4f newValue;

      if (dtUtil::ParseVec<osg::Vec4f>(value, newValue, 4))
      {
         SetValue(newValue);
         return true;
      }
      else return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec4fActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec4dActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec4d newValue;

      if (dtUtil::ParseVec<osg::Vec4d>(value, newValue, 4))
      {
         SetValue(newValue);
         return true;
      }
      else return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec4dActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

}
