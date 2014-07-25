/* -*-c++-*-
 * testShaders - testshaders (.h & .cpp) - Using 'The MIT License'
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
#include <dtABC/labelactor.h>
#include <dtCore/object.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/shadermanager.h>
#include <dtCore/transform.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/project.h>

#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osgGA/GUIEventAdapter>


using namespace dtABC;
using namespace dtCore;

class TestShadersApp : public Application
{

public:
   TestShadersApp(const std::string& configFilename = "testshadersconfig.xml")
      : Application(configFilename)
   {
      mTotalTime = 0.0f;

      std::string contextName = dtUtil::GetDeltaRootPath() + "/examples/data";
      dtCore::Project::GetInstance().SetContext(contextName, true);

      LoadGeometry();
      EnableShaders();

      Transform xform(0.0f, -6.0f, 0.0f, 0.0f, 0.0f, 0.0f);

      GetCamera()->SetTransform(xform);

      // Auto near far does not take into account changes from the geometry shader
      GetCamera()->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);

      double vfov, aspectRatio, nearClip, farClip;
      GetCamera()->GetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);

      // The geometry shader seems to be very sensitive to the value for the near plane
      GetCamera()->SetPerspectiveParams(vfov, aspectRatio, 2.0, 100.0); 

      GetWindow()->SetWindowTitle("testShaders");

      CreateHelpLabel();
   }

protected:

   virtual ~TestShadersApp() {}

public:

   void LoadGeometry()
   {
      osg::Sphere* sphere = new osg::Sphere;
      
      osg::ShapeDrawable* sphereDrawable = new osg::ShapeDrawable(sphere);
      sphereDrawable->setUseDisplayList(false);
      sphereDrawable->setUseVertexBufferObjects(true);

      mGeode = new osg::Geode;
      mGeode->addDrawable(sphereDrawable);

      mObject = new dtCore::Transformable;
      mObject->GetMatrixNode()->addChild(mGeode);
      
      AddDrawable(mObject.get());
   }

   void EnableShaders()
   {
      dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();
      shaderManager.LoadAndAssignShader(*mObject, "Shaders/testshader.dtshader");

      mEnabled = true;
   }

   void DisableShaders()
   {
      dtCore::ShaderManager::GetInstance().UnassignShader(*mObject);
      mEnabled = false;
   }

   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key)
   {
      bool verdict(false);
      if (key == osgGA::GUIEventAdapter::KEY_Escape)
      {
         this->Quit();
         verdict = true;
      }
      else if (key == osgGA::GUIEventAdapter::KEY_Space)
      {
         if (mEnabled)
         {
            DisableShaders();
         }
         else
         {
            EnableShaders();
         }
         verdict = true;
      }
      else if (key == osgGA::GUIEventAdapter::KEY_F1)
      {
         // Disable until the gui is compatible with the geom shader
         mLabel->SetActive(!mLabel->GetActive());
         verdict = true;
      }

      return verdict;
   }

   virtual void PreFrame(const double deltaFrameTime)
   {
      mTotalTime += deltaFrameTime;

      osg::Matrix rotateMat;
      rotateMat.makeRotate(osg::DegreesToRadians(30.0f) * mTotalTime, osg::Vec3(0.0f, 0.0f, 1.0f));
      Transform xform;
      mObject->GetTransform(xform);
      xform.SetRotation(rotateMat);
      mObject->SetTransform(xform);
   }

private:
   void CreateHelpLabel()
   {
      mLabel = new dtABC::LabelActor();
      osg::Vec2 testSize(17.0f, 2.5f);
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
      testString += "Space: Toggle Shaders\n";

      return testString;
   }

   RefPtr<dtCore::Transformable> mObject;
   RefPtr<osg::Geode> mGeode;

   RefPtr<dtABC::LabelActor> mLabel;

   float mTotalTime;
   bool mEnabled;

};

int main(int argc, char* argv[])
{

   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" +
                               dtUtil::GetDeltaRootPath() + "/examples/data" + ";" +
                               dtUtil::GetDeltaRootPath() + "/examples/testShaders");

   RefPtr<TestShadersApp> app = new TestShadersApp("testshadersconfig.xml");
   app->Config();
   app->Run();

   return 0;
}
