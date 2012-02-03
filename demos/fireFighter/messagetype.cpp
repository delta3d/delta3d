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
#include <fireFighter/messagetype.h>
#include <fireFighter/messages.h>

DT_IMPLEMENT_MESSAGE_TYPE_CLASS(FireFighterMessageType)
const FireFighterMessageType FireFighterMessageType::GAME_STATE_CHANGED("GAME_STATE_CHANGED", "INFO", "Sent when a game state changes", USER_DEFINED_MESSAGE_TYPE + 1, DT_MSG_CLASS(GameStateChangedMessage));
const FireFighterMessageType FireFighterMessageType::ITEM_ACQUIRED("ITEM_ACQUIRED", "INFO", "Sent the the player picks up an item", USER_DEFINED_MESSAGE_TYPE + 2, DT_MSG_CLASS(dtGame::Message));
const FireFighterMessageType FireFighterMessageType::ITEM_ACTIVATED("ITEM_ACTIVATED", "INFO", "Sent when an inventory item is selected", USER_DEFINED_MESSAGE_TYPE + 3, DT_MSG_CLASS(dtGame::Message));
const FireFighterMessageType FireFighterMessageType::ITEM_DEACTIVATED("ITEM_DEACTIVATED", "INFO", "Sent when an inventory item is deselected", USER_DEFINED_MESSAGE_TYPE + 9, DT_MSG_CLASS(dtGame::Message));
//const FireFighterMessageType FireFighterMessageType::ITEM_USED("ITEM_USED", "INFO", "Sent when an item in the inventory is used", USER_DEFINED_MESSAGE_TYPE + 4, DT_MSG_CLASS(dtGame::Message));
const FireFighterMessageType FireFighterMessageType::ITEM_SELECTED("ITEM_SELECTED", "INFO", "Sent when an item in the inventory is selected", USER_DEFINED_MESSAGE_TYPE + 8, DT_MSG_CLASS(dtGame::Message));
const FireFighterMessageType FireFighterMessageType::ITEM_INTERSECTED("ITEM_INTERSECTED", "INFO", "Sent when an item is collided with", USER_DEFINED_MESSAGE_TYPE + 5, DT_MSG_CLASS(dtGame::Message));
const FireFighterMessageType FireFighterMessageType::MISSION_COMPLETE("MISSION_COMPLETE", "INFO", "Sent when the top level game task is completed", USER_DEFINED_MESSAGE_TYPE + 6, DT_MSG_CLASS(dtGame::Message));
const FireFighterMessageType FireFighterMessageType::MISSION_FAILED("MISSION_FAILED", "INFO", "Sent when the mission is failed", USER_DEFINED_MESSAGE_TYPE + 10, DT_MSG_CLASS(dtGame::Message));
const FireFighterMessageType FireFighterMessageType::HELP_WINDOW_OPENED("HELP_WINDOW_OPENED", "INFO", "Sent when F1 is pressed", USER_DEFINED_MESSAGE_TYPE + 11, DT_MSG_CLASS(dtGame::Message));
const FireFighterMessageType FireFighterMessageType::HELP_WINDOW_CLOSED("HELP_WINDOW_CLOSED", "INFO", "Sent when F1 is pressed", USER_DEFINED_MESSAGE_TYPE + 12, DT_MSG_CLASS(dtGame::Message));
