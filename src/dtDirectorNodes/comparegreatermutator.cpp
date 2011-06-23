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
#include <dtDirectorNodes/comparegreatermutator.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CompareGreaterMutator::CompareGreaterMutator()
      : CompareMutator()
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   CompareGreaterMutator::~CompareGreaterMutator()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string CompareGreaterMutator::GetValueLabel()
   {
      return ">";
   }

   //////////////////////////////////////////////////////////////////////////
   bool CompareGreaterMutator::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (CompareMutator::CanConnectValue(link, value))
      {
         dtDAL::DataType& type = value->GetPropertyType();
         switch (type.GetTypeId())
         {
         case dtDAL::DataType::BOOLEAN_ID:
         case dtDAL::DataType::INT_ID:
         case dtDAL::DataType::FLOAT_ID:
         case dtDAL::DataType::DOUBLE_ID:
         case dtDAL::DataType::UNKNOWN_ID:
            return true;

         default:
            return false;
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CompareGreaterMutator::Compare(const osg::Vec4& left, const osg::Vec4& right)
   {
      return left.x() > right.x();
   }
}

////////////////////////////////////////////////////////////////////////////////
