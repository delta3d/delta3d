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
#include <dtDirectorNodes/compareequalitymutator.h>

#include <dtCore/doubleactorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CompareEqualityMutator::CompareEqualityMutator()
      : CompareMutator()
      , mEpsilon(0.01)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   CompareEqualityMutator::~CompareEqualityMutator()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareEqualityMutator::BuildPropertyMap()
   {
      CompareMutator::BuildPropertyMap();

      // Create our value links.
      dtCore::DoubleActorProperty* epsilonProp = new dtCore::DoubleActorProperty(
         "Epsilon", "Epsilon",
         dtCore::DoubleActorProperty::SetFuncType(this, &CompareEqualityMutator::SetEpsilon),
         dtCore::DoubleActorProperty::GetFuncType(this, &CompareEqualityMutator::GetEpsilon),
         "Sets the amount of floating point error to ignore.");
      AddProperty(epsilonProp);
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string CompareEqualityMutator::GetValueLabel()
   {
      return "=";
   }

   //////////////////////////////////////////////////////////////////////////
   bool CompareEqualityMutator::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (CompareMutator::CanConnectValue(link, value))
      {
         if (link->GetName() == "A" || link->GetName() == "B")
         {
            if (value->CanBeType(dtCore::DataType::STRING)  ||
                value->CanBeType(dtCore::DataType::BOOLEAN) ||
                value->CanBeType(dtCore::DataType::INT)     ||
                value->CanBeType(dtCore::DataType::FLOAT)   ||
                value->CanBeType(dtCore::DataType::DOUBLE)  ||
                value->CanBeType(dtCore::DataType::VEC2F)   ||
                value->CanBeType(dtCore::DataType::VEC3F)   ||
                value->CanBeType(dtCore::DataType::VEC4F))
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
   bool CompareEqualityMutator::Compare(const osg::Vec4& left, const osg::Vec4& right)
   {
      for (int index = 0; index < 4; ++index)
      {
         if (fabs(left[index] - right[index]) > mEpsilon)
         {
            return false;
         }
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CompareEqualityMutator::Compare(const std::string& left, const std::string& right)
   {
      return left == right;
   }
}

////////////////////////////////////////////////////////////////////////////////
