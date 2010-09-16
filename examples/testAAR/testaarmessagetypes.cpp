/* -*-c++-*-
 * testAAR - testaarmessagetypes (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * William E. Johnson II
 */

#include "testaarmessagetypes.h"

DT_IMPLEMENT_MESSAGE_TYPE_CLASS(TestAARMessageType);

TestAARMessageType TestAARMessageType::PLACE_ACTOR("PLACE_ACTOR", "Place actor message", "", USER_DEFINED_MESSAGE_TYPE + 1, DT_MSG_CLASS(dtGame::Message));
TestAARMessageType TestAARMessageType::RESET("RESET", "Reset message", "Resets the scene", USER_DEFINED_MESSAGE_TYPE + 2, DT_MSG_CLASS(dtGame::Message));
TestAARMessageType TestAARMessageType::REQUEST_ALL_CONTROLLER_UPDATES("REQUEST_UPDATES", "Requests for updates from the controller", "", USER_DEFINED_MESSAGE_TYPE + 3, DT_MSG_CLASS(dtGame::Message));
TestAARMessageType TestAARMessageType::PRINT_TASKS("PRINT_TASKS", "Prints the tasks", "", USER_DEFINED_MESSAGE_TYPE + 4, DT_MSG_CLASS(dtGame::Message));
TestAARMessageType TestAARMessageType::UPDATE_TASK_CAMERA("UPDATE_TASK_CAMERA", "Updates the task camera", "", USER_DEFINED_MESSAGE_TYPE + 5, DT_MSG_CLASS(dtGame::Message));
TestAARMessageType TestAARMessageType::PLACE_IGNORED_ACTOR("PLACE_IGNORED_ACTOR", "Place ignored actor message", "", USER_DEFINED_MESSAGE_TYPE + 6, DT_MSG_CLASS(dtGame::Message));
