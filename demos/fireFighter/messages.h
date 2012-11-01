/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * William E. Johnson II
 */
#ifndef DELTA_FIRE_FIGHTER_MESSAGES
#define DELTA_FIRE_FIGHTER_MESSAGES

#include <dtGame/message.h>
#include <fireFighter/export.h>

// Forward declaration
class MessageType;
class GameState;

class FIRE_FIGHTER_EXPORT GameStateChangedMessage : public dtGame::Message
{
   public:

      /// Constructor
      GameStateChangedMessage();

      /// Gets the outgoing game state
      GameState& GetOldState() const;

      /// Sets the outgoing game state
      void SetOldState(const GameState &oldState);

      /// Gets the incoming state
      GameState& GetNewState() const;

      /// Sets the incoming state
      void SetNewState(const GameState &newState);

   protected:

      /// Destructor
      virtual ~GameStateChangedMessage();

   private:

      dtGame::EnumMessageParameter &mOldParam, &mNewParam;
};
#endif
