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

#include <dtDAL/doubleactorproperty.h>

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
      dtDAL::DoubleActorProperty* epsilonProp = new dtDAL::DoubleActorProperty(
         "Epsilon", "Epsilon",
         dtDAL::DoubleActorProperty::SetFuncType(this, &CompareEqualityMutator::SetEpsilon),
         dtDAL::DoubleActorProperty::GetFuncType(this, &CompareEqualityMutator::GetEpsilon),
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
         dtDAL::DataType& type = value->GetPropertyType();
         switch (type.GetTypeId())
         {
         case dtDAL::DataType::STRING_ID:
         case dtDAL::DataType::BOOLEAN_ID:
         case dtDAL::DataType::INT_ID:
         case dtDAL::DataType::FLOAT_ID:
         case dtDAL::DataType::DOUBLE_ID:
         case dtDAL::DataType::VEC2F_ID:
         case dtDAL::DataType::VEC3F_ID:
         case dtDAL::DataType::VEC4F_ID:
         case dtDAL::DataType::UNKNOWN_ID:
            return true;

         default:
            return false;
         }
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
