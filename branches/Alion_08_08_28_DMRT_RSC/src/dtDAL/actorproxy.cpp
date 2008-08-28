/*
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
 */
#include <prefix/dtdalprefix-src.h>
#include <dtDAL/actorproxy.h>
#include <dtUtil/log.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/actorproxyicon.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/project.h>
#include <dtCore/scene.h>
#include <dtCore/uniqueid.h>
#include <sstream>
#include <algorithm>

namespace dtDAL
{
   ///////////////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ActorProxy::RenderMode);
   const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_ACTOR("DRAW_ACTOR");
   const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_BILLBOARD_ICON("DRAW_BILLBOARD_ICON");
   const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON("DRAW_ACTOR_AND_BILLBOARD_ICON");
   const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_AUTO("DRAW_AUTO");
   ///////////////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////////////
   ActorProxy::ActorProxy()
   {
      SetClassName("dtCore::DeltaDrawable");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActorProxy::ActorProxy(const ActorProxy& rhs)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActorProxy::~ActorProxy()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActorProxy& ActorProxy::operator=(const ActorProxy& rhs)
   {
      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetClassName(const std::string& name)
   {
      mClassName = name;
      mClassNameSet.insert(mClassName);
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   class RefStringInsert
   {
      public:
         void operator()(const dtUtil::RefString& string)
         {
            toFill->insert(string.Get());
         }

         std::set<std::string>* toFill;
   };
   
   ///////////////////////////////////////////////////////////////////////////////////////
   const std::set<std::string> ActorProxy::GetClassHierarchy() const 
   { 
      RefStringInsert insertFunc;
      std::set<std::string> hierarchy;
      insertFunc.toFill = &hierarchy;
      std::for_each(mClassNameSet.begin(), mClassNameSet.end(), insertFunc);
      return hierarchy;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& ActorProxy::GetId() const
   {
      return GetActor() ? GetActor()->GetUniqueId() : dtCore::UniqueId();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetId(const dtCore::UniqueId& newId)
   {
      if (GetActor())
      {
         GetActor()->SetUniqueId(newId);
      }
   }


   ///////////////////////////////////////////////////////////////////////////////////////
   const std::string& ActorProxy::GetName() const
   {
      return GetActor() ? GetActor()->GetName() : "";
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetName(const std::string& name)
   {
      if (GetActor())
      {
         GetActor()->SetName(name);
      }
      if (mBillBoardIcon.valid())
      {
         mBillBoardIcon->GetDrawable()->SetName(name);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   bool ActorProxy::IsGhostProxy() const
   {
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ResourceDescriptor* ActorProxy::GetResource(const std::string &name)
   {
      ResourceMapType::iterator itor = mResourceMap.find(name);
      return itor != mResourceMap.end() ? &itor->second : NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   const ResourceDescriptor* ActorProxy::GetResource(const std::string &name) const
   {
      ResourceMapType::const_iterator itor = mResourceMap.find(name);
      return itor != mResourceMap.end() ? &itor->second : NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetResource(const std::string &name, ResourceDescriptor *source)
   {
      if (source == NULL)
      {
         mResourceMap.erase(name);
      }
      else
      {
         //attempt to insert the value
         std::pair<ResourceMapType::iterator, bool> result = mResourceMap.insert(std::make_pair(name, *source));
         // result.second tells me if it was inserted
         if (!result.second)
         {
            // if not, first of the result is the iterator to the map entry, so just change the map value.
            result.first->second = *source;
         }
      }
   }
    
    
   ///////////////////////////////////////////////////////////////////////////////////////
   const ActorProxy* ActorProxy::GetLinkedActor(const std::string& name) const
   {
      ActorProxyMapType::const_iterator itor = mActorProxyMap.find(name);
      return itor != mActorProxyMap.end() ? itor->second.get() : NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActorProxy* ActorProxy::GetLinkedActor(const std::string& name)
   {
      ActorProxyMapType::iterator itor = mActorProxyMap.find(name);
      return itor != mActorProxyMap.end() ? itor->second.get() : NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetLinkedActor(const std::string& name, ActorProxy* newValue)
   {
      if (newValue == NULL)
      {
         mActorProxyMap.erase(name);
      }
      else
      {
         //attempt to insert the value
         std::pair<ActorProxyMapType::iterator, bool> result = 
            mActorProxyMap.insert(std::make_pair(name, newValue));
         // result.second tells me if it was inserted
         if (!result.second)
         {
            // if not, first of the result is the iterator to the map entry, so just change the map value.
            result.first->second = newValue;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::AddProperty(ActorProperty *newProp)
   {
      if(newProp == NULL)
      {
         throw dtUtil::Exception(ExceptionEnum::InvalidParameter, 
            "AddProperty cannot add a NULL property", __FILE__, __LINE__);
      }

      PropertyMapType::iterator itor =
         mPropertyMap.find(newProp->GetName());
      if(itor != mPropertyMap.end())
      {
         std::ostringstream ss;
         ss << "Could not add new property " << newProp->GetName() << " because "
            << "a property with that name already exists.";
         LOG_ERROR(ss.str());
      }
      else
      {
         mPropertyMap.insert(std::make_pair(dtUtil::RefString(newProp->GetName()),newProp));
         mProperties.push_back(newProp);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::RemoveProperty(const std::string& nameToRemove)
   {
      PropertyMapType::iterator itor =
         mPropertyMap.find(nameToRemove);
      if (itor != mPropertyMap.end())
      {
         mPropertyMap.erase(itor);
         for (size_t i = 0; i < mProperties.size(); ++i)
         {
            if (mProperties[i]->GetName() == nameToRemove)
            {
               mProperties.erase(mProperties.begin() + i);
               break;
            }
         }
      }
      else
      {
         std::ostringstream msg;
         msg << "Could not find property " << nameToRemove << " to remove. Reason was: " << "was not found in mPropertyMap";
         LOG_DEBUG(msg.str());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActorProperty* ActorProxy::GetProperty(const std::string &name)
   {
      PropertyMapType::iterator itor =
         mPropertyMap.find(name);

      if(itor == mPropertyMap.end())
         return NULL;
      else
         return itor->second.get();
   }
    
   ///////////////////////////////////////////////////////////////////////////////////////
   const ActorProperty* ActorProxy::GetProperty(const std::string &name) const
   {
      PropertyMapType::const_iterator itor =
         mPropertyMap.find(name);

      if(itor == mPropertyMap.end())
         return NULL;
      else
         return itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::GetPropertyList(std::vector<const ActorProperty *> &propList) const
   {
      propList.clear();
      propList.reserve(mProperties.size());

      for (size_t i = 0; i < mProperties.size(); ++i)
      {
         propList.push_back(mProperties[i].get());
      }
   }


   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::GetPropertyList(std::vector<ActorProperty *> &propList)
   {
      propList.clear();
      propList.reserve(mProperties.size());

      for (size_t i = 0; i < mProperties.size(); ++i)
      {
         propList.push_back(mProperties[i].get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   const ActorProxy::RenderMode& ActorProxy::GetRenderMode()
   {
      return RenderMode::DRAW_ACTOR;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetBillBoardIcon(ActorProxyIcon *icon)
   {
      mBillBoardIcon = icon;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActorProxyIcon *ActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new ActorProxyIcon();
      }

      return mBillBoardIcon.get();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* ActorProxy::GetActor()
   {
      return mActor.get();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   const dtCore::DeltaDrawable* ActorProxy::GetActor() const
   {
      return mActor.get();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetActor(dtCore::DeltaDrawable &actor)
   {
      mActor = &actor;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ActorProxy> ActorProxy::Clone() 
   {
      std::ostringstream error;

      //First tell the library manager to create a new mActor using this
      // actors actor type.
      dtCore::RefPtr<ActorProxy> copy;

      try
      {
         copy = LibraryManager::GetInstance().CreateActorProxy(*mActorType).get();
      }
      catch(const dtUtil::Exception &e)
      {
         error << "Clone of actor proxy: " << GetName() << " failed. Reason was: " << e.What();
         LOG_ERROR(error.str());
         return NULL;
      }

      //The names should be the same until
      //the user changes them.
      copy->SetName(GetName());

      //Now copy all of the properties from this proxy to the clone.
      for (size_t i = 0; i < mProperties.size(); ++i)
      {
         if (!mProperties[i]->IsReadOnly())
            copy->mProperties[i]->CopyFrom(*mProperties[i]);
      }

      return copy;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetActorType(const ActorType &type)
   {
      mActorType = &type;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::OnRemove() const
   {

   }

   const bool ActorProxy::IsInSTAGE() const
   { 
      return dtDAL::Project::GetInstance().GetEditMode();
   }
}
