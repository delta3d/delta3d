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
#include <prefix/dtdalprefix.h>
#include <dtDAL/actorproxy.h>

#include <dtCore/scene.h>
#include <dtCore/uniqueid.h>

#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxyicon.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/project.h>
#include <dtDAL/stringactorproperty.h>

#include <dtUtil/log.h>

#include <algorithm>
#include <sstream>

namespace dtDAL
{
   ///////////////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ActorProxy::RenderMode);
   const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_ACTOR("DRAW_ACTOR");
   const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_BILLBOARD_ICON("DRAW_BILLBOARD_ICON");
   const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON("DRAW_ACTOR_AND_BILLBOARD_ICON");
   const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_AUTO("DRAW_AUTO");
   const dtUtil::RefString ActorProxy::DESCRIPTION_PROPERTY("Description");
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
   void ActorProxy::Init(const dtDAL::ActorType& actorType)
   {
      SetActorType(actorType);
      CreateActor();
      // These are called to make it validate that they aren't Null
      // before proceeding.
      GetActorType();
      GetActor();
      BuildPropertyMap();
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
      return GetActor()->GetUniqueId();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetId(const dtCore::UniqueId& newId)
   {
      GetActor()->SetUniqueId(newId);
   }


   ///////////////////////////////////////////////////////////////////////////////////////
   const std::string& ActorProxy::GetName() const
   {
      return GetActor()->GetName();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetName(const std::string& name)
   {
      GetActor()->SetName(name);
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
   ResourceDescriptor ActorProxy::GetResource(const std::string& name)
   {
      ResourceMapType::iterator itor = mResourceMap.find(name);
      return itor != mResourceMap.end() ? itor->second : dtDAL::ResourceDescriptor::NULL_RESOURCE;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   const ResourceDescriptor ActorProxy::GetResource(const std::string& name) const
   {
      ResourceMapType::const_iterator itor = mResourceMap.find(name);
      return itor != mResourceMap.end() ? itor->second : dtDAL::ResourceDescriptor::NULL_RESOURCE;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetResource(const std::string& name, ResourceDescriptor* source)
   {
      DEPRECATE("void ActorProxy::SetResource(const std::string&, ResourceDescriptor*)",
                "void ActorProxy::SetResource(const std::string&, const ResourceDescriptor&)");

      if (source != NULL)
      {
         SetResource(name, *source);
      }
      else
      {
         SetResource(name, dtDAL::ResourceDescriptor::NULL_RESOURCE);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetResource(const std::string& name, const ResourceDescriptor& source)
   {
      //TODO Should probably deprecate this functionality.  The ResourceDescripter value should
      //be stored in the Actor, not generically in the ActorProxy.

      if (source.IsEmpty())
      {
         mResourceMap.erase(name);
      }
      else
      {
         //attempt to insert the value
         std::pair<ResourceMapType::iterator, bool> result = mResourceMap.insert(std::make_pair(name, source));
         // result.second tells me if it was inserted
         if (!result.second)
         {
            // if not, first of the result is the iterator to the map entry, so just change the map value.
            result.first->second = source;
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
   static void CheckActorType(const dtDAL::ActorType* actorType)
   {
      if (actorType == NULL)
      {
         throw dtUtil::Exception("The ActorType on an ActorProxy is NULL.  The only way this could happen is "
                  "if the actor was created with the new operator rather than via "
                  "dtDAL::LibraryManager::GetInstance().CreateActorProxy().",
                  __FILE__, __LINE__);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   const ActorType& ActorProxy::GetActorType() const
   {
      CheckActorType(mActorType.get());
      return *mActorType;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   const ActorProxy::RenderMode& ActorProxy::GetRenderMode()
   {
      return RenderMode::DRAW_ACTOR;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetBillBoardIcon(ActorProxyIcon* icon)
   {
      mBillBoardIcon = icon;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActorProxyIcon *ActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new ActorProxyIcon(ActorProxyIcon::IMAGE_BILLBOARD_GENERIC);
      }

      return mBillBoardIcon.get();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   static void CheckActor(const dtCore::DeltaDrawable* actor)
   {
      if (actor == NULL)
      {
         throw dtUtil::Exception("The Actor on an ActorProxy is NULL.  The only ways this could happen is "
                  "if the actor was created with the new operator rather than via "
                  "dtDAL::LibraryManager::GetInstance().CreateActorProxy "
                  "or the CreateActor method on the proxy subclass did not call SetActor() with a valid actor.",
                  __FILE__, __LINE__);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* ActorProxy::GetActor()
   {
      CheckActor(mActor.get());
      return mActor.get();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   const dtCore::DeltaDrawable* ActorProxy::GetActor() const
   {
      CheckActor(mActor.get());
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

      copy->CopyPropertiesFrom(*this);

      return copy;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::SetActorType(const ActorType& type)
   {
      mActorType = &type;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::OnRemove() const
   {

   }

   //////////////////////////////////////////////////////////////////////////
   const bool ActorProxy::IsInSTAGE() const
   {
      return dtDAL::Project::GetInstance().GetEditMode();
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorProxy::OnMapLoadBegin()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorProxy::OnMapLoadEnd()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorProxy::BuildPropertyMap()
   {
      dtCore::DeltaDrawable* actor = GetActor();

      const std::string GROUP_DRAWABLE("DeltaDrawable");

      AddProperty(new StringActorProperty(
                  ActorProxy::DESCRIPTION_PROPERTY.Get(),
                  "Description",
                  StringActorProperty::SetFuncType(actor, &dtCore::DeltaDrawable::SetDescription),
                  StringActorProperty::GetFuncType(actor, &dtCore::DeltaDrawable::GetDescription),
                  "Generic text field used to describe this object",
                  GROUP_DRAWABLE));

      AddProperty(new dtDAL::BooleanActorProperty(
                  "IsActive", "IsActive",
                  dtDAL::BooleanActorProperty::SetFuncType(actor, &dtCore::DeltaDrawable::SetActive),
                  dtDAL::BooleanActorProperty::GetFuncType(actor, &dtCore::DeltaDrawable::GetActive),
                  "Determines whether the drawable will render.", GROUP_DRAWABLE));
   }
}
