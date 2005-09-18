#ifndef PACKETS_INCLUDED
#define PACKETS_INCLUDED

#include <osg/Vec3>
#include <gnelib/Packet.h>

class PositionPacket : public GNE::Packet
{
public:

   PositionPacket( osg::Vec3 xyz, osg::Vec3 hpr);

   PositionPacket();

   PositionPacket( const PositionPacket &p);

   virtual ~PositionPacket() {}

   static const int ID;

   virtual int getSize() const;

   virtual void writePacket(GNE::Buffer &raw) const;

   virtual void readPacket( GNE::Buffer &raw);


   osg::Vec3 mXYZ;
   osg::Vec3 mHPR;

};

#endif //PACKETS_INCLUDED