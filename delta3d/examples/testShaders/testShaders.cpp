#include "dtCore/dt.h"
#include "dtABC/dtabc.h"
#include "gui_fl/guimgr.h"

#include <dtCore/object.h>

#include <osg/Drawable>
#include <osg/PrimitiveSet>
#include <osg/Program>


using namespace dtABC;
using namespace dtCore;

class TestShadersApp : public Application
{

   DECLARE_MANAGEMENT_LAYER( TestShadersApp )

public:
   TestShadersApp( std::string configFilename = "config.xml" )
      : Application( configFilename )
   {

      LoadGeometry();
      EnableShaders();

      mMotionModel = new dtCore::OrbitMotionModel(GetKeyboard(), GetMouse());
      mMotionModel->SetAngularRate(0.5f);
      mMotionModel->SetLinearRate(0.5f);
      mMotionModel->SetTarget(GetCamera());

      Transform xform(0.0f, -10.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      GetCamera()->SetTransform(&xform);

   }


   void LoadGeometry()
   {
      mObject = new dtCore::Object;
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

      bool vertLoaded = vertexShader->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/testshader.vert");

      bool fragLoaded = fragmentShader->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/testshader.frag");

      ss->setAttributeAndModes( mProg.get(), osg::StateAttribute::ON );
      mEnabled = false;
   }


   virtual void KeyPressed(dtCore::Keyboard* keyboard, 
      Producer::KeyboardKey key,
      Producer::KeyCharacter character)
   {
      if (key == Producer::Key_Escape)
      {
         this->Quit();
      }
      else if(key == Producer::Key_space)
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
      }
   }


   ~TestShadersApp()
   {
    
   }

private:

   RefPtr<dtCore::OrbitMotionModel>          mMotionModel;
   RefPtr<dtCore::Object>                    mObject;

   RefPtr<osg::Program>                      mProg; 
   RefPtr<osg::Program>                      mDefault;

   bool                                      mEnabled;


};

IMPLEMENT_MANAGEMENT_LAYER( TestShadersApp )

int main(int argc, char* argv[])
{

   SetDataFilePathList( GetDeltaRootPath() + "/examples/testShaders/;" +
      GetDeltaDataPathList()  );

   RefPtr<TestShadersApp> app = new TestShadersApp( "config.xml" );
   app->Config();
   app->Run();


   return 0;
}
