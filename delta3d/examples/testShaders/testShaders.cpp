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

      Transform xform(0.0f, -3.0f, 0.0f, 0.0f, 0.0f, 0.0f);
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

      RefPtr<osg::Shader> vertexShader = new osg::Shader( osg::Shader::VERTEX );
      RefPtr<osg::Shader> fragmentShader = new osg::Shader( osg::Shader::FRAGMENT );

      mProg->addShader( vertexShader.get() );
      mProg->addShader( fragmentShader.get() );

      bool vertLoaded = vertexShader->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/testshader.vert");

      bool fragLoaded = fragmentShader->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/testshader.frag");

      ss->setAttributeAndModes( mProg.get(), osg::StateAttribute::ON );
   }


   ~TestShadersApp()
   {
    
   }

private:

   RefPtr<dtCore::Object>                    mObject;
   RefPtr<osg::Program>                      mProg; 


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
