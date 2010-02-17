/* -*-c++-*-
* testLights - testlights (.h & .cpp) - Using 'The MIT License'
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
#ifndef DELTA_TESTLIGHTS
#define DELTA_TESTLIGHTS

#include <dtCore/object.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/infinitelight.h>
#include <dtCore/positionallight.h>
#include <dtCore/spotlight.h>
#include <dtABC/application.h>
#include <dtABC/labelactor.h>

class TestLightsApp : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( TestLightsApp )

public:

   TestLightsApp( const std::string& configFilename = "config.xml" );

protected:

   virtual ~TestLightsApp() {}

public:

   void Config();

   bool KeyPressed(const dtCore::Keyboard* keyboard, int key);

   void PreFrame( const double deltaFrameTime );

private:
   void CreateHelpLabel();
   std::string CreateHelpLabelText();

   //static values between 0-360 used to calculate new colors + positions
   static float countOne;
   static float countTwo;
   static float countThree;

   dtCore::RefPtr<dtCore::Object> mWarehouse;
   dtCore::RefPtr<dtCore::Object> mSphere;

   dtCore::RefPtr<dtCore::SpotLight> mGlobalSpot;
   dtCore::RefPtr<dtCore::PositionalLight> mPositional;
   dtCore::RefPtr<dtCore::InfiniteLight> mGlobalInfinite;

   dtCore::RefPtr<dtCore::OrbitMotionModel> mOmm;

   dtCore::RefPtr<dtABC::LabelActor> mLabel;
};

#endif // DELTA_TESTLIGHTS
