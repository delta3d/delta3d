/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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
 * Bradley Anderegg 06/28/2006
 */

#ifndef __DELTA_ASTARCOSTFUNC_H__
#define __DELTA_ASTARCOSTFUNC_H__

namespace dtAI
{
   /**
    * AStarCostFunc is a function that generates a cost between two nodes
    * derive this class and implement operator() with your custom data type.
    * The return must be a scalar value.
    */
   template<class _DataType, class _CostType>
   class AStarCostFunc
   {
      public:
         typedef _DataType data_type;
         typedef _CostType Real;

      public:
         AStarCostFunc(){}
         virtual ~AStarCostFunc(){}

         virtual Real operator()(data_type pFrom, data_type pTo) const = 0;

      private:

   };

} // namespace dtAI

#endif // __DELTA_ASTARCOSTFUNC_H__
