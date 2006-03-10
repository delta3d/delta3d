#ifndef PACKETS_INCLUDED
#define PACKETS_INCLUDED

#include <osg/Vec3>
#include <gnelib/Packet.h>

/** This custom packet will allow us to pass a position and rotation
 *  through the network connection.
 *  Two important things to remember:
 *  1) register the packet with GNE using
 *     GNE::PacketParser::defaultRegisterPacket<PositionPacket>();
 *  2) Every custom packet needs a unique ID
 */
class PositionPacket : public GNE::Packet
{
public:

   PositionPacket( osg::Vec3 xyz, osg::Vec3 hpr, const std::string &ownerID );

   ///default constructor
   PositionPacket();

   ///copy constructor
   PositionPacket( const PositionPacket &p );

   virtual ~PositionPacket() {}

   static const int ID;

   virtual int getSize() const;

   virtual void writePacket( GNE::Buffer &raw ) const;

   virtual void readPacket( GNE::Buffer &raw );

   osg::Vec3 mXYZ;
   osg::Vec3 mHPR;
   std::string mOwnerID;
};

/** This custom packet will allow us to pass a player's UniqueID upon
*  exit to let server and other clients known a player has quit the game.
*/
class PlayerQuitPacket : public GNE::Packet
{
public:

   PlayerQuitPacket( const std::string& playerID );
   PlayerQuitPacket();

public:

   PlayerQuitPacket( const PlayerQuitPacket& p );

   virtual ~PlayerQuitPacket() {}

   static const int ID;

   virtual int getSize() const;

   virtual void writePacket( GNE::Buffer& raw ) const;

   virtual void readPacket( GNE::Buffer& raw );

   std::string mPlayerID;
};

#endif //PACKETS_INCLUDED
