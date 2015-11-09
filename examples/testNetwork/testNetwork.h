/* -*-c++-*-
 * testNetwork - testNetwork (.h & .cpp) - Using 'The MIT License'
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
#ifndef DELTA_TestNetwork
#define DELTA_TestNetwork

#include <dtABC/application.h>
#include <dtABC/labelactor.h>
#include <dtCore/refptr.h>
#include <dtCore/flymotionmodel.h>

#include "mynetwork.h"

namespace dtCore
{
   class Object;
}

////////////////////////////////////////////////////////////////////////////////
class TestNetwork : public dtABC::Application
{
   public:
      TestNetwork(const std::string& hostName, const std::string& configFilename = "testnetworkconfig.xml");
   protected:
      virtual ~TestNetwork() {}
   public:
      virtual void Config();

      bool KeyPressed(const dtCore::Keyboard* keyboard, int key);

      virtual void PreFrame(const double deltaFrameTime);
      virtual void Frame(const double deltaFrameTime);
      virtual void PostFrame(const double deltaFrameTime);
      virtual void Quit();

   private:

      /// send our position out to all connections
      void SendPosition();

      void CreateHelpLabel();
      std::string CreateHelpLabelText();

      dtCore::RefPtr<MyNetwork> mNet; ///<Reference the NetMgr derived class
      std::string mHostName; ///<The hostname to connect to (if we're a client)
      dtCore::RefPtr<dtCore::Object> mTerrain; ///<Ground
      dtCore::RefPtr<dtCore::FlyMotionModel> mMotion; ///<Motion model
      dtCore::RefPtr<dtABC::LabelActor> mLabel; ///<Help Label

      bool mQuitRequested;
};

#endif // DELTA_TestNetwork
