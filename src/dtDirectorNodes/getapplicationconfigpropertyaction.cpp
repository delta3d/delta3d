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
 * Author: Eric R. Heine
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/getapplicationconfigpropertyaction.h>

#include <dtABC/application.h>
#include <dtCore/stringactorproperty.h>
#include <dtDirector/director.h>
#include <dtGame/gamemanager.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   GetApplicationConfigPropertyAction::GetApplicationConfigPropertyAction()
      : ActionNode()
      , mPropertyName("")
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   GetApplicationConfigPropertyAction::~GetApplicationConfigPropertyAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetApplicationConfigPropertyAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::StringActorProperty* nameProp = new dtCore::StringActorProperty(
         "PropertyName", "Property Name",
         dtCore::StringActorProperty::SetFuncType(this, &GetApplicationConfigPropertyAction::SetPropertyName),
         dtCore::StringActorProperty::GetFuncType(this, &GetApplicationConfigPropertyAction::GetPropertyName),
         "The name of the config property to retrieve.");
      AddProperty(nameProp);

      mResultProp = new dtCore::StringActorProperty(
         "Result", "Result",
         dtCore::StringActorProperty::SetFuncType(this, &GetApplicationConfigPropertyAction::SetResult),
         dtCore::StringActorProperty::GetFuncType(this, &GetApplicationConfigPropertyAction::GetResult),
         "The current value of the property.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, mResultProp.get(), true, true, false));
      mValues.push_back(ValueLink(this, nameProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GetApplicationConfigPropertyAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      const dtABC::Application& app = GetDirector()->GetGameManager()->GetApplication();
      std::string propName = GetString("PropertyName");
      if (app.IsConfigPropertyDefined(propName))
      {
         std::string result = app.GetConfigPropertyValue(propName);
         SetValueNodeValue(result, "Result");
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetApplicationConfigPropertyAction::SetPropertyName(const std::string& value)
   {
      mPropertyName = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string GetApplicationConfigPropertyAction::GetPropertyName() const
   {
      return mPropertyName;
   }

   //////////////////////////////////////////////////////////////////////////
   void GetApplicationConfigPropertyAction::SetResult(const std::string& value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   std::string GetApplicationConfigPropertyAction::GetResult() const
   {
      return "";
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string& GetApplicationConfigPropertyAction::GetName()
   {
      return mPropertyName;
   }
}

////////////////////////////////////////////////////////////////////////////////
