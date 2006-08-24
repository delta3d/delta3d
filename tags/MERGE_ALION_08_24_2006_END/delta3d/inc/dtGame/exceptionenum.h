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
#ifndef DELTA_GAME_EXCEPTION_ENUM
#define DELTA_GAME_EXCEPTION_ENUM

#include <dtUtil/enumeration.h>
#include "dtGame/export.h"

namespace dtGame
{

   class DT_GAME_EXPORT ExceptionEnum : public dtUtil::Enumeration
   {
      DECLARE_ENUM(ExceptionEnum);
      public:
         ///Exception enumeration used for general game manager exceptions.
         static ExceptionEnum GENERAL_GAMEMANAGER_EXCEPTION;

         ///Exception enumeration used for method invalid parameters.
         static ExceptionEnum INVALID_PARAMETER;
         
         ///Exception enumeration used when trying to create a actor on the game manager with an unknown type.
         static ExceptionEnum UNKNOWN_ACTOR_TYPE;
         
         ///Exception enumeration used when an actor is remote and an operation only applicable to local actors is requested.
         static ExceptionEnum ACTOR_IS_REMOTE;

         ///Exception enumeration used when actors are in invalid states and code can't continue.
         static ExceptionEnum INVALID_ACTOR_STATE;

         ///Game applcation was unable to initialize for some reason.
         static ExceptionEnum GAME_APPLICATION_CONFIG_ERROR;

         ///Used when an error occures working with the dead reckoing component.
         static ExceptionEnum DEAD_RECKONING_EXCEPTION;
      protected:
         ExceptionEnum(const std::string &name) : Enumeration(name)
         {
            AddInstance(this);
         }
   };

}

#endif /*DELTA_GAME_EXCEPTION_ENUM*/
