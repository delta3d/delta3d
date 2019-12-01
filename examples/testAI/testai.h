/* -*-c++-*-
* testAI - testai (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2004-2008 MOVES Institute
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
#ifndef DELTA_TestAI
#define DELTA_TestAI

#include <string>

#include <dtCore/refptr.h>
#include <dtABC/application.h>
#include <dtABC/labelactor.h>
#include <dtCore/camera.h>
#include "aicharacter.h"

#include <osg/Matrix>

class TestAI : public dtABC::Application
{

   public:
      TestAI(const std::string& mapName = "TesttownLt", const std::string& configFilename = "config.xml");
   protected:
      /*virtual*/~TestAI();
   public:

      virtual void Config();

      virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key);

      virtual void PreFrame( const double deltaFrameTime );
     
   private:
      void LoadDemoMap(const std::string& pStr);
      bool GoToWaypoint(int pNum);
      void CreateHelpLabel();
      std::string CreateHelpLabelText();

      bool mDrawNavMesh;
      std::string mMapFilename;
      dtCore::RefPtr<dtCore::Camera> mOverheadCamera;
      dtCore::RefPtr<dtAI::AICharacter> mCharacter;
      dtCore::RefPtr<dtABC::LabelActor> mLabel;
      dtAI::Waypoint* mCurrentWaypoint;
      osg::Matrix mCameraOffset; 
};

#endif // DELTA_TestAI
