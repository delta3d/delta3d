#include "mynetwork.h"
#include "packets.h"
#include <osg/io_utils>
#include <dtUtil/stringutils.h>
#include <dtCore/scene.h>

using namespace dtUtil;
using namespace dtCore;

MyNetwork::MyNetwork(void)
{
}

MyNetwork::~MyNetwork(void)
{
}


///One or more GNE::Packets was received, let's do something with them
void MyNetwork::OnReceive( GNE::Connection &conn)
{
   mMutex.acquire();

   GNE::Packet *next = conn.stream().getNextPacket();

   while (next != NULL)
   {
      int type = next->getType();

      if(type == GNE::PingPacket::ID) 
      {
         //it's a PingPacket so lets process it.
         GNE::PingPacket &ping = *((GNE::PingPacket*)next);
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
         //aha, this is our custom packet.  Decompose it and move our remote 
         //player representation.
         PositionPacket *pos = (PositionPacket*)next;
         osg::Vec3 newXYZ = pos->mXYZ;
         osg::Vec3 newHPR = pos->mHPR;
         std::string ownerID = pos->mOwnerID;

         Transform xform;
         xform.SetTranslation(newXYZ);
         xform.SetRotation(newHPR);

         RefPtr<Object> remoteObj = mOtherPlayerList[ownerID];
         if (remoteObj !=0 ) 
         {
            //the player ID is already in our list so lets update it's position
            remoteObj->SetTransform(&xform, Transformable::ABS_CS);
         }
         else
         {
            //this player isn't in our list, so let's create him
            MakePlayer( ownerID );
         }

         if( GetIsServer() )
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

      delete next;
      next = conn.stream().getNextPacket();
   }

   mMutex.release();
}


///Create a new player to represent the remote guy
void MyNetwork::MakePlayer(const std::string ownerID)
{
   mMutex.acquire();

   LOG_INFO("making a new remote player named: " + ownerID);

   mOtherPlayerList[ownerID] = new dtCore::Object(ownerID);
   mOtherPlayerList[ownerID]->LoadFile("models/uh-1n.ive");
   Scene::GetInstance(0)->AddDrawable( mOtherPlayerList[ownerID].get() );

   mMutex.release();
}


void MyNetwork::OnExit( GNE::Connection &conn)
{
   //do the default NetMgr behavior too
   NetMgr::OnExit(conn);

   mMutex.acquire();

   //todo: the other player exited the network, remove his representation

   mMutex.release();
}

void MyNetwork::OnDisconnect( GNE::Connection &conn)
{
  RemoveConnection(&conn);
}
