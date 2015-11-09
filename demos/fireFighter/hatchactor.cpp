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

#include <fireFighter/hatchactor.h>
#include <fireFighter/gamelevelactor.h>
#include <fireFighter/entityactorregistry.h>
#include <fireFighter/messagetype.h>

#include <dtAudio/audiomanager.h>

#include <dtCore/actorproperty.h>
#include <dtCore/functor.h>
#include <dtCore/gameeventmanager.h>

#include <dtGame/basemessages.h>
#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtGame/messagefactory.h>

#include <osg/MatrixTransform>

using dtCore::RefPtr;

static osg::Node* FindNamedNode(const std::string& searchName, osg::Node* currNode)
{
   osg::Group* currGroup = nullptr;
   osg::Node*  foundNode = nullptr;

   // check to see if we have a valid (non-nullptr) node.
   // if we do have a null node, return nullptr.
   if (currNode == nullptr)
   {
      return nullptr;
   }

   // We have a valid node, check to see if this is the node we
   // are looking for. If so, return the current node.
   if (currNode->getName() == searchName)
   {
      return currNode;
   }

   // We have a valid node, but not the one we are looking for.
   // Check to see if it has children (non-leaf node). If the node
   // has children, check each of the child nodes by recursive call.
   // If one of the recursive calls returns a non-null value we have
   // found the correct node, so return this node.
   // If we check all of the children and have not found the node,
   // return nullptr
   currGroup = currNode->asGroup(); // returns nullptr if not a group.
   if (currGroup != nullptr)
   {
      for (unsigned int i = 0; i < currGroup->getNumChildren(); i++)
      {
         foundNode = FindNamedNode(searchName, currGroup->getChild(i));
         if (foundNode != nullptr)
         {
            return foundNode; // found a match!
         }
      }
      return nullptr; // We have checked each child node - no match found.
   }
   else
   {
      return nullptr; // leaf node, no match
   }
}

HatchActor::HatchActor()
   : mHatchNode(nullptr)
   , mGameMapLoaded(false)
{

}

HatchActor::~HatchActor()
{

}

void HatchActor::OnEnteredWorld()
{
   GameItemActor::OnEnteredWorld();

   RegisterForMessages(dtGame::MessageType::INFO_MAP_LOADED, dtUtil::MakeFunctor(&HatchActor::OnMapLoaded, *this));
   RegisterForMessages(dtGame::MessageType::INFO_MAP_UNLOADED, dtUtil::MakeFunctor(&HatchActor::OnMapLoaded, *this));
}

void HatchActor::Activate(bool enable)
{
   GameItemActor::Activate(enable);

   // If we are in STAGE, we have a nullptr game manager. So peace out of here
   if (IsInSTAGE())
   {
      return;
   }

   if (mHatchNode != nullptr)
   {
      // Open or close the door
      osg::Matrix rotMat;
      const float deg = IsActivated() ? -105.0f : 105.0f;

      rotMat.makeRotate(
         osg::DegreesToRadians(0.0f), osg::Vec3(1.0f, 0.0f, 0.0f),
         osg::DegreesToRadians(0.0f), osg::Vec3(0.0f, 1.0f, 0.0f),
         osg::DegreesToRadians(deg),  osg::Vec3(0.0f, 0.0f, 1.0f));

      mHatchNode->preMult(rotMat);
   }

   if (mGameMapLoaded && !IsActivated())
   {
      PlayItemUseSnd();
   }

   // Special case. Since Activate(bool) is a property that is utilized in
   // STAGE, this function is called once on startup to initialize the
   // properties. When this happens, the game manager has not be initialized
   // or something. It comes back with a weird memory address and outright
   // crashes. This doesn't occur in other versions of Activate(bool) because
   // the base class version does not send a message. That behavior is handled
   // in the player class.
   if (mGameMapLoaded)
   {
      dtGame::GameManager& mgr = *GetGameManager();

      RefPtr<dtGame::Message> msg =
         mgr.GetMessageFactory().CreateMessage(enable ? FireFighterMessageType::ITEM_ACTIVATED :
                                                        FireFighterMessageType::ITEM_DEACTIVATED);

      msg->SetAboutActorId(GetId());
      mgr.SendMessage(*msg);
   }

   const std::string& name = "OpenHatch";

   // No event, peace out
   if (!IsActivated())
   {
      return;
   }

   dtCore::GameEvent* event = dtCore::GameEventManager::GetInstance().FindEvent(name);
   if (event == nullptr)
   {
      throw dtUtil::Exception("Failed to find the game event: " + name, __FILE__, __LINE__);
   }

   dtGame::GameManager& mgr = *GetGameManager();
   RefPtr<dtGame::Message> msg =
      mgr.GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT);

   dtGame::GameEventMessage& gem = static_cast<dtGame::GameEventMessage&>(*msg);
   gem.SetGameEvent(*event);
   mgr.SendMessage(gem);
}

void HatchActor::OnMapLoaded(const dtGame::Message& msg)
{
   if (msg.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      const dtGame::MapMessage& mlm = static_cast<const dtGame::MapMessage&>(msg);
      std::vector<std::string> mapNames;
      mlm.GetMapNames(mapNames);
      if (!mapNames.empty() && mapNames[0] == "GameMap")
      {
         mGameMapLoaded = true;
         // Find the game level actor and search with its node
         dtCore::ActorPtrVector proxies;
         GetGameManager()->FindActorsByType(*EntityActorRegistry::TYPE_GAME_LEVEL_ACTOR, proxies);
         GameLevelActor* gla = proxies[0]->GetDrawable<GameLevelActor>();
         if (gla == nullptr)
         {
            LOG_ERROR("Failed to find the game level actor in the map. Unable to open or close the hatch door");
         }
         else
         {
            osg::Node* hatchNode = FindNamedNode("HatchEngr", gla->GetOSGNode());
            mHatchNode = dynamic_cast<osg::MatrixTransform*>(hatchNode);
            if (mHatchNode == nullptr)
            {
               LOG_ERROR("Failed to find the hatch node in the game level.");
            }
         }
      }
   }
   else if (msg.GetMessageType() == dtGame::MessageType::INFO_MAP_UNLOADED)
   {
      const dtGame::MapMessage& mlm = static_cast<const dtGame::MapMessage&>(msg);
      std::vector<std::string> mapNames;
      mlm.GetMapNames(mapNames);
      if (!mapNames.empty() && mapNames[0] == "GameMap")
      {
         mGameMapLoaded = false;
      }
   }
   else
   {
      LOG_ERROR("Received a message of incorrect type: " + msg.GetMessageType().GetName());
   }
}

