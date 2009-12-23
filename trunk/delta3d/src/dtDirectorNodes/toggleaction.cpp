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

#include <dtDirectorNodes/toggleaction.h>

#include <dtDirector/director.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtDAL/enginepropertytypes.h>


namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   ToggleAction::ToggleAction()
      : ActionNode()  
   {
      mToggleActor = "";
      mTogglePropertyName = "IsActive";

      AddAuthor("Michael Guerrero");
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
      mInputs.push_back(InputLink(this, "TurnOn"));
      mInputs.push_back(InputLink(this, "TurnOff"));
      mInputs.push_back(InputLink(this, "Toggle"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void ToggleAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &ToggleAction::SetToggleActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &ToggleAction::GetToggleActor),
         "dtCore::Transformable", "The actor to with a property to toggle.");
      AddProperty(actorProp);

      dtDAL::StringActorProperty* toggleActorProp = new dtDAL::StringActorProperty(
         "TogglePropertyName", "Toggle Property Name",
         dtDAL::StringActorProperty::SetFuncType(this, &ToggleAction::SetTogglePropertyName),
         dtDAL::StringActorProperty::GetFuncType(this, &ToggleAction::GetTogglePropertyName),
         "The name of the boolean actor property to toggle.");
      AddProperty(toggleActorProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp, true, true, true));      
   }

   /////////////////////////////////////////////////////////////////////////////
   bool ToggleAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      bool onOrOff = false;

      switch (input)
      {
      case INPUT_TURN_ON:
         {
            // Set to true and continue without break
            onOrOff = true;
         }
      case INPUT_TURN_OFF:
         {
            int count = GetPropertyCount("Actor");
            for (int index = 0; index < count; index++)
            {
               dtDAL::ActorProxy* proxy = GetActor("Actor", index);
               if (proxy)
               {
                  std::string togglePropertyString = GetString("TogglePropertyName");

                  dtDAL::BooleanActorProperty* toggleProperty;
                  proxy->GetProperty(togglePropertyString, toggleProperty);

                  if (toggleProperty)
                  {
                     toggleProperty->SetValue(onOrOff);
                  }
               }
            }
         }
         break;

      case INPUT_TOGGLE:
         {
            // find out what the value is and then negate it
         }
         break;
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
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
}

////////////////////////////////////////////////////////////////////////////////
