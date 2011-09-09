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

#ifndef SUBTRACT_MUTATOR_NODE
#define SUBTRACT_MUTATOR_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirectorNodes/arithmeticmutator.h>
#include <dtDirectorNodes/nodelibraryexport.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT SubtractMutator: public ArithmeticMutator
   {
   public:

      /**
       * Constructor.
       */
      SubtractMutator();

      /**
       * Accessors for the name of the node.
       */
      virtual std::string GetValueLabel();

      /**
       * Performs the arithmetic operation.
       *
       * @param[in]  left   The left value.
       * @param[in]  right  The right value.
       *
       * @return     The result.
       */
      virtual osg::Vec4 PerformOperation(const osg::Vec4& left, const osg::Vec4& right);

   protected:

      /**
       * Destructor.
       */
      ~SubtractMutator();

   private:
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // SUBTRACT_MUTATOR_NODE
