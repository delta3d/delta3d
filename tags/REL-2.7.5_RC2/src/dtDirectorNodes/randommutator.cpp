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
#include <dtDirectorNodes/randommutator.h>
#include <dtUtil/mathdefines.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   RandomMutator::RandomMutator()
      : ArithmeticMutator()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   RandomMutator::~RandomMutator()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   std::string RandomMutator::GetValueLabel()
   {
      return "+";
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec4 RandomMutator::PerformOperation(const osg::Vec4& left, const osg::Vec4& right)
   {
      osg::Vec4 result;

      // Our randomization is different based on what type of value we are associated with.
      dtCore::DataType& leftType = Node::GetPropertyType("A");
      if (leftType == dtCore::DataType::INT)
      {
         for (int index = 0; index < 4; ++index)
         {
            if (left[index] < right[index])
            {
               result[index] = dtUtil::RandRange((int)left[index], (int)right[index]);
            }
            else
            {
               result[index] = left[index];
            }
         }
      }
      else
      {
         for (int index = 0; index < 4; ++index)
         {
            if (left[index] < right[index])
            {
               result[index] = dtUtil::RandFloat(left[index], right[index]);
            }
            else
            {
               result[index] = left[index];
            }
         }
      }

      return result;
   }
}

////////////////////////////////////////////////////////////////////////////////
