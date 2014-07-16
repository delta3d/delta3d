/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2009, Alion Science and Technology, BMH Operation
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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "testappgamestates.h"



namespace dtExample
{
   //////////////////////////////////////////////////////////////////////////
   // GAME STATE TYPE CODE
   //////////////////////////////////////////////////////////////////////////
   TestAppGameState TestAppGameState::STATE_HELP("STATE_HELP");
   TestAppGameState TestAppGameState::STATE_GAME("STATE_GAME");
   TestAppGameState TestAppGameState::STATE_GAME_OPTIONS("STATE_GAME_OPTIONS");
   TestAppGameState TestAppGameState::STATE_GAME_QUIT("STATE_GAME_QUIT");
   TestAppGameState TestAppGameState::STATE_TITLE("STATE_TITLE");



   //////////////////////////////////////////////////////////////////////////
   // TRANSITION CODE
   //////////////////////////////////////////////////////////////////////////
   TestAppTransition TestAppTransition::TRANSITION_MENU("TRANSITION_MENU");

} // END - namespace dtExample
