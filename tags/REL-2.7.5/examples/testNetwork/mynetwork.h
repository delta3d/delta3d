/* -*-c++-*-
 * testNetwork - mynetwork (.h & .cpp) - Using 'The MIT License'
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
#ifndef MYNETWORK_INCLUDE
#define MYNETWORK_INCLUDE

#include <dtNet/netmgr.h>
#include <dtCore/refptr.h>
#include <dtCore/object.h>
#include <dtCore/scene.h>

/** Deriving from NetMgr will allow use to overwrite some virtual methods.
  * We'll use these methods for controlling our network connections.
  */
class MyNetwork : public dtNet::NetMgr
{
public:
   MyNetwork(dtCore::Scene* scene);
   virtual ~MyNetwork() {}

   ///One or more GNE::Packets was received, let's do something with them
   virtual void OnReceive(GNE::Connection& conn);

   virtual void OnExit(GNE::Connection& conn);
   virtual void OnDisconnect(GNE::Connection& conn);

   void PreFrame(const double deltaFrameTime);

private:
   dtCore::RefPtr<dtCore::Scene >              mScene;
   std::queue<dtCore::RefPtr<dtCore::Object> > mObjectsToAdd;
   std::queue<std::string >                    mIDsToRemove;

   ///a map of player ID strings and their corresponding Object
   typedef std::map<std::string, dtCore::RefPtr<dtCore::Object> > StringObjectMap;
   StringObjectMap mOtherPlayerMap;

   ///Create a new player to represent the remote guy
   void MakePlayer(const std::string& ownerID);
   GNE::Mutex mMutex;
};

#endif // _DEBUG
