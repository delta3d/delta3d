/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author William E. Johnson II
 */
#ifndef DELTA_TEST_AAR_MESSAGE_TYPES
#define DELTA_TEST_AAR_MESSAGE_TYPES

#include <dtGame/messagetype.h>
#include "export.h"

class TEST_AAR_EXPORT TestAARMessageType : public dtGame::MessageType
{
   public:

      DECLARE_ENUM(TestAARMessageType);

      static TestAARMessageType PLACE_ACTOR;
      static TestAARMessageType RESET;
      static TestAARMessageType REQUEST_ALL_CONTROLLER_UPDATES;
      static TestAARMessageType PRINT_TASKS;
      static TestAARMessageType UPDATE_TASK_CAMERA;

   private:

      /// Constructor
      TestAARMessageType(const std::string &name, 
                            const std::string &category = "", 
                            const std::string &desc = "", 
                            const unsigned short id = 0) : dtGame::MessageType(name, category, desc, id)
      {
         AddInstance(this);
      }
};

#endif

