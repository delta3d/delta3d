/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
#include <dtDirectorNodes/foreachactoraction.h>

#include <dtCore/stringselectoractorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/actorfactory.h>

#include <dtDirector/director.h>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   ForEachActorAction::ForEachActorAction()
      : ActionNode()
      , mClassType("<None>")
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   ForEachActorAction::~ForEachActorAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Each Actor", "Activates once for each actor found."));
      mOutputs.push_back(OutputLink(this, "Finished", "Activates after all actors have been iterated through."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::StringSelectorActorProperty* classProp = new dtCore::StringSelectorActorProperty(
         "Actor Type", "Actor Type",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &ForEachActorAction::SetClassType),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &ForEachActorAction::GetClassType),
         dtCore::StringSelectorActorProperty::GetListFuncType(&dtCore::ActorFactory::GetInstance(),
         &dtCore::ActorFactory::GetClassTypes),
         "The type of actor to iterate through (optional).", "", false);
      AddProperty(classProp);

      dtCore::StringActorProperty* filterProp = new dtCore::StringActorProperty(
         "Name Filter", "Name Filter",
         dtCore::StringActorProperty::SetFuncType(this, &ForEachActorAction::SetNameFilter),
         dtCore::StringActorProperty::GetFuncType(this, &ForEachActorAction::GetNameFilter),
         "The name to filter actors with (optional).");
      AddProperty(filterProp);

      mActorProp = new dtCore::ActorIDActorProperty(
         "Current Actor", "Current Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &ForEachActorAction::SetCurrentActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &ForEachActorAction::GetCurrentActor),
         "", "The currently iterated actor.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, classProp, false, false, true, false));
      mValues.push_back(ValueLink(this, filterProp, false, false, true, false));
      mValues.push_back(ValueLink(this, mActorProp, true, true, true, true));
   }

   //////////////////////////////////////////////////////////////////////////
   bool ForEachActorAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      // On the first update, find all the actors we want to iterate through.
      if (input == 0)
      {
         if (firstUpdate)
         {
            mFoundActors.clear();

            std::string classType = GetString("Actor Type");
            std::string nameFilter = GetString("Name Filter");

            int count = dtCore::BaseActorObject::GetInstanceCount();
            for (int index = 0; index < count; ++index)
            {
               dtCore::BaseActorObject* object = dtCore::BaseActorObject::GetInstance(index);
               if (object)
               {
                  if (!classType.empty() && classType != "<None>" &&
                      !object->GetActorType().InstanceOf(classType))
                  {
                     continue;
                  }

                  if (!nameFilter.empty())
                  {
                     std::string name = object->GetName();

                     // Make both strings lower case
                     std::transform(nameFilter.begin(), nameFilter.end(), nameFilter.begin(), ::tolower);
                     std::transform(name.begin(), name.end(), name.begin(), ::tolower);

                     if (name.find(nameFilter.c_str()) == std::string::npos)
                     {
                        continue;
                     }
                  }

                  dtCore::ActorProperty* prototypeProp = object->GetProperty("Initial Ownership");
                  dtCore::ActorProperty* ghostProp = object->GetProperty("Is Ghost");
                  if ((!prototypeProp || prototypeProp->ToString() != "PROTOTYPE") &&
                     (!ghostProp || ghostProp->ToString() == "false"))
                  {
                     mFoundActors.push_back(object->GetId());
                  }
               }
            }
         }

         // Iterate through each found actor.
         GetDirector()->PushStack(this, 11);

         for (int index = 0; index < (int)mFoundActors.size(); ++index)
         {
            // We push a stack so that we can execute the entire "For Each"
            // output chain before we return back to this node.
            GetDirector()->PushStack(this, 10);
         }

         return false;
      }
      // If the input is 10, it means we are iterating through an actor.
      else if (input == 10)
      {
         if (mFoundActors.size())
         {
            dtCore::UniqueId currentActor = mFoundActors.back();
            mFoundActors.pop_back();

            SetActorID(currentActor, "Current Actor");

            ActivateOutput("Each Actor");
         }
         return false;
      }
      else if (input == 11)
      {
         ActivateOutput("Finished");
         return false;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::UpdateName()
   {
      std::string classType = GetString("Actor Type");
      std::string nameFilter = GetString("Name Filter");

      mName.clear();

      if (!classType.empty())
      {
         mName += classType;
      }

      if (!nameFilter.empty())
      {
         if (!mName.empty())
         {
            mName += " ";
         }

         mName += "(" + nameFilter + ")";
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::SetClassType(const std::string& value)
   {
      mClassType = value;
      UpdateName();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& ForEachActorAction::GetClassType() const
   {
      return mClassType;
   }

   //////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::SetNameFilter(const std::string& value)
   {
      mNameFilter = value;
      UpdateName();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& ForEachActorAction::GetNameFilter() const
   {
      return mNameFilter;
   }

   //////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::SetCurrentActor(const dtCore::UniqueId& value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId ForEachActorAction::GetCurrentActor() const
   {
      dtCore::UniqueId id;
      id = "";
      return id;
   }
}

////////////////////////////////////////////////////////////////////////////////
