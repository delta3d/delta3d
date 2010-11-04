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
 * Bradley Anderegg 07/24/2006
 */

#ifndef __DELTA_STATEVARIABLE_H__
#define __DELTA_STATEVARIABLE_H__

#include <dtAI/export.h>
#include <string>

namespace dtAI
{
   /**
    * A single entity of a dtAI::WorldState
    */
   class DT_AI_EXPORT IStateVariable
   {
   public:
      virtual ~IStateVariable() {};

      virtual IStateVariable* Copy() const = 0;

      virtual const std::string ToString() const = 0;

   private:
   };

   inline std::ostream& operator << (std::ostream &o, const IStateVariable& stateVar)
   {
      o << stateVar.ToString();
      return o;
   }
} // namespace dtAI

#endif // __DELTA_STATEVARIABLE_H__
