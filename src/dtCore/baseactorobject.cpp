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

#include <dtCore/actorfactory.h>
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
   const dtUtil::RefString BaseActorObject::PROPERTY_NAME("Name");
   const dtUtil::RefString BaseActorObject::PROPERTY_TYPE_CATEGORY("Type Category");
   const dtUtil::RefString BaseActorObject::PROPERTY_TYPE_NAME("Type Name");
   const dtUtil::RefString BaseActorObject::PROPERTY_CLASS_NAME("Class Name");
   const dtUtil::RefString BaseActorObject::PROPERTY_DESCRIPTION("Description");
   const dtUtil::RefString BaseActorObject::PROPERTY_ACTIVE("IsActive");
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   BaseActorObject::BaseActorObject()
   : mActorType(new ActorType(std::string()))
   {
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
   BaseActorObject* BaseActorObject::GetPrototype()
   {
      return mPrototype.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   const BaseActorObject* BaseActorObject::GetPrototype() const
   {
      return mPrototype.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetPrototype(BaseActorObject* proto)
   {
      mPrototype = proto;
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

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::CreateDrawable()
   {
      CreateActor();
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& BaseActorObject::GetClassName() const
   {
      return GetActorType().GetSharedClassInfo().GetClassName();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool BaseActorObject::IsInstanceOf(const dtUtil::RefString& clsname) const
   {
      return GetActorType().GetSharedClassInfo().IsInstanceOf(clsname);
   }


   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetClassName(const std::string& name)
   {
      if (!mActorType.valid())
      {
         // This sets a stub one so the code will work setting this in the constructor.
         SetActorType(*new ActorType(std::string()));
      }
      GetActorType().GetSharedClassInfo().SetClassName(name);
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
      std::for_each(GetActorType().GetSharedClassInfo().mClassHierarchy.begin(), GetActorType().GetSharedClassInfo().mClassHierarchy.end(), insertFunc);
      return hierarchy;
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& BaseActorObject::GetId() const
   {
      return mId;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetId(const dtCore::UniqueId& newId)
   {
      mId = newId;
      if (GetDrawable() != NULL)
      {
         GetDrawable()->SetUniqueId(newId);
      }
   }


   /////////////////////////////////////////////////////////////////////////////
   const std::string& BaseActorObject::GetName() const
   {
      return mName;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetName(const std::string& name)
   {
      mName = name;
      if (GetDrawable() != NULL)
      {
         GetDrawable()->SetName(name);
      }
      if (mBillBoardIcon.valid())
      {
         mBillBoardIcon->GetDrawable()->SetName(name);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool BaseActorObject::IsGhost() const
   {
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   static void CheckActorType(const dtCore::ActorType* actorType)
   {
      if (actorType == NULL)
      {
         throw dtUtil::Exception("The ActorType on an BaseActorObject is NULL.  The only way this could happen is "
                  "if the actor was created with the new operator rather than via "
                  "dtCore::ActorFactory::GetInstance().CreateActor().",
                  __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const ActorType& BaseActorObject::GetActorType() const
   {
      CheckActorType(mActorType.get());
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
//   static void CheckDrawable(const dtCore::DeltaDrawable* actor)
//   {
//      if (actor == NULL)
//      {
//         throw dtUtil::Exception("The Actor on an BaseActorObject is NULL.  The only ways this could happen is "
//                  "if the actor was created with the new operator rather than via "
//                  "dtCore::ActorFactory::GetInstance().CreateActor "
//                  "or the CreateActor method on the proxy subclass did not call SetActor() with a valid actor.",
//                  __FILE__, __LINE__);
//      }
//   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* BaseActorObject::GetDrawable()
   {
      //CheckDrawable(mDrawable.get());
      return mDrawable.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::DeltaDrawable* BaseActorObject::GetDrawable() const
   {
      //CheckDrawable(mDrawable.get());
      return mDrawable.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseActorObject::SetDrawable(dtCore::DeltaDrawable& drawable)
   {
      mDrawable = &drawable;
      mId = drawable.GetUniqueId();
      mName = drawable.GetName();
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
         copy = ActorFactory::GetInstance().CreateActor(*mActorType).get();
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
      if (mActorType.valid())
         type.MergeSharedClassInfo(mActorType->GetSharedClassInfo());
      mActorType = &type;
   }

   //////////////////////////////////////////////////////////////////////////////
   const ObjectType& BaseActorObject::GetObjectType() const { return *mActorType; }

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
      const std::string GROUP_INFORMATION("Taxonomy");
      const std::string GROUP_DRAWABLE("DeltaDrawable");

      if (!IsSystemComponent())
      {
         // Leave this off for now on components because dtEditQt does weird things with the names.
         StringActorProperty* nameProp = new StringActorProperty(
            PROPERTY_NAME, PROPERTY_NAME,
            StringActorProperty::SetFuncType(this, &BaseActorObject::SetName),
            StringActorProperty::GetFuncType(this, &BaseActorObject::GetName),
            "The Display Name of the Actor.", GROUP_INFORMATION);
         nameProp->SetMultipleEdit(false);
         nameProp->SetIgnoreWhenSaving(true);
         AddProperty(nameProp);
      }

      if (mActorType)
      {
         StringActorProperty* categoryProp = new StringActorProperty(
            PROPERTY_TYPE_CATEGORY, PROPERTY_TYPE_CATEGORY,
            StringActorProperty::SetFuncType(),
            StringActorProperty::GetFuncType(&GetActorType(), &ActorType::GetCategory),
            "The Category Name of the Actor.", GROUP_INFORMATION);
         categoryProp->SetReadOnly(true);
         AddProperty(categoryProp);

         StringActorProperty* typeProp = new StringActorProperty(
            PROPERTY_TYPE_NAME, PROPERTY_TYPE_NAME,
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

      dtCore::DeltaDrawable* drawable = GetDrawable();
      if (drawable != NULL)
      {
         AddProperty(new StringActorProperty(
                     BaseActorObject::PROPERTY_DESCRIPTION,
                     BaseActorObject::PROPERTY_DESCRIPTION,
                     StringActorProperty::SetFuncType(drawable, &dtCore::DeltaDrawable::SetDescription),
                     StringActorProperty::GetFuncType(drawable, &dtCore::DeltaDrawable::GetDescription),
                     "Generic text field used to describe this object",
                     GROUP_DRAWABLE));

         AddProperty(new dtCore::BooleanActorProperty(
                     BaseActorObject::PROPERTY_ACTIVE, BaseActorObject::PROPERTY_ACTIVE,
                     dtCore::BooleanActorProperty::SetFuncType(drawable, &dtCore::DeltaDrawable::SetActive),
                     dtCore::BooleanActorProperty::GetFuncType(drawable, &dtCore::DeltaDrawable::GetActive),
                     "Determines whether the drawable will render.", GROUP_DRAWABLE));
      }

      if (drawable != NULL)
      {
         static const dtUtil::RefString PROPERTY_SHADER_GROUP("ShaderGroup");
         static const dtUtil::RefString PROPERTY_SHADER_GROUP_DESC("Sets the shader group on the game actor.");
         static const dtUtil::RefString GROUPNAME("ShaderParams");

         AddProperty(new dtCore::StringActorProperty(PROPERTY_SHADER_GROUP, PROPERTY_SHADER_GROUP,
            dtCore::StringActorProperty::SetFuncType(drawable, &dtCore::DeltaDrawable::SetShaderGroup),
            dtCore::StringActorProperty::GetFuncType(drawable, &dtCore::DeltaDrawable::GetShaderGroup),
            PROPERTY_SHADER_GROUP_DESC,GROUPNAME));
      }
   }

} // namespace dtCore
