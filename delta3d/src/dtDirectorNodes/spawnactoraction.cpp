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

#include <dtCore/actoridactorproperty.h>
#include <dtCore/stringselectoractorproperty.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/actortype.h>
#include <dtCore/librarymanager.h>
#include <dtCore/vectoractorproperties.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SpawnActorAction::SpawnActorAction()
      : ActionNode()
      , mActorType("")
      , mTemplateActor(NULL)
      , mContainerProp(NULL)
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

      mOutputs.push_back(OutputLink(this, "Failed", "Activated if the actor has failed to spawn."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::StringSelectorActorProperty* typeProp = new dtCore::StringSelectorActorProperty(
         "Actor Type", "Actor Type",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &SpawnActorAction::SetActorType),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &SpawnActorAction::GetActorType),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &SpawnActorAction::GetActorTypeList),
         "The type of the actor to spawn.");
      AddProperty(typeProp);

      mNameProp = new dtCore::StringActorProperty(
         "Name", "Name",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &SpawnActorAction::SetActorName),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &SpawnActorAction::GetActorName),
         "Actor name.");

      mGhostProp = new dtCore::BooleanActorProperty(
         "Is Ghost", "Is Ghost",
         dtCore::BooleanActorProperty::SetFuncType(this, &SpawnActorAction::SetGhost),
         dtCore::BooleanActorProperty::GetFuncType(this, &SpawnActorAction::GetGhost),
         "Is this proxy a ghost.");

      dtCore::Vec3ActorProperty* spawnPosProp = new dtCore::Vec3ActorProperty(
         "Spawn Location", "Spawn Location",
         dtCore::Vec3ActorProperty::SetFuncType(this, &SpawnActorAction::SetSpawnLocation),
         dtCore::Vec3ActorProperty::GetFuncType(this, &SpawnActorAction::GetSpawnLocation),
         "The location to spawn the new actor.");
      AddProperty(spawnPosProp);

      mContainerProp = new dtCore::ContainerActorProperty(
         "Actor Properties", "Actor Properties",
         "The properties of the new actor to be spawned.", "");
      AddProperty(mContainerProp);

      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Out Actor", "Out Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &SpawnActorAction::SetOutActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &SpawnActorAction::GetOutActor),
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
      dtCore::BaseActorObject* locationActor = GetActor("Spawn Location");
      if (locationActor)
      {
         dtCore::Vec3ActorProperty* transProp =
            dynamic_cast<dtCore::Vec3ActorProperty*>(locationActor->GetProperty("Translation"));
         dtCore::Vec3ActorProperty* rotProp =
            dynamic_cast<dtCore::Vec3ActorProperty*>(locationActor->GetProperty("Rotation"));

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
         dtCore::RefPtr<dtCore::BaseActorObject> proxy = mTemplateActor->Clone();

         if (proxy.valid())
         {
            dtCore::Vec3ActorProperty* transProp =
               dynamic_cast<dtCore::Vec3ActorProperty*>(proxy->GetProperty("Translation"));
            dtCore::Vec3ActorProperty* rotProp =
               dynamic_cast<dtCore::Vec3ActorProperty*>(proxy->GetProperty("Rotation"));

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
      if (ActionNode::CanConnectValue(link, value))
      {
         if (link->GetName() == "Spawn Location")
         {
            if (value->CanBeType(dtCore::DataType::ACTOR) ||
                value->CanBeType(dtCore::DataType::VEC3))
            {
               return true;
            }
            return false;
         }
         return true;
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::OnLinkValueChanged(const std::string& linkName)
   {
      ActionNode::OnLinkValueChanged(linkName);

      if (linkName == "Actor Type")
      {
         UpdateTemplate();
      }
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

         std::vector<const dtCore::ActorType*> types;
         dtCore::LibraryManager::GetInstance().GetActorTypes(types);
         int count = (int)types.size();
         for (int index = 0; index < count; ++index)
         {
            const dtCore::ActorType* type = types[index];
            if (type)
            {
               if (type->GetFullName() == actorType)
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
               mName = "";
            }

            mTemplateActor = dtCore::LibraryManager::GetInstance().CreateActor(category, name);
            if (mTemplateActor.valid())
            {
               mContainerProp->AddProperty(mNameProp);

               std::vector<dtCore::ActorProperty*> propList;
               mTemplateActor->GetPropertyList(propList);
               int count = (int)propList.size();
               for (int index = 0; index < count; ++index)
               {
                  dtCore::ActorProperty* prop = propList[index];
                  if (prop)
                  {
                     mContainerProp->AddProperty(prop);
                  }
               }

               mTemplateActor->AddProperty(mGhostProp);
               mName = mTemplateActor->GetActorType().GetFullName();
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::SetActorType(const std::string& value)
   {
      mActorType = value;

      UpdateTemplate();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string SpawnActorAction::GetActorType() const
   {
      return mActorType;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> SpawnActorAction::GetActorTypeList() const
   {
      std::map<std::string, std::string> listMap;

      std::vector<const dtCore::ActorType*> types;
      dtCore::LibraryManager::GetInstance().GetActorTypes(types);
      int count = (int)types.size();
      for (int index = 0; index < count; ++index)
      {
         const dtCore::ActorType* type = types[index];
         if (type)
         {
            listMap[type->GetFullName()] = "x";
         }
      }

      std::vector<std::string> list;
      std::map<std::string, std::string>::iterator iter;
      for (iter = listMap.begin(); iter != listMap.end(); ++iter)
      {
         list.push_back(iter->first);
      }

      return list;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::SetActorName(const std::string& value)
   {
      if (mTemplateActor.valid())
      {
         mTemplateActor->SetName(value);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string SpawnActorAction::GetActorName() const
   {
      if (mTemplateActor.valid())
      {
         return mTemplateActor->GetName();
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnActorAction::SetGhost(bool value)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SpawnActorAction::GetGhost() const
   {
      return true;
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
