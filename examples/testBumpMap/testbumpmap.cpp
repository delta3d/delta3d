/* -*-c++-*-
 * testBumpMap - testbumpmap (.h & .cpp) - Using 'The MIT License'
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

#include <dtABC/application.h>
#include <dtCore/globals.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/object.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <osgGA/GUIEventAdapter>

#include "bumpmapdrawable.h"

using namespace dtABC;
using namespace dtCore;

class TestBumpMapApp : public Application
{

public:
   TestBumpMapApp(const std::string& configFilename = "config.xml")
      : Application(configFilename)
   {
      mWireframe = false;

      GetWindow()->SetWindowTitle("testBumpMap");
      //initialize our drawable
      mBumpMapDrawable = new BumpMapDrawable();
      mBumpMapDrawable->Initialize();

      //add it to the scene
      AddDrawable(mBumpMapDrawable.get());

      //setup the camera
      mMotionModel = new FlyMotionModel(GetKeyboard(), GetMouse());
      mMotionModel->SetTarget(GetCamera());
      GetCamera()->SetPerspectiveParams(90.0f, 1.33, 1.0f, 5000.0f);
      Transform xform(0.0f, -300.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      GetCamera()->SetTransform(xform);
   }

   //make the light go in a circle
   void PreFrame(const double deltaFrameTime)
   {
      static float lightRadius = 850.0f;
      static float lightHeight = 300.0f;
      static float radPerSec   = osg::DegreesToRadians(45.0f);
      static float totalTime   = 0.0f;

      totalTime += deltaFrameTime;
      osg::Vec3 lp;

      lp[0] = lightRadius * cosf(radPerSec * totalTime);
      lp[1] = lightRadius * sinf(radPerSec * totalTime);
      lp[2] = lightHeight;

      osg::Vec3 ep;
      Transform trans;
      GetCamera()->GetTransform(trans);
      trans.GetTranslation(ep);

      mBumpMapDrawable->SetUniforms(lp, ep);
   }

   bool KeyPressed(const dtCore::Keyboard* keyboard, int key)
   {
      bool verdict(false);
      if (key == osgGA::GUIEventAdapter::KEY_Escape)
      {
         this->Quit();
         verdict = true;
      }
      else if (key == ' ')
      {
         mWireframe = !mWireframe;
         mBumpMapDrawable->SetWireframe(mWireframe);
         verdict = true;
      }

      return verdict;
   }

   virtual ~TestBumpMapApp()
   {
   }

private:

   RefPtr<FlyMotionModel>                     mMotionModel;
   RefPtr<BumpMapDrawable>                   mBumpMapDrawable;

   bool                                      mWireframe;
};

int main(int argc, char* argv[])
{

   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" +
                               dtCore::GetDeltaRootPath() + "/examples/data" + ";");

   RefPtr<TestBumpMapApp> app = new TestBumpMapApp( "config.xml" );
   app->Config();
   app->Run();

   return 0;
}
