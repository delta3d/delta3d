#include <dtCore/dt.h>
#include <dtABC/dtabc.h>

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
   TestShadersApp( std::string configFilename = "testshadersconfig.xml" )
      : Application( configFilename )
   {
      mTotalTime = 0.0f;

      LoadGeometry();
      EnableShaders();

      Transform xform(0.0f, -3.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      GetCamera()->SetTransform(&xform);

      GetWindow()->SetWindowTitle("testShaders");

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

      vertexShader->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/testshader.vert");
      fragmentShader->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/testshader.frag");

      ss->setAttributeAndModes( mProg.get(), osg::StateAttribute::ON );
      mEnabled = true;
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


   virtual void PreFrame(const double deltaFrameTime )
   {
      mTotalTime += deltaFrameTime;

      osg::Matrix rotateMat;
      rotateMat.makeRotate(osg::DegreesToRadians(30.0f) * mTotalTime, osg::Vec3(0.0f, 0.0f, 1.0f));
      Transform xform;
      mObject->GetTransform(&xform);
      xform.SetRotation(rotateMat);
      mObject->SetTransform(&xform);
   }


   ~TestShadersApp()
   {
    
   }

private:

   RefPtr<dtCore::Object>                    mObject;

   RefPtr<osg::Program>                      mProg; 
   RefPtr<osg::Program>                      mDefault;

   float                                     mTotalTime;
   bool                                      mEnabled;

   DeltaWin::Resolution                      mRes;
};

IMPLEMENT_MANAGEMENT_LAYER( TestShadersApp )

int main(int argc, char* argv[])
{

   SetDataFilePathList( GetDeltaRootPath() + "/examples/testShaders/;" +
      GetDeltaDataPathList()  );

   RefPtr<TestShadersApp> app = new TestShadersApp( "testshadersconfig.xml" );
   app->Config();
   app->Run();


   return 0;
}
