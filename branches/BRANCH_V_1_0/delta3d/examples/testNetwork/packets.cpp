#include <iostream>
#include <gnelib/PacketParser.h>
#include <gnelib/Buffer.h>
#include "packets.h"


//our unique ID for this custom packet
const int PositionPacket::ID = GNE::PacketParser::MIN_USER_ID;


PositionPacket::PositionPacket():
GNE::Packet(ID)
{
}

PositionPacket::PositionPacket( osg::Vec3 xyz, osg::Vec3 hpr):
GNE::Packet(ID)
{
   mXYZ = xyz;
   mHPR = hpr;
}

PositionPacket::PositionPacket( const PositionPacket &p):
GNE::Packet(ID)
{
   mXYZ = p.mXYZ;
   mHPR = p.mHPR;
}

void PositionPacket::writePacket(GNE::Buffer &raw) const
{
   GNE::Packet::writePacket(raw);
   raw << mXYZ._v[0];
   raw << mXYZ._v[1];
   raw << mXYZ._v[2];
   raw << mHPR._v[0];
   raw << mHPR._v[1];
   raw << mHPR._v[2];
}

void PositionPacket::readPacket( GNE::Buffer &raw)
{
   GNE::Packet::readPacket(raw);
   raw >> mXYZ._v[0];
   raw >> mXYZ._v[1];
   raw >> mXYZ._v[2];
   raw >> mHPR._v[0];
   raw >> mHPR._v[1];
   raw >> mHPR._v[2];
}

///return the size in bytes
int PositionPacket::getSize() const
{
   return Packet::getSize() + sizeof(mXYZ) + sizeof(mHPR);
}
