#include <iostream>
#include <gnelib/PacketParser.h>
#include <gnelib/Buffer.h>
#include "packets.h"


//our unique ID for this custom packet
const int PositionPacket::ID = GNE::PacketParser::MIN_USER_ID;

////////////////////////////////////////////////////////////////////////////////
PositionPacket::PositionPacket()
   : GNE::Packet(ID)
{
}

////////////////////////////////////////////////////////////////////////////////
PositionPacket::PositionPacket(osg::Vec3 xyz, osg::Vec3 hpr, const std::string& ownerID)
   : GNE::Packet(ID)
{
   mXYZ = xyz;
   mHPR = hpr;
   mOwnerID = ownerID;
}

////////////////////////////////////////////////////////////////////////////////
PositionPacket::PositionPacket(const PositionPacket& p)
   : GNE::Packet(ID)
{
   mXYZ = p.mXYZ;
   mHPR = p.mHPR;
   mOwnerID = p.mOwnerID;
}

////////////////////////////////////////////////////////////////////////////////
void PositionPacket::writePacket(GNE::Buffer& raw) const
{
   GNE::Packet::writePacket(raw);
   raw << mXYZ._v[0];
   raw << mXYZ._v[1];
   raw << mXYZ._v[2];
   raw << mHPR._v[0];
   raw << mHPR._v[1];
   raw << mHPR._v[2];
   raw << mOwnerID;
}

////////////////////////////////////////////////////////////////////////////////
void PositionPacket::readPacket(GNE::Buffer& raw)
{
   GNE::Packet::readPacket(raw);
   raw >> mXYZ._v[0];
   raw >> mXYZ._v[1];
   raw >> mXYZ._v[2];
   raw >> mHPR._v[0];
   raw >> mHPR._v[1];
   raw >> mHPR._v[2];
   raw >> mOwnerID;
}

////////////////////////////////////////////////////////////////////////////////
int PositionPacket::getSize() const
{
   //return the size in bytes
   return Packet::getSize() + sizeof(mXYZ) + sizeof(mHPR) + sizeof(mOwnerID);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const int PlayerQuitPacket::ID = GNE::PacketParser::MIN_USER_ID + 1;

////////////////////////////////////////////////////////////////////////////////
PlayerQuitPacket::PlayerQuitPacket(const std::string& playerID)
   : GNE::Packet(ID)
   , mPlayerID(playerID)
{
}

////////////////////////////////////////////////////////////////////////////////
PlayerQuitPacket::PlayerQuitPacket()
   : GNE::Packet(ID)
{
}

////////////////////////////////////////////////////////////////////////////////
PlayerQuitPacket::PlayerQuitPacket(const PlayerQuitPacket& p)
   : GNE::Packet(ID)
   , mPlayerID(p.mPlayerID)
{
}

////////////////////////////////////////////////////////////////////////////////
void PlayerQuitPacket::writePacket(GNE::Buffer& raw) const
{
   GNE::Packet::writePacket(raw);
   raw << mPlayerID;
}

////////////////////////////////////////////////////////////////////////////////
void PlayerQuitPacket::readPacket(GNE::Buffer& raw)
{
   GNE::Packet::readPacket(raw);
   raw >> mPlayerID;
}

////////////////////////////////////////////////////////////////////////////////
int PlayerQuitPacket::getSize() const
{
   return Packet::getSize() + sizeof(mPlayerID);
}
