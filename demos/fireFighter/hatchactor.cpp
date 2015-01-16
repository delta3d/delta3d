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
   osg::Group* currGroup = NULL;
   osg::Node*  foundNode = NULL;

   // check to see if we have a valid (non-NULL) node.
   // if we do have a null node, return NULL.
   if (currNode == NULL)
   {
      return NULL;
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
   // return NULL
   currGroup = currNode->asGroup(); // returns NULL if not a group.
   if (currGroup != NULL)
   {
      for (unsigned int i = 0; i < currGroup->getNumChildren(); i++)
      {
         foundNode = FindNamedNode(searchName, currGroup->getChild(i));
         if (foundNode != NULL)
         {
            return foundNode; // found a match!
         }
      }
      return NULL; // We have checked each child node - no match found.
   }
   else
   {
      return NULL; // leaf node, no match
   }
}

////////////////////////////////////////////////
HatchActorProxy::HatchActorProxy()
{

}

HatchActorProxy::~HatchActorProxy()
{

}

void HatchActorProxy::BuildPropertyMap()
{
   GameItemActorProxy::BuildPropertyMap();
}

void HatchActorProxy::BuildInvokables()
{
   GameItemActorProxy::BuildInvokables();
}

////////////////////////////////////////////////

HatchActor::HatchActor(dtGame::GameActorProxy& parent)
   : GameItemActor(parent)
   , mHatchNode(NULL)
   , mGameMapLoaded(false)
{

}

HatchActor::~HatchActor()
{

}

void HatchActor::OnEnteredWorld()
{
   GameItemActor::OnEnteredWorld();

   dtGame::Invokable* invoke = new dtGame::Invokable("MapLoaded",
      dtUtil::MakeFunctor(&HatchActor::OnMapLoaded, *this));

   GetGameActorProxy().AddInvokable(*invoke);

   GetGameActorProxy().RegisterForMessages(dtGame::MessageType::INFO_MAP_LOADED, "MapLoaded");
   GetGameActorProxy().RegisterForMessages(dtGame::MessageType::INFO_MAP_UNLOADED, "MapLoaded");
}

void HatchActor::Activate(bool enable)
{
   GameItemActor::Activate(enable);

   // If we are in STAGE, we have a NULL game manager. So peace out of here
   if (GetGameActorProxy().IsInSTAGE())
   {
      return;
   }

   if (mHatchNode != NULL)
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
      dtGame::GameManager& mgr = *GetGameActorProxy().GetGameManager();

      RefPtr<dtGame::Message> msg =
         mgr.GetMessageFactory().CreateMessage(enable ? FireFighterMessageType::ITEM_ACTIVATED :
                                                        FireFighterMessageType::ITEM_DEACTIVATED);

      msg->SetAboutActorId(GetUniqueId());
      mgr.SendMessage(*msg);
   }

   const std::string& name = "OpenHatch";

   // No event, peace out
   if (!IsActivated())
   {
      return;
   }

   dtCore::GameEvent* event = dtCore::GameEventManager::GetInstance().FindEvent(name);
   if (event == NULL)
   {
      throw dtUtil::Exception("Failed to find the game event: " + name, __FILE__, __LINE__);
   }

   dtGame::GameManager& mgr = *GetGameActorProxy().GetGameManager();
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
         std::vector<dtCore::BaseActorObject*> proxies;
         GetGameActorProxy().GetGameManager()->FindActorsByType(*EntityActorRegistry::TYPE_GAME_LEVEL_ACTOR, proxies);
         GameLevelActor* gla = dynamic_cast<GameLevelActor*>(proxies[0]->GetDrawable());
         if (gla == NULL)
         {
            LOG_ERROR("Failed to find the game level actor in the map. Unable to open or close the hatch door");
         }
         else
         {
            osg::Node* hatchNode = FindNamedNode("HatchEngr", gla->GetOSGNode());
            mHatchNode = dynamic_cast<osg::MatrixTransform*>(hatchNode);
            if (mHatchNode == NULL)
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

