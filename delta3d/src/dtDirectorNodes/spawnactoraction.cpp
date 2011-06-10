/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
 * Author: Jeff P. Houde
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/spawnactoraction.h>

#include <dtABC/application.h>

#include <dtCore/scene.h>

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/stringselectoractorproperty.h>
#include <dtDAL/containeractorproperty.h>
#include <dtDAL/actortype.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/vectoractorproperties.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SpawnActorAction::SpawnActorAction()
      : ActionNode()
      , mActorType("")
      , mTemplateActor(NULL)
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   SpawnActorAction::~SpawnActorAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::StringSelectorActorProperty* typeProp = new dtDAL::StringSelectorActorProperty(
         "Actor Type", "Actor Type",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &SpawnActorAction::SetActorType),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &SpawnActorAction::GetActorType),
         dtDAL::StringSelectorActorProperty::GetListFuncType(this, &SpawnActorAction::GetActorTypeList),
         "The type of actor to spawn.");
      AddProperty(typeProp);

      dtDAL::Vec3ActorProperty* spawnPosProp = new dtDAL::Vec3ActorProperty(
         "Spawn Location", "Spawn Location",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &SpawnActorAction::SetSpawnLocation),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &SpawnActorAction::GetSpawnLocation),
         "The location to spawn the new actor.");
      AddProperty(spawnPosProp);

      mContainerProp = new dtDAL::ContainerActorProperty(
         "Actor Properties", "Actor Properties",
         "The properties of the new actor to be spawned.", "");
      AddProperty(mContainerProp);

      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Out Actor", "Out Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &SpawnActorAction::SetOutActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &SpawnActorAction::GetOutActor),
         "", "The actor that was spawned.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, typeProp, false, false, true, false));
      mValues.push_back(ValueLink(this, spawnPosProp, false, false, false));
      mValues.push_back(ValueLink(this, actorProp, true, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SpawnActorAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      osg::Vec3 spawnLocation = GetVec3("Spawn Location");
      osg::Vec3 spawnRotation;
      dtDAL::BaseActorObject* locationActor = GetActor("Spawn Location");
      if (locationActor)
      {
         dtDAL::Vec3ActorProperty* transProp =
            dynamic_cast<dtDAL::Vec3ActorProperty*>(locationActor->GetProperty("Translation"));
         dtDAL::Vec3ActorProperty* rotProp =
            dynamic_cast<dtDAL::Vec3ActorProperty*>(locationActor->GetProperty("Rotation"));

         if (transProp)
         {
            spawnLocation = transProp->GetValue();
         }
         if (rotProp)
         {
            spawnRotation = rotProp->GetValue();
         }
      }

      if (mTemplateActor.valid())
      {
         dtCore::RefPtr<dtDAL::BaseActorObject> proxy = mTemplateActor->Clone();

         if (proxy.valid())
         {
            dtDAL::Vec3ActorProperty* transProp =
               dynamic_cast<dtDAL::Vec3ActorProperty*>(proxy->GetProperty("Translation"));
            dtDAL::Vec3ActorProperty* rotProp =
               dynamic_cast<dtDAL::Vec3ActorProperty*>(proxy->GetProperty("Rotation"));

            if (transProp)
            {
               transProp->SetValue(spawnLocation);
            }
            if (rotProp)
            {
               rotProp->SetValue(spawnRotation);
            }

            dtGame::GameManager* gm = GetDirector()->GetGameManager();
            if (gm)
            {
               dtGame::GameActorProxy* gameProxy =
                  dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

               // If this is a game actor, make sure to add it to the GM as
               // one in order to make sure OnEnteredWorld gets invoked
               if (gameProxy)
               {
                  gm->AddActor(*gameProxy, false, false);
               }
               else
               {
                  gm->AddActor(*proxy);
               }
            }
            else
            {
               dtABC::Application* app = dtABC::Application::GetInstance(0);
               if (app)
               {
                  app->GetScene()->AddDrawable(proxy->GetActor());
               }
            }

            SetActorID(proxy->GetId(), "Out Actor");
            return ActionNode::Update(simDelta, delta, input, firstUpdate);
         }
      }

      ActivateOutput("Failed");
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool SpawnActorAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (Node::CanConnectValue(link, value))
      {
         if (link->GetName() == "Spawn Location")
         {
            if (value->CanBeType(dtDAL::DataType::ACTOR) ||
               value->CanBeType(dtDAL::DataType::VEC3))
            {
               return true;
            }
         }
         else
         {
            return true;
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::OnLinkValueChanged(const std::string& linkName)
   {
      if (linkName == "Actor Type")
      {
         UpdateTemplate();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::SetActorType(const std::string& value)
   {
      mActorType = value;

      UpdateTemplate();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::UpdateTemplate()
   {
      std::string actorType = GetString("Actor Type");
      std::string templateType;

      if (mTemplateActor.valid())
      {
         templateType = mTemplateActor->GetActorType().GetFullName();
      }

      if (!actorType.empty() && templateType != actorType)
      {
         std::string name;
         std::string category;

         std::string typeName = GetString("Actor Type");

         std::vector<const dtDAL::ActorType*> types;
         dtDAL::LibraryManager::GetInstance().GetActorTypes(types);
         int count = (int)types.size();
         for (int index = 0; index < count; ++index)
         {
            const dtDAL::ActorType* type = types[index];
            if (type)
            {
               if (type->GetFullName() == typeName)
               {
                  name = type->GetName();
                  category = type->GetCategory();
                  break;
               }
            }
         }

         if (!name.empty() && !category.empty())
         {
            if (mTemplateActor.valid())
            {
               mContainerProp->ClearProperties();
               mTemplateActor = NULL;
            }

            mTemplateActor = dtDAL::LibraryManager::GetInstance().CreateActor(category, name);
            if (mTemplateActor.valid())
            {
               std::vector<dtDAL::ActorProperty*> propList;
               mTemplateActor->GetPropertyList(propList);
               int count = (int)propList.size();
               for (int index = 0; index < count; ++index)
               {
                  dtDAL::ActorProperty* prop = propList[index];
                  if (prop)
                  {
                     mContainerProp->AddProperty(prop);
                  }
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string SpawnActorAction::GetActorType() const
   {
      return mActorType;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> SpawnActorAction::GetActorTypeList() const
   {
      std::vector<std::string> list;

      std::vector<const dtDAL::ActorType*> types;
      dtDAL::LibraryManager::GetInstance().GetActorTypes(types);
      int count = (int)types.size();
      for (int index = 0; index < count; ++index)
      {
         const dtDAL::ActorType* type = types[index];
         if (type)
         {
            list.push_back(type->GetFullName());
         }
      }

      return list;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::SetSpawnLocation(const osg::Vec3& value)
   {
      mSpawnLocation = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 SpawnActorAction::GetSpawnLocation() const
   {
      return mSpawnLocation;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::SetOutActor(const dtCore::UniqueId& value)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId SpawnActorAction::GetOutActor()
   {
      dtCore::UniqueId id;
      id = "";
      return id;
   }
}

////////////////////////////////////////////////////////////////////////////////
