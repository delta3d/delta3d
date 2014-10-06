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

#ifndef __DELTA_WORLDSTATE_H__
#define __DELTA_WORLDSTATE_H__

#include <dtAI/export.h>
#include <dtAI/statevariable.h>

#include <map>
#include <string>
#include <ostream>

namespace dtAI
{
   /**
    *
    */
   class DT_AI_EXPORT WorldState
   {
   public:
      typedef std::pair<std::string, IStateVariable*> StringStateMapping;
      typedef std::map<std::string, IStateVariable*> StateVarMapping;

   public:
      WorldState();
      WorldState(const WorldState& pWS);

      WorldState& operator=(const WorldState& pWS);

      virtual ~WorldState();

      float GetCost() const;
      void AddCost(float pCost);

      void AddState(const std::string& pName, IStateVariable* pStateVar);
      IStateVariable* GetState(const std::string& pState);

      template <typename T>
      void GetState(const std::string& pState, T*& pStateVar)
      {
         pStateVar = dynamic_cast<T*>(GetState(pState));
      }

      const IStateVariable* GetState(const std::string& pState) const;

      template <typename T>
      void GetState(const std::string& pState, const T*& pStateVar) const
      {
         pStateVar = dynamic_cast<const T*>(GetState(pState));
      }

      const StateVarMapping& GetStateVariables() const { return mStateVariables; }
      StateVarMapping& GetStateVariables() { return mStateVariables; }

   private:
      void FreeMem();

      float mCost;
      StateVarMapping mStateVariables;
   };

   DT_AI_EXPORT std::ostream& operator << (std::ostream &o, const WorldState &worldState);

} // namespace dtAI

#endif // __DELTA_WORLDSTATE_H__
