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

#ifndef COMPARE_LESS_MUTATOR_NODE
#define COMPARE_LESS_MUTATOR_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirectorNodes/comparemutator.h>
#include <dtDirectorNodes/nodelibraryexport.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT CompareLessMutator: public CompareMutator
   {
   public:

      /**
       * Constructor.
       */
      CompareLessMutator();

      /**
       * Accessors for the name of the node.
       */
      virtual std::string GetValueLabel();

      /**
       * Determines whether a value link on this node can connect
       * to a given value.
       *
       * @param[in]  link   The link.
       * @param[in]  value  The value to connect to.
       *
       * @return     True if a connection can be made.
       */
      virtual bool CanConnectValue(ValueLink* link, ValueNode* value);

      /**
       * Performs the compare operation.
       *
       * @param[in]  left   The left value.
       * @param[in]  right  The right value.
       *
       * @return     The result.
       */
      virtual bool Compare(const osg::Vec4& left, const osg::Vec4& right);

   protected:

      /**
       * Destructor.
       */
      ~CompareLessMutator();

   private:
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // COMPARE_LESS_MUTATOR_NODE
