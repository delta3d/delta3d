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


void MyNetwork::OnReceive( GNE::Connection &conn)
{
   mMutex.acquire();

   GNE::Packet *next = conn.stream().getNextPacket();

   while (next != NULL)
   {
      int type = next->getType();

      if(type == GNE::PingPacket::ID) 
      {
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
         PositionPacket *pos = (PositionPacket*)next;
         osg::Vec3 newXYZ = pos->mXYZ;
         osg::Vec3 newHPR = pos->mHPR;

         Transform xform;
         xform.SetTranslation(newXYZ);
         xform.SetRotation(newHPR);
         mOtherPlayer->SetTransform(&xform, Transformable::ABS_CS);
      }

      delete next;
      next = conn.stream().getNextPacket();
   }

   mMutex.release();
}

void MyNetwork::OnConnect( GNE::SyncConnection &conn )
{
   //do the default NetMgr behavior too
   NetMgr::OnConnect(conn);

   //make a player to represent the server
   MakePlayer();
}

void MyNetwork::MakePlayer()
{
   mMutex.acquire();

   mOtherPlayer = new dtCore::Object("other guy");
   mOtherPlayer->LoadFile("models/uh-1n.ive");
   Scene::GetInstance(0)->AddDrawable(mOtherPlayer.get());

   mMutex.release();
}

void MyNetwork::OnNewConn( GNE::SyncConnection &conn)
{
   //do the default NetMgr behavior too
   NetMgr::OnNewConn(conn);

   //make a player to represent the connected client
   MakePlayer();
}


void MyNetwork::OnExit( GNE::Connection &conn)
{
   //do the default NetMgr behavior too
   NetMgr::OnExit(conn);

   mMutex.acquire();

   //the other player exited the network, remove his representation
   Scene::GetInstance(0)->RemoveDrawable(mOtherPlayer.get());

   mMutex.release();
}
