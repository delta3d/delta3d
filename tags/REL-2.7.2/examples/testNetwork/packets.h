/* -*-c++-*-
* testNetwork - packets (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, MOVES Institute
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
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

   PositionPacket(osg::Vec3 xyz, osg::Vec3 hpr, const std::string& ownerID);
   PositionPacket();

   PositionPacket(const PositionPacket& p);

   virtual ~PositionPacket() {}

   static const int ID;

   virtual int getSize() const;

   virtual void writePacket(GNE::Buffer& raw) const;

   virtual void readPacket(GNE::Buffer& raw);

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

   PlayerQuitPacket(const std::string& playerID);
   PlayerQuitPacket();

public:

   PlayerQuitPacket(const PlayerQuitPacket& p);

   virtual ~PlayerQuitPacket() {}

   static const int ID;

   virtual int getSize() const;

   virtual void writePacket(GNE::Buffer& raw) const;

   virtual void readPacket(GNE::Buffer& raw);

   std::string mPlayerID;
};

#endif //PACKETS_INCLUDED
