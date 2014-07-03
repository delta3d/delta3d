#include "mynetwork.h"
#include "packets.h"
#include <osg/io_utils>
#include <dtUtil/stringutils.h>
#include <dtCore/transform.h>

using namespace dtUtil;
using namespace dtCore;

////////////////////////////////////////////////////////////////////////////////
MyNetwork::MyNetwork(dtCore::Scene* scene)
   : mScene(scene)
{
}

////////////////////////////////////////////////////////////////////////////////
void MyNetwork::OnReceive(GNE::Connection& conn)
{
   mMutex.acquire();

   GNE::Packet* next = conn.stream().getNextPacket();

   while (next != 0)
   {
      int type = next->getType();

      if (type == GNE::PingPacket::ID)
      {
         //it's a PingPacket so lets process it.
         GNE::PingPacket& ping = *static_cast<GNE::PingPacket*>(next);
         if (ping.isRequest())
         {
            ping.makeReply();
            conn.stream().writePacket(ping, true);
         }
         else
         {
            LOG_INFO("Ping: " + ping.getPingInformation().pingTime.toString());
         }
      }
      else if (type == PositionPacket::ID)
      {
         //aha, this is one of our custom packets.  Decompose it and move our remote
         //player representation.
         PositionPacket* pos = static_cast<PositionPacket*>(next);
         osg::Vec3 newXYZ = pos->mXYZ;
         osg::Vec3 newHPR = pos->mHPR;
         std::string ownerID = pos->mOwnerID;

         Transform xform;
         xform.SetTranslation(newXYZ);
         xform.SetRotation(newHPR);

         StringObjectMap::iterator iter = mOtherPlayerMap.find(ownerID);

         if (iter != mOtherPlayerMap.end())
         {
            //the player ID is already in our list so lets update it's position
            iter->second->SetTransform(xform, Transformable::ABS_CS);
         }
         else
         {
            //this player isn't in our list, so let's create him
            MakePlayer(ownerID);
         }

         if (GetIsServer())
         {
            //rebroadcast the packet to all connections except for the
            //one who sent the packet in the first place
            ConnectionIterator conns = mConnections.begin();
            while (conns != mConnections.end())
            {
               if (conns->first != conn.getRemoteAddress(true).toString())
               {
                  SendPacket(conns->first, *next);
               }
               ++conns;
            }
         }
      }
      else if (type == PlayerQuitPacket::ID)
      {
         PlayerQuitPacket* playerQuitPacket = static_cast<PlayerQuitPacket*>(next);
         mIDsToRemove.push(playerQuitPacket->mPlayerID);
      }

      delete next;
      next = conn.stream().getNextPacket();
   }

   mMutex.release();
}

////////////////////////////////////////////////////////////////////////////////
void MyNetwork::MakePlayer(const std::string& ownerID)
{
   mMutex.acquire();

   LOG_INFO("Making a new remote player named: " + ownerID)

   dtCore::RefPtr<dtCore::Object> object = new dtCore::Object(ownerID);
   object->LoadFile("StaticMeshes/uh-1n.ive");

   Transform transform(0.0f, 0.0f, 5.0f);
   object->SetTransform(transform);

   // Insert the new Objects into our map of IDs->Objects
   StringObjectMap::value_type value(ownerID,  object);
   mOtherPlayerMap.insert(value);

   mObjectsToAdd.push(object);

   mMutex.release();
}

////////////////////////////////////////////////////////////////////////////////
void MyNetwork::OnExit(GNE::Connection& conn)
{
   // do the default NetMgr behavior too
   NetMgr::OnExit(conn);
}

////////////////////////////////////////////////////////////////////////////////
void MyNetwork::OnDisconnect(GNE::Connection& conn)
{
   RemoveConnection(&conn);
}

////////////////////////////////////////////////////////////////////////////////
void MyNetwork::PreFrame(const double deltaFrameTime)
{
   // Process the Objects we wish to add to the scene
   while (!mObjectsToAdd.empty())
   {
      mScene->AddChild(mObjectsToAdd.front().get());

      // Convert the address to a string for logging.
      std::ostringstream oss;
      oss << mObjectsToAdd.front();
      LOG_INFO("Added player " + oss.str() + " to the scene.")

      mObjectsToAdd.pop();
   }

   // Process the Objects we wish to remove from the scene
   while (!mIDsToRemove.empty())
   {
      StringObjectMap::iterator iter = mOtherPlayerMap.find(mIDsToRemove.front());

      if (iter != mOtherPlayerMap.end())
      {
         mScene->RemoveChild((iter->second).get());
         LOG_INFO("Removed player " + iter->first + " from the scene.")

         mOtherPlayerMap.erase(iter);
      }
      else
      {
         LOG_WARNING("Attempt to remove player " + iter->first + " failed.")
      }

      mIDsToRemove.pop();
   }
}
