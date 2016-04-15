/* -*-c++-*-
* testbumpmap - testbumpmap (.h & .cpp) - Using 'The MIT License'
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
#include "testbumpmap.h"

#include <dtUtil/geometrycollector.h>
#include <dtUtil/datapathutils.h>

#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/shadermanager.h>
#include <dtCore/transform.h>
#include <dtCore/light.h>

#include <osg/LightSource>
#include <osg/Drawable>
#include <osg/Program>
#include <osg/Texture2D>
#include <osg/Geode>
#include <osgGA/GUIEventAdapter>
#include <osgDB/ReadFile>
#include <osgUtil/TangentSpaceGenerator>


////////////////////////////////////////////////////////////////////////////////
TestBumpMapApp::TestBumpMapApp(const std::string& customObjectName,
                               const std::string& configFilename /*= "config.xml"*/,
                               bool usePrecomputedTangents /*= false*/)
   : Application(configFilename)
   , mPitchAngle(0.0f)
   , mYawAngle(0.0f)
   , mUsePrecomputedTangents(usePrecomputedTangents)
   , mDiffuseTexture(NULL)
   , mNormalTexture(NULL)

{
   //load the xml file which specifies our shaders
   dtCore::ShaderManager& sm = dtCore::ShaderManager::GetInstance();
   sm.LoadShaderDefinitions("shaders/ShaderDefinitions.xml");

   // Load our art assets
   LoadTextures();
   LoadGeometry(customObjectName);

   dtCore::ShaderParamInt* customMode = NULL;
   dtCore::ShaderParamInt* sphereMode = NULL;

   // Assign the bump shader to the nodes
   AssignShaderToObject(mCustomObject.get(), customMode);
   AssignShaderToObject(mSphere.get(), sphereMode);

   // Store pointers to the mode for toggling different paths in the shader
   mCustomShaderMode = customMode;
   mSphereShaderMode = sphereMode;

   dtCore::Transform objectTransform;
   mCustomObject->GetTransform(objectTransform);

   // Apply the motion model to control the light centered around our object
   mOrbitMotion = new dtCore::OrbitMotionModel(GetKeyboard(), GetMouse());
   mOrbitMotion->SetTarget(GetScene()->GetLight(0));

   // Translation doesn't matter with a directional light
   mOrbitMotion->SetLeftRightTranslationAxis(NULL);
   mOrbitMotion->SetUpDownTranslationAxis(NULL);

   // Adjust the positioning of the camera depending on the size of the object
   CenterCameraOnObject(mCustomObject.get());

   CreateHelpLabel();

   mRotationUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "RotationMatrix");
   GetCamera()->GetOSGCamera()->getOrCreateStateSet()->addUniform(mRotationUniform);
   
   SetNextStatisticsType();
}

////////////////////////////////////////////////////////////////////////////////
void TestBumpMapApp::LoadGeometry(const std::string& customObjectName)
{
   // Load a sphere a second object to see the effect on
   // NOTE! This model mirrors the uv coords so the lighting
   // will be incorrect on the backside.
   mSphere = new dtCore::Object("Sphere");
   mSphere->LoadFile("StaticMeshes/physics_happy_sphere.ive");
   mSphere->SetActive(false);
   AddDrawable(mSphere.get());

   mCustomObject = new dtCore::Object("Custom");
   mCustomObject->LoadFile(customObjectName);
   AddDrawable(mCustomObject.get());

   // Load some geometry to represent the direction of the light
   mLightObject = new dtCore::Object("Light Arrow");
   mLightObject->LoadFile("StaticMeshes/LightArrow.ive");
   mLightObject->SetScale(osg::Vec3(0.5f, 0.5f, 0.5f));

   AddDrawable(mLightObject.get());

   dtCore::Light* light = GetScene()->GetLight(0);

   // Infinite lights must start here, point light from the postive y axis
   light->GetLightSource()->getLight()->setPosition(osg::Vec4(osg::Y_AXIS, 0.0f));

   // Calculate tangent vectors from the geometry for use in tangent space calculations
   GenerateTangentsForObject(mSphere.get());
   GenerateTangentsForObject(mCustomObject.get());
}

////////////////////////////////////////////////////////////////////////////////
void TestBumpMapApp::LoadTextures()
{
   osg::Image* diffuseImage = osgDB::readImageFile("textures/Fieldstone.tga");
   osg::Image* normalImage  = osgDB::readImageFile("textures/Fieldstone_NORM.tga");
   osg::Image* specImage    = osgDB::readImageFile("textures/Fieldstone_SPEC.tga");

   mDiffuseTexture = new osg::Texture2D;
   mDiffuseTexture->setImage(diffuseImage);
   mDiffuseTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
   mDiffuseTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
   mDiffuseTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
   mDiffuseTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
   mDiffuseTexture->setMaxAnisotropy(8);

   mNormalTexture = new osg::Texture2D;
   mNormalTexture->setImage(normalImage);
   mNormalTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
   mNormalTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
   mNormalTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
   mNormalTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
   mNormalTexture->setMaxAnisotropy(8);

   mSpecularTexture = new osg::Texture2D;
   mSpecularTexture->setImage(specImage);
   mSpecularTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
   mSpecularTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
   mSpecularTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
   mSpecularTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
   mSpecularTexture->setMaxAnisotropy(8);
}

////////////////////////////////////////////////////////////////////////////////
bool TestBumpMapApp::KeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   bool verdict(false);

   switch(key)
   {
   case osgGA::GUIEventAdapter::KEY_Left:
      {
         mYawAngle += 15.0f;
         verdict = true;
         break;
      }
   case osgGA::GUIEventAdapter::KEY_Right:
      {
         mYawAngle -= 15.0f;
         verdict = true;
         break;
      }
   case osgGA::GUIEventAdapter::KEY_Up:
      {
         mPitchAngle += 15.0f;
         verdict = true;
         break;
      }
   case osgGA::GUIEventAdapter::KEY_Down:
      {
         mPitchAngle -= 15.0f;
         verdict = true;
         break;
      }
   case osgGA::GUIEventAdapter::KEY_Space:
      {
         static bool renderToggle = false;
         renderToggle = !renderToggle;

         dtCore::Object* current = (renderToggle) ? mSphere.get(): mCustomObject.get();
         CenterCameraOnObject(current);

         mSphere->SetActive(renderToggle);
         mCustomObject->SetActive(!renderToggle);

         break;
      }
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
      {
         int value = key - 48;

         // Offset the ascii value to get the numeric one
         mCustomShaderMode->SetValue(value);
         mSphereShaderMode->SetValue(value);

         break;
      }

   case osgGA::GUIEventAdapter::KEY_F1:
      {
         mLabel->SetActive(!mLabel->GetActive());
         break;
      }
   }

   return verdict;
}

////////////////////////////////////////////////////////////////////////////////
void TestBumpMapApp::PreFrame(const double deltaFrameTime)
{
   float angleIncrement = deltaFrameTime * 2.0f;
   mYawAngle += angleIncrement;
   mPitchAngle += angleIncrement;

   osg::Matrix rotateMat;
   rotateMat.makeRotate(osg::DegreesToRadians(mYawAngle), osg::Vec3(0.0f, 0.0f, 1.0f),
                        osg::DegreesToRadians(mPitchAngle), osg::Vec3(1.0f, 0.0f, 0.0f),
                        osg::DegreesToRadians(0.0f), osg::Vec3(0.0f, 1.0f, 0.0f));

   dtCore::Transform objectTransform;
   mCustomObject->GetTransform(objectTransform);
   objectTransform.SetRotation(rotateMat);

   // lazily set both
   mCustomObject->SetTransform(objectTransform);
   mSphere->SetTransform(objectTransform);

   // Update the transform of the light arrow to match the light position
   dtCore::Transform lightTransform;
   GetScene()->GetLight(0)->GetTransform(lightTransform);
   mLightObject->SetTransform(lightTransform);

   mRotationUniform->set(rotateMat);
}

////////////////////////////////////////////////////////////////////////////////
void TestBumpMapApp::GenerateTangentsForObject(dtCore::Object* object)
{
   // Override texture values in the geometry to ensure that we can apply normal mapping
   osg::StateSet* ss = object->GetOSGNode()->getOrCreateStateSet();
   ss->setTextureAttributeAndModes(0, mDiffuseTexture.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   ss->setTextureAttributeAndModes(1, mNormalTexture.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   ss->setTextureAttributeAndModes(2, mSpecularTexture.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

   // We only need to compute the tangents if the shader is going to use them
   if (mUsePrecomputedTangents)
   {
      // Get all geometry in the graph to apply the shader to
      osg::ref_ptr<dtUtil::GeometryCollector> geomCollector = new dtUtil::GeometryCollector;
      object->GetOSGNode()->accept(*geomCollector);

      // Calculate tangent vectors for all faces and store them as vertex attributes
      for (size_t geomIndex = 0; geomIndex < geomCollector->mGeomList.size(); ++geomIndex)
      {
         osg::Geometry* geom = geomCollector->mGeomList[geomIndex];

         osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator;
         tsg->generate(geom, 0);

         if (!geom->getVertexAttribArray(6))
         {
            //geom->setVertexAttribData(6, osg::Geometry::ArrayData(tsg->getTangentArray(), osg::Geometry::BIND_PER_VERTEX, GL_FALSE));
            geom->setVertexAttribArray(6, tsg->getTangentArray());
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void TestBumpMapApp::AssignShaderToObject(dtCore::Object* object, dtCore::ShaderParamInt*& outMode)
{
   dtCore::ShaderManager& sm = dtCore::ShaderManager::GetInstance();

   std::string shaderName = (mUsePrecomputedTangents) ? "TestBumpMap": "AttributelessBump";

   dtCore::ShaderProgram* sp = sm.FindShaderPrototype(shaderName, "TestShaders");

   if (sp != NULL)
   {
      dtCore::ShaderProgram* boundProgram = sm.AssignShaderFromPrototype(*sp, *object->GetOSGNode());

      osg::Program* osgProgram = boundProgram->GetShaderProgram();
      outMode = dynamic_cast<dtCore::ShaderParamInt*>(boundProgram->FindParameter("mode"));

      if (mUsePrecomputedTangents)
      {
         // Hook up the vertex attrib to the location where it is created
         osgProgram->addBindAttribLocation("tangentAttrib", 6);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void TestBumpMapApp::CenterCameraOnObject(dtCore::Object* object)
{
   osg::Vec3 center;
   float radius;

   object->GetBoundingSphere(center, radius);

   // position the camera slightly behind the origin
   dtCore::Transform cameraTransform;
   cameraTransform.SetTranslation(center -osg::Y_AXIS * radius * 4.0f);

   // Move our light icon to the outer bounds of the object
   mOrbitMotion->SetDistance(radius);
   mOrbitMotion->SetFocalPoint(center);

   GetCamera()->SetTransform(cameraTransform);
}

////////////////////////////////////////////////////////////////////////////////
void TestBumpMapApp::CreateHelpLabel()
{
   mLabel = new dtABC::LabelActor();
   osg::Vec2 testSize(20.5f, 3.5f);
   mLabel->SetBackSize(testSize);
   mLabel->SetFontSize(0.8f);
   mLabel->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::LEFT_CENTER);
   mLabel->SetText(CreateHelpLabelText());
   mLabel->SetEnableDepthTesting(false);
   mLabel->SetEnableLighting(false);

   GetCamera()->AddChild(mLabel.get());
   dtCore::Transform labelOffset(-17.0f, 50.0f, 11.25f, 0.0f, 90.0f, 0.0f);
   mLabel->SetTransform(labelOffset, dtCore::Transformable::REL_CS);
   AddDrawable(GetCamera());
}

////////////////////////////////////////////////////////////////////////////////
std::string TestBumpMapApp::CreateHelpLabelText()
{
   std::string testString("");
   testString += "F1: Toggle Help Screen\n";
   testString += "\n";
   testString += "Space: Toggle sphere/cube\n";
   testString += "0-9: Set Shader Mode to 0-9\n";

   return testString;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
   std::string customObjectName = "StaticMeshes/physics_crate.ive";

   // Allow specifying of custom geometry from the command line
   if (argc > 1)
   {
      customObjectName = std::string(argv[1]);
   }

   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" +
                               dtUtil::GetDeltaRootPath() + "/examples/data" + ";" +
                               dtUtil::GetDeltaRootPath() + "/examples/testBumpMap");

   dtCore::RefPtr<TestBumpMapApp> app = new TestBumpMapApp(customObjectName, "config.xml", true);
   app->Config();
   app->Run();

   return 0;
}
