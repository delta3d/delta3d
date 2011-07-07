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
#include <dtDirectorNodes/comparelessmutator.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CompareLessMutator::CompareLessMutator()
      : CompareMutator()
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   CompareLessMutator::~CompareLessMutator()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string CompareLessMutator::GetValueLabel()
   {
      return "&lt;";
   }

   //////////////////////////////////////////////////////////////////////////
   bool CompareLessMutator::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (CompareMutator::CanConnectValue(link, value))
      {
         if (link->GetName() == "A" || link->GetName() == "B")
         {
            if (value->CanBeType(dtDAL::DataType::BOOLEAN) ||
                value->CanBeType(dtDAL::DataType::INT)     ||
                value->CanBeType(dtDAL::DataType::FLOAT)   ||
                value->CanBeType(dtDAL::DataType::DOUBLE))
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
   bool CompareLessMutator::Compare(const osg::Vec4& left, const osg::Vec4& right)
   {
      return left.x() < right.x();
   }
}

////////////////////////////////////////////////////////////////////////////////
