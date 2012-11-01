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
#include <dtDirectorNodes/getrandomvalueaction.h>

#include <dtCore/doubleactorproperty.h>
#include <dtUtil/mathdefines.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   GetRandomValueAction::GetRandomValueAction()
      : ActionNode()
      , mMin(0.0)
      , mMax(1.0)
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   GetRandomValueAction::~GetRandomValueAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetRandomValueAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::DoubleActorProperty* minProp = new dtCore::DoubleActorProperty(
         "Min", "Min",
         dtCore::DoubleActorProperty::SetFuncType(this, &GetRandomValueAction::SetMin),
         dtCore::DoubleActorProperty::GetFuncType(this, &GetRandomValueAction::GetMin),
         "The low end of the random value range.");
      AddProperty(minProp);

      dtCore::DoubleActorProperty* maxProp = new dtCore::DoubleActorProperty(
         "Max", "Max",
         dtCore::DoubleActorProperty::SetFuncType(this, &GetRandomValueAction::SetMax),
         dtCore::DoubleActorProperty::GetFuncType(this, &GetRandomValueAction::GetMax),
         "The high end of the random value range.");
      AddProperty(maxProp);

      mpValueProp = new dtCore::DoubleActorProperty(
         "Value", "Value",
         dtCore::DoubleActorProperty::SetFuncType(this, &GetRandomValueAction::SetValue),
         dtCore::DoubleActorProperty::GetFuncType(this, &GetRandomValueAction::GetValue),
         "The randomly chosen value.");

      mValues.push_back(ValueLink(this, minProp, false, false, false));
      mValues.push_back(ValueLink(this, maxProp, false, false, false));
      mValues.push_back(ValueLink(this, mpValueProp.get(), true, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GetRandomValueAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      double min = GetDouble("Min");
      double max = GetDouble("Max");
      SetDouble(dtUtil::RandFloat(min, max), "Value");

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GetRandomValueAction::SetMin(double value)
   {
      mMin = value;
   }

   ///////////////////////////////////////////////////////////////////////////////
   double GetRandomValueAction::GetMin() const
   {
      return mMin;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GetRandomValueAction::SetMax(double value)
   {
      mMax = value;
   }

   ///////////////////////////////////////////////////////////////////////////////
   double GetRandomValueAction::GetMax() const
   {
      return mMax;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GetRandomValueAction::SetValue(double value)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   double GetRandomValueAction::GetValue() const
   {
      return 0.0;
   }

   /////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
