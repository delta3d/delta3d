#include <dtABC/application.h>
#include <dtCore/globals.h>
#include <dtCore/object.h>
#include <osg/Drawable>
#include <osg/PrimitiveSet>
#include <osg/Program>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>

#include <osgGA/GUIEventAdapter>

using namespace dtABC;
using namespace dtCore;

class TestShadersApp : public Application
{

public:
   TestShadersApp( const std::string& configFilename = "testshadersconfig.xml" )
      : Application( configFilename )
   {
      mTotalTime = 0.0f;

      LoadGeometry();
      EnableShaders();

      Transform xform(0.0f, -3.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      GetCamera()->SetTransform(xform);

      GetWindow()->SetWindowTitle("testShaders");
   }

protected:

   virtual ~TestShadersApp() {}

public:

   void LoadGeometry()
   {
      mObject = new dtCore::Object("Happy Sphere");
      mObject->LoadFile("models/physics_happy_sphere.ive");
      AddDrawable(mObject.get());
   }

   void EnableShaders()
   {
      //create state set
      osg::StateSet* ss = mObject->GetOSGNode()->getOrCreateStateSet();

      //load the shader file
      mProg = new osg::Program;
      mDefault = new osg::Program;

      RefPtr<osg::Shader> vertexShader = new osg::Shader( osg::Shader::VERTEX );
      RefPtr<osg::Shader> fragmentShader = new osg::Shader( osg::Shader::FRAGMENT );

      mProg->addShader( vertexShader.get() );
      mProg->addShader( fragmentShader.get() );

      vertexShader->loadShaderSourceFromFile( dtCore::FindFileInPathList("/shaders/testshader.vert") );
      fragmentShader->loadShaderSourceFromFile( dtCore::FindFileInPathList("/shaders/testshader.frag") );

      ss->setAttributeAndModes( mProg.get(), osg::StateAttribute::ON );
      mEnabled = true;
   }

   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key)
   {
      bool verdict(false);
      if (key == osgGA::GUIEventAdapter::KEY_Escape)
      {
         this->Quit();
         verdict = true;
      }
      else if(key == osgGA::GUIEventAdapter::KEY_Space)
      {
         osg::StateSet* ss = mObject->GetOSGNode()->getOrCreateStateSet(); 

         if(mEnabled)
         {
            ss->setAttributeAndModes( mProg.get(), osg::StateAttribute::OFF );
            ss->setAttributeAndModes( mDefault.get(), osg::StateAttribute::ON );
            mEnabled = false;
         }
         else
         {
            ss->setAttributeAndModes( mDefault.get(), osg::StateAttribute::OFF );
            ss->setAttributeAndModes( mProg.get(), osg::StateAttribute::ON );            
            mEnabled = true;
         }
         verdict = true;
      }

      return verdict;
   }

   virtual void PreFrame(const double deltaFrameTime )
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

   RefPtr<dtCore::Object>                    mObject;

   RefPtr<osg::Program>                      mProg; 
   RefPtr<osg::Program>                      mDefault;

   float                                     mTotalTime;
   bool                                      mEnabled;

   DeltaWin::Resolution                      mRes;
};

int main(int argc, char* argv[])
{

   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" + 
                               dtCore::GetDeltaRootPath() + "/examples/data" + ";" +
                               dtCore::GetDeltaRootPath() + "/examples/testShaders");

   RefPtr<TestShadersApp> app = new TestShadersApp( "testshadersconfig.xml" );
   app->Config();
   app->Run();

   return 0;
}