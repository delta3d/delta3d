/* -*-c++-*-
* testprerender - testprerender (.h & .cpp) - Using 'The MIT License'
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
* Michael Guerrero
*/

#include "testprerender.h"

#include <dtCore/globals.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/deltawin.h>
#include <dtCore/transform.h>

#include <dtDAL/project.h>
#include <dtDAL/map.h>

#include <osg/Texture2D>
#include <osgGA/GUIEventAdapter>

////////////////////////////////////////////////////////////////////////////////
TestPreRender::TestPreRender(const std::string& mainSceneObjectName,
                             const std::string& configFilename /*= "config.xml"*/)
   : Application(configFilename)
{
   LoadGeometry(mainSceneObjectName);

   dtCore::Transform objectTransform;
   mBoxObject->GetTransform(objectTransform);

   // Adjust the positioning of the camera depending on the size of the object
   CenterCameraOnObject(mBoxObject.get());

   int x, y, width, height;
   GetWindow()->GetPosition(x, y, width, height);

   CreateTextureScene();
   CreateTextureTarget(width, height);
   CreateTextureCamera(width, height);

   // Create the view and attach the associated resources
   mTextureView = new dtCore::View;
   mTextureView->SetScene(mTextureScene.get());
   mTextureView->SetCamera(mTextureCamera.get());

   AddView(*mTextureView);

   // Give the camera a texture to render to
   mTextureCamera->GetOSGCamera()->attach(osg::Camera::COLOR_BUFFER, mTextureTarget.get());

   // Override the default texture for the model and apply our rendered texture
   osg::StateSet* ss = mBoxObject->GetOSGNode()->getOrCreateStateSet();
   ss->setTextureAttributeAndModes(0, mTextureTarget.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

   // Apply the motion model to control the light centered around our object
   mOrbitMotion = new dtCore::OrbitMotionModel(GetKeyboard(), GetMouse());
   mOrbitMotion->SetTarget(mTextureCamera.get());
}


////////////////////////////////////////////////////////////////////////////////
void TestPreRender::LoadGeometry(const std::string& customObjectName)
{
   mBoxObject = new dtCore::Object("Main Scene Object");
   mBoxObject->LoadFile(customObjectName);
   AddDrawable(mBoxObject.get());
}

////////////////////////////////////////////////////////////////////////////////
bool TestPreRender::KeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   bool verdict(false);

   switch(key)
   {
   case osgGA::GUIEventAdapter::KEY_Escape:
      {
         this->Quit();
         verdict = true;
         break;
      }
   case osgGA::GUIEventAdapter::KEY_Return:

      if (keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Alt_L) ||
          keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Alt_R))
      {
         bool onOrOff = !GetWindow()->GetFullScreenMode();
         GetWindow()->SetFullScreenMode(onOrOff);
      }
   }

   return verdict;
}

////////////////////////////////////////////////////////////////////////////////
void TestPreRender::PreFrame(const double deltaFrameTime)
{
   static float mTotalTime = 0.0f;
   mTotalTime += deltaFrameTime * 0.15f;

   osg::Matrix rotateMat;
   rotateMat.makeRotate(osg::DegreesToRadians(30.0f) * mTotalTime, osg::Vec3(1.0f, 0.0f, 1.0f));

   dtCore::Transform objectTransform;
   mBoxObject->GetTransform(objectTransform);
   objectTransform.SetRotation(rotateMat);

   mBoxObject->SetTransform(objectTransform);
}

////////////////////////////////////////////////////////////////////////////////
void TestPreRender::CenterCameraOnObject(dtCore::Object* object)
{
   osg::Vec3 center;
   float radius;

   object->GetBoundingSphere(&center, &radius);

   // position the camera slightly behind the origin
   dtCore::Transform cameraTransform;
   cameraTransform.SetTranslation(center -osg::Y_AXIS * radius * 4.0f);

   GetCamera()->SetTransform(cameraTransform);
}

////////////////////////////////////////////////////////////////////////////////
void TestPreRender::CreateTextureScene()
{
   // Create a new custom scene
   mTextureScene = new dtCore::Scene;

   // Set the root directory where our art assets are to be found
   std::string contextName = dtCore::GetDeltaRootPath() + "/examples/data/demoMap";
   dtDAL::Project::GetInstance().SetContext(contextName, true);

   // Load the map into our custom scene
   dtDAL::Map& map = dtDAL::Project::GetInstance().LoadMapIntoScene("MyCoolMap", *mTextureScene);

   std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > container;
   map.FindProxies(container, "", "", "", "dtCore::SkyBox");

   // Add the skybox to the main scene as well
   GetScene()->AddDrawable(container[0]->GetActor());
}

////////////////////////////////////////////////////////////////////////////////
void TestPreRender::CreateTextureCamera(int width, int height)
{
   // Create the delta camera
   mTextureCamera = new dtCore::Camera("Texture Camera");
   mTextureCamera->SetWindow(GetWindow());
   mTextureCamera->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);

   // Make the necessary changes directly to the osg camera
   osg::Camera* osgCam = mTextureCamera->GetOSGCamera();
   osgCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   osgCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
   osgCam->setRenderOrder(osg::Camera::PRE_RENDER);
   osgCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
   osgCam->setViewport(0, 0, width, height);
}

////////////////////////////////////////////////////////////////////////////////
void TestPreRender::CreateTextureTarget(int width, int height)
{
   // All of these parameters must be set for frame buffer objects (FBO's) to work
   mTextureTarget = new osg::Texture2D;
   mTextureTarget->setTextureSize(width, height);
   mTextureTarget->setInternalFormat(GL_RGBA);
   mTextureTarget->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
   mTextureTarget->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);

   // Extra non essential parameters
   mTextureTarget->setResizeNonPowerOfTwoHint(false);
   mTextureTarget->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
   mTextureTarget->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
   mTextureTarget->setBorderColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

////////////////////////////////////////////////////////////////////////////////
TestPreRender::~TestPreRender()
{
   // This needs to be done in order to ensure that our
   // manually loaded map can properly deallocate resources
   dtDAL::Project& project = dtDAL::Project::GetInstance();
   project.CloseAllMaps(true);
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
   std::string mainObjectName = "models/physics_crate.ive";

   // Allow specifying of custom geometry from the command line
   if (argc > 1)
   {
      mainObjectName = std::string(argv[1]);
   }

   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" +
                               dtCore::GetDeltaRootPath() + "/examples/data" + ";" +
                               dtCore::GetDeltaRootPath() + "/examples/testPreRender");

   // We need to set custom display settings before the creating the app
   osg::DisplaySettings* display = osg::DisplaySettings::instance();
   display->setMinimumNumStencilBits(8);

   dtCore::RefPtr<TestPreRender> app = new TestPreRender(mainObjectName, "config.xml");
   app->Config();
   app->Run();

   return 0;
}
