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
#include <prefix/dtcoreprefix.h>
#include <dtCore/baseactorobject.h>

#include <dtCore/scene.h>
#include <dtCore/uniqueid.h>

#include <dtCore/actorproperty.h>
#include <dtCore/actorproxyicon.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/datatype.h>

#include <dtCore/librarymanager.h>
#include <dtCore/project.h>
#include <dtCore/stringactorproperty.h>

#include <dtUtil/log.h>

#include <algorithm>
#include <sstream>

namespace dtCore
{
   IMPLEMENT_MANAGEMENT_LAYER(BaseActorObject)

   /////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(BaseActorObject::RenderMode);
   const BaseActorObject::RenderMode BaseActorObject::RenderMode::DRAW_ACTOR("DRAW_ACTOR");
   const BaseActorObject::RenderMode BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON("DRAW_BILLBOARD_ICON");
   const BaseActorObject::RenderMode BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON("DRAW_ACTOR_AND_BILLBOARD_ICON");
   const BaseActorObject::RenderMode BaseActorObject::RenderMode::DRAW_AUTO("DRAW_AUTO");
   const dtUtil::RefString BaseActorObject::DESCRIPTION_PROPERTY("Description");
   const dtUtil::RefString BaseActorObject::ACTIVE_PROPERTY("IsActive");
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   BaseActorObject::BaseActorObject()
   {
      SetClassName("dtCore::DeltaDrawable");

      RegisterInstance(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   BaseActorObject::BaseActorObject(const BaseActorObject& rhs)
   {
      RegisterInstance(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   BaseActorObject::~BaseActorObject()
   {
      DeregisterInstance(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   BaseActorObject& BaseActorObject::operator=(const BaseActorObject& rhs)
   {
      return *this;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::Init(const dtCore::ActorType& actorType)
   {
      SetActorType(actorType);
      CreateDrawable();
      // These are called to make it validate that they aren't Null
      // before proceeding.
      GetActorType();
      GetDrawable();
      BuildPropertyMap();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool BaseActorObject::ShouldPropertySave(const dtCore::ActorProperty& prop) const
   {
      bool result = PropertyContainer::ShouldPropertySave(prop);

      // Any properties in this group are saved elsewhere, therefore
      // we ignore these properties.
      if (prop.GetGroupName() == "Actor Information")
      {
         result = false;
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::CreateDrawable()
   {
      CreateActor();
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetClassName(const std::string& name)
   {
      mClassName = name;
      mClassNameSet.insert(mClassName);
   }

   /////////////////////////////////////////////////////////////////////////////
   class RefStringInsert
   {
   public:
      void operator()(const dtUtil::RefString& string)
      {
         toFill->insert(string.Get());
      }

      std::set<std::string>* toFill;
   };

   /////////////////////////////////////////////////////////////////////////////
   const std::set<std::string> BaseActorObject::GetClassHierarchy() const
   {
      RefStringInsert insertFunc;
      std::set<std::string> hierarchy;
      insertFunc.toFill = &hierarchy;
      std::for_each(mClassNameSet.begin(), mClassNameSet.end(), insertFunc);
      return hierarchy;
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& BaseActorObject::GetId() const
   {
      return GetDrawable()->GetUniqueId();
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetId(const dtCore::UniqueId& newId)
   {
      GetDrawable()->SetUniqueId(newId);
   }


   /////////////////////////////////////////////////////////////////////////////
   const std::string& BaseActorObject::GetName() const
   {
      return GetDrawable()->GetName();
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetName(const std::string& name)
   {
      GetDrawable()->SetName(name);
      if (mBillBoardIcon.valid())
      {
         mBillBoardIcon->GetDrawable()->SetName(name);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool BaseActorObject::IsGhostProxy() const
   {
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   ResourceDescriptor BaseActorObject::GetResource(const std::string& name)
   {
      ResourceMapType::iterator itor = mResourceMap.find(name);
      return itor != mResourceMap.end() ? itor->second : dtCore::ResourceDescriptor::NULL_RESOURCE;
   }

   /////////////////////////////////////////////////////////////////////////////
   const ResourceDescriptor BaseActorObject::GetResource(const std::string& name) const
   {
      ResourceMapType::const_iterator itor = mResourceMap.find(name);
      return itor != mResourceMap.end() ? itor->second : dtCore::ResourceDescriptor::NULL_RESOURCE;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetResource(const std::string& name, ResourceDescriptor* source)
   {
      DEPRECATE("void BaseActorObject::SetResource(const std::string&, ResourceDescriptor*)",
                "void BaseActorObject::SetResource(const std::string&, const ResourceDescriptor&)");

      if (source != NULL)
      {
         SetResource(name, *source);
      }
      else
      {
         SetResource(name, dtCore::ResourceDescriptor::NULL_RESOURCE);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetResource(const std::string& name, const ResourceDescriptor& source)
   {
      //TODO Should probably deprecate this functionality.  The ResourceDescripter value should
      //be stored in the Actor, not generically in the BaseActorObject.

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

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetLinkedActor(const std::string& name, BaseActorObject* newValue)
   {
   }


   /////////////////////////////////////////////////////////////////////////////
   static void CheckDrawableType(const dtCore::ActorType* actorType)
   {
      if (actorType == NULL)
      {
         throw dtUtil::Exception("The ActorType on an BaseActorObject is NULL.  The only way this could happen is "
                  "if the actor was created with the new operator rather than via "
                  "dtCore::LibraryManager::GetInstance().CreateActorProxy().",
                  __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const ActorType& BaseActorObject::GetActorType() const
   {
      CheckDrawableType(mActorType.get());
      return *mActorType;
   }

   /////////////////////////////////////////////////////////////////////////////
   const BaseActorObject::RenderMode& BaseActorObject::GetRenderMode()
   {
      return RenderMode::DRAW_ACTOR;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetBillBoardIcon(ActorProxyIcon* icon)
   {
      mBillBoardIcon = icon;
   }

   /////////////////////////////////////////////////////////////////////////////
   ActorProxyIcon* BaseActorObject::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new ActorProxyIcon(ActorProxyIcon::IMAGE_BILLBOARD_GENERIC);
      }

      return mBillBoardIcon.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   static void CheckDrawable(const dtCore::DeltaDrawable* actor)
   {
      if (actor == NULL)
      {
         throw dtUtil::Exception("The Actor on an BaseActorObject is NULL.  The only ways this could happen is "
                  "if the actor was created with the new operator rather than via "
                  "dtCore::LibraryManager::GetInstance().CreateActorProxy "
                  "or the CreateActor method on the proxy subclass did not call SetActor() with a valid actor.",
                  __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* BaseActorObject::GetDrawable()
   {
      CheckDrawable(mDrawable.get());
      return mDrawable.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::DeltaDrawable* BaseActorObject::GetDrawable() const
   {
      CheckDrawable(mDrawable.get());
      return mDrawable.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetDrawable(dtCore::DeltaDrawable& drawable)
   {
      mDrawable = &drawable;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseActorObject> BaseActorObject::Clone()
   {
      std::ostringstream error;

      //First tell the library manager to create a new mDrawable using this
      // actors actor type.
      dtCore::RefPtr<BaseActorObject> copy;

      try
      {
         copy = LibraryManager::GetInstance().CreateActor(*mActorType).get();
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

   //////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetActorType(const ActorType& type)
   {
      mActorType = &type;
   }

   //////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::OnRemove() const
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   bool BaseActorObject::IsInSTAGE() const
   {
      return dtCore::Project::GetInstance().GetEditMode();
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::OnMapLoadBegin()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::OnMapLoadEnd()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::BuildPropertyMap()
   {
      dtCore::DeltaDrawable* drawable = GetDrawable();

      const std::string GROUP_INFORMATION("Actor Information");
      const std::string GROUP_DRAWABLE("DeltaDrawable");

      StringActorProperty* nameProp = new StringActorProperty(
         "Actor Name", "Actor Name",
         StringActorProperty::SetFuncType(this, &BaseActorObject::SetName),
         StringActorProperty::GetFuncType(this, &BaseActorObject::GetName),
         "The Display Name of the Actor.", GROUP_INFORMATION);
      nameProp->SetMultipleEdit(false);
      AddProperty(nameProp);

      if (mActorType)
      {
         StringActorProperty* categoryProp = new StringActorProperty(
            "Actor Category", "Actor Category",
            StringActorProperty::SetFuncType(),
            StringActorProperty::GetFuncType(&GetActorType(), &ActorType::GetCategory),
            "The Category Name of the Actor.", GROUP_INFORMATION);
         categoryProp->SetReadOnly(true);
         AddProperty(categoryProp);

         StringActorProperty* typeProp = new StringActorProperty(
            "Actor Type", "Actor Type",
            StringActorProperty::SetFuncType(),
            StringActorProperty::GetFuncType(&GetActorType(), &ActorType::GetName),
            "The Type Name of the Actor.", GROUP_INFORMATION);
         typeProp->SetReadOnly(true);
         AddProperty(typeProp);
      }

      StringActorProperty* classProp = new StringActorProperty(
         "Actor Class", "Actor Class",
         StringActorProperty::SetFuncType(),
         StringActorProperty::GetFuncType(this, &BaseActorObject::GetClassName),
         "The Class Name of the Actor.", GROUP_INFORMATION);
      classProp->SetReadOnly(true);
      AddProperty(classProp);

      AddProperty(new StringActorProperty(
                  BaseActorObject::DESCRIPTION_PROPERTY.Get(),
                  "Description",
                  StringActorProperty::SetFuncType(drawable, &dtCore::DeltaDrawable::SetDescription),
                  StringActorProperty::GetFuncType(drawable, &dtCore::DeltaDrawable::GetDescription),
                  "Generic text field used to describe this object",
                  GROUP_DRAWABLE));

      AddProperty(new dtCore::BooleanActorProperty(
                  BaseActorObject::ACTIVE_PROPERTY.Get(), "IsActive",
                  dtCore::BooleanActorProperty::SetFuncType(drawable, &dtCore::DeltaDrawable::SetActive),
                  dtCore::BooleanActorProperty::GetFuncType(drawable, &dtCore::DeltaDrawable::GetActive),
                  "Determines whether the drawable will render.", GROUP_DRAWABLE));
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string BaseActorObject::GetDefaultPropertyKey() const
   {
      return std::string("Actor: ") + GetActorType().GetFullName();
   }
} // namespace dtCore
