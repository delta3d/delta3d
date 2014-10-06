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
 * Author: MG
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/toggleaction.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/stringactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   ToggleAction::ToggleAction()
      : ActionNode()
   {
      mToggleActor = "";
      mTogglePropertyName = "IsActive";
      mToggleBool = false;

      AddAuthor("Michael Guerrero");
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   ToggleAction::~ToggleAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void ToggleAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "TurnOn", "Turns an actor or boolean value on."));
      mInputs.push_back(InputLink(this, "TurnOff", "Turns an actor or boolean value off."));
      mInputs.push_back(InputLink(this, "Toggle", "Toggles the status of an actors property or a boolean value."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void ToggleAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Actor", "Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &ToggleAction::SetToggleActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &ToggleAction::GetToggleActor),
         "dtCore::Transformable", "The actor with a property to toggle.");
      AddProperty(actorProp);

      dtCore::StringActorProperty* toggleActorProp = new dtCore::StringActorProperty(
         "TogglePropertyName", "Toggle Property Name",
         dtCore::StringActorProperty::SetFuncType(this, &ToggleAction::SetTogglePropertyName),
         dtCore::StringActorProperty::GetFuncType(this, &ToggleAction::GetTogglePropertyName),
         "The name of the boolean actor property to toggle.");
      AddProperty(toggleActorProp);

      dtCore::BooleanActorProperty* toggleBoolean = new dtCore::BooleanActorProperty(
         "ToggleBoolean", "Toggle Boolean",
         dtCore::BooleanActorProperty::SetFuncType(this, &ToggleAction::SetToggleBoolean),
         dtCore::BooleanActorProperty::GetFuncType(this, &ToggleAction::GetToggleBoolean),
         "A boolean value to toggle on and off with this action.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp, true, true, true));
      mValues.push_back(ValueLink(this, toggleActorProp, false, false, true, false));
      mValues.push_back(ValueLink(this, toggleBoolean, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool ToggleAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      bool onOrOff = false;
      bool shouldToggle = false;

      int count = GetPropertyCount("Actor");

      switch (input)
      {
      case INPUT_TOGGLE:
         {
            // Set to true and continue without break
            shouldToggle = true;
         }
      case INPUT_TURN_ON:
         {
            if (!shouldToggle)
            {
               // Set to true and continue without break
               onOrOff = true;
            }
         }
      case INPUT_TURN_OFF:
         {
            // Toggle our attached actor property
            for (int index = 0; index < count; index++)
            {
               dtCore::BaseActorObject* proxy = GetActor("Actor", index);
               if (proxy)
               {
                  std::string togglePropertyString = GetString("TogglePropertyName");

                  dtCore::BooleanActorProperty* toggleProperty;
                  proxy->GetProperty(togglePropertyString, toggleProperty);

                  if (toggleProperty)
                  {
                     // If this is a toggle negate the current value
                     if (shouldToggle)
                     {
                        onOrOff = !toggleProperty->GetValue();
                     }

                     toggleProperty->SetValue(onOrOff);
                  }
               }
            }

            // Toggle our attached boolean
            if (shouldToggle)
            {
               onOrOff = !GetBoolean("ToggleBoolean");
            }
            SetBoolean(onOrOff, "ToggleBoolean");
         }
         break;
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ToggleAction::OnLinkValueChanged(const std::string& linkName)
   {
      ActionNode::OnLinkValueChanged(linkName);

      if (linkName == "Actor")
      {
         std::string name;
         if (!GetActorID("Actor").ToString().empty() || GetValueNodeCount("Actor") > 0)
         {
            std::string togglePropertyString = GetString("TogglePropertyName");

            if (!togglePropertyString.empty())
            {
               name = "Property: " + togglePropertyString;
            }
         }
         SetName(name);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ToggleAction::SetToggleActor(const dtCore::UniqueId& value)
   {
      mToggleActor = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId ToggleAction::GetToggleActor()
   {
      return mToggleActor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ToggleAction::SetTogglePropertyName(const std::string& value)
   {
      mTogglePropertyName = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string ToggleAction::GetTogglePropertyName() const
   {
      return mTogglePropertyName;
   }

   //////////////////////////////////////////////////////////////////////////
   void ToggleAction::SetToggleBoolean(bool value)
   {
      mToggleBool = value;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ToggleAction::GetToggleBoolean() const
   {
      return mToggleBool;
   }
}

////////////////////////////////////////////////////////////////////////////////
