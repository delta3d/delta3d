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
#include <dtDirectorNodes/dividemutator.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   DivideMutator::DivideMutator()
      : ArithmeticMutator()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   DivideMutator::~DivideMutator()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   std::string DivideMutator::GetValueLabel()
   {
      return "/";
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec4 DivideMutator::PerformOperation(const osg::Vec4& left, const osg::Vec4& right)
   {
      osg::Vec4 result;
      result.x() = (right.x() > 0)? left.x() / right.x(): 0.0f;
      result.y() = (right.y() > 0)? left.y() / right.y(): 0.0f;
      result.z() = (right.z() > 0)? left.z() / right.z(): 0.0f;
      result.w() = (right.w() > 0)? left.w() / right.w(): 0.0f;
      return result;
   }
}

////////////////////////////////////////////////////////////////////////////////
