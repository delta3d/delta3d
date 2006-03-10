/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author David Guthrie
 */
#include "dtGame/exceptionenum.h"

namespace dtGame
{
   IMPLEMENT_ENUM(ExceptionEnum);

   ExceptionEnum ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION("General Game Manager Exception");
   ExceptionEnum ExceptionEnum::INVALID_PARAMETER("Invalid method parameter");   
   ExceptionEnum ExceptionEnum::UNKNOWN_ACTOR_TYPE("Unknown actor type");
   ExceptionEnum ExceptionEnum::ACTOR_IS_REMOTE("Actor is remote");   
   ExceptionEnum ExceptionEnum::INVALID_ACTOR_STATE("Invalid Actor State");
   ExceptionEnum ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR("Game Application Config Failed.");
   ExceptionEnum ExceptionEnum::DEAD_RECKONING_EXCEPTION("Dead Reckoning Exception");
}
