/* -*-c++-*-
 * testMap - testmap (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Eddie Johnson
 * Chris Osborn
 * Erik Johnson
 */

/// Demo application illustrating map loading and actor manipulation
#include <dtABC/application.h>
#include <dtABC/beziercontroller.h>
#include <dtABC/labelactor.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/globals.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>

using namespace dtCore;
using namespace dtDAL;
using namespace dtABC;

class TestMap : public Application
{
public:
   TestMap()
      : Application("testMap.xml")
      , mMotionModel(NULL)
      , mMap(NULL)
   {
      // Set up a motion model so we may move the camera
      mMotionModel = new OrbitMotionModel(GetKeyboard(), GetMouse());
      mMotionModel->SetTarget(GetCamera());

      // Steps to load a map into Delta3D
      // 1. Set the project context, just like in the editor
      // 2. Get a reference to the map
      // 3. Load the map into the scene through the project class
      std::string contextName = dtCore::GetDeltaRootPath() + "/examples/data/demoMap";
      Project::GetInstance().SetContext(contextName, true);
      mMap = &Project::GetInstance().GetMap("MyCoolMap");

      //Since we are in an Application we can simply call...
      LoadMap(*mMap);

      // translate the camera to the predefined start position
      {
         std::vector< dtCore::RefPtr<ActorProxy> > proxies;
         mMap->FindProxies(proxies, "startPosition");
         if (!proxies.empty())
         {
            Transformable* startPoint;
            proxies[0]->GetActor(startPoint);

            if (startPoint != NULL)
            {
               Transform xform;
               startPoint->GetTransform(xform);
               GetCamera()->SetTransform(xform);
            }
         }
      }

      CreateHelpLabel();
   }

   virtual bool KeyReleased(const dtCore::Keyboard* keyboard, int kc)
   {
      if (kc == 'r')
      {
         Reset();
         return true;
      }
      else if (kc == osgGA::GUIEventAdapter::KEY_F1)
      {
         mLabel->SetActive(!mLabel->GetActive());
         return true;
      }
      else
      {
         return false;
      }
   }

   // Re-initialize to default conditions
   void Reset()
   {
      std::vector< dtCore::RefPtr<ActorProxy> > proxies;
      mMap->FindProxies(proxies, "Controller0");
      if (!proxies.empty())
      {
         BezierController* controller;
         proxies[0]->GetActor(controller);

         if (controller != NULL)
         {
            controller->Start();
         }
      }
   }

protected:

   virtual ~TestMap()
   {
   }

private:
   void CreateHelpLabel()
   {
      mLabel = new dtABC::LabelActor();
      osg::Vec2 testSize(27.0f, 2.5f);
      mLabel->SetBackSize(testSize);
      mLabel->SetFontSize(0.8f);
      mLabel->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::LEFT_CENTER);
      mLabel->SetText(CreateHelpLabelText());
      mLabel->SetEnableDepthTesting(false);
      mLabel->SetEnableLighting(false);

      GetCamera()->AddChild(mLabel.get());
      dtCore::Transform labelOffset(-17.0f, 50.0f, 11.75f, 0.0f, 90.0f, 0.0f);
      mLabel->SetTransform(labelOffset, dtCore::Transformable::REL_CS);
      AddDrawable(GetCamera());
   }

   std::string CreateHelpLabelText()
   {
      std::string testString("");
      testString += "F1: Toggle Help Screen\n";
      testString += "\n";
      testString += "r: Reset Helicopter (if at end of cycle)\n";

      return testString;
   }

   RefPtr<MotionModel> mMotionModel;
   RefPtr<Map> mMap;
   RefPtr<dtABC::LabelActor> mLabel;
};

int main()
{
   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" +
                               dtCore::GetDeltaRootPath() + "/examples/data" + ";");
   RefPtr<TestMap> app;

   try
   {
      app = new TestMap;
      app->Config();
      app->Run();
   }
   catch (const dtUtil::Exception& e)
   {
      std::cout << e.What() << '\n';
   }

   return 0;
}
