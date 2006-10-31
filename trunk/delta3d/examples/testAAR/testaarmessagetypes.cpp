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
 * William E. Johnson II
 */
#include "testaarmessagetypes.h"

IMPLEMENT_ENUM(TestAARMessageType);

TestAARMessageType TestAARMessageType::PLACE_ACTOR("PLACE_ACTOR", "Place actor message", "", USER_DEFINED_MESSAGE_TYPE + 1);
TestAARMessageType TestAARMessageType::RESET("RESET", "Reset message", "Resets the scene", USER_DEFINED_MESSAGE_TYPE + 2);
TestAARMessageType TestAARMessageType::REQUEST_ALL_CONTROLLER_UPDATES("REQUEST_UPDATES", "Requests for updates from the controller", "", USER_DEFINED_MESSAGE_TYPE + 3);
TestAARMessageType TestAARMessageType::PRINT_TASKS("PRINT_TASKS", "Prints the tasks", "", USER_DEFINED_MESSAGE_TYPE + 4);
TestAARMessageType TestAARMessageType::UPDATE_TASK_CAMERA("UPDATE_TASK_CAMERA", "Updates the task camera", "", USER_DEFINED_MESSAGE_TYPE + 5);
TestAARMessageType TestAARMessageType::PLACE_IGNORED_ACTOR("PLACE_IGNORED_ACTOR", "Place ignored actor message", "", USER_DEFINED_MESSAGE_TYPE + 6);
