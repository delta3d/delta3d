#include <dtCore/dt.h>
#include <dtABC/dtabc.h>

#include "bumpmapdrawable.h"

using namespace dtABC;
using namespace dtCore;

class TestBumpMapApp : public Application
{

	DECLARE_MANAGEMENT_LAYER( TestBumpMapApp )

public:
	TestBumpMapApp( const std::string& configFilename = "config.xml" )
		: Application( configFilename )
	{
      mWireframe = false;

      GetWindow()->SetWindowTitle("testBumpMap");
      //initialize our drawable
		mBumpMapDrawable = new BumpMapDrawable();
      mBumpMapDrawable->Initialize();

      //add it to the scene
      AddDrawable(mBumpMapDrawable.get());

      //setup the camera
      mMotionModel = new FlyMotionModel( GetKeyboard(), GetMouse() );
      mMotionModel->SetTarget(GetCamera());
      GetCamera()->SetPerspective(90.0f, 70.0f, 1.0f, 5000.0f);
		Transform xform(0.0f, -300.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		GetCamera()->SetTransform(&xform);

	}

   //make the light go in a circle
   void PreFrame( const double deltaFrameTime )
   {
      static float lightRadius = 850.0f;
      static float lightHeight = 300.0f;
      static float radPerSec   = osg::DegreesToRadians(45.0f);
      static float totalTime = 0.0f;

      totalTime += deltaFrameTime;
      osg::Vec3 lp;
      
      lp[0] = lightRadius * cosf(radPerSec * totalTime);
      lp[1] = lightRadius * sinf(radPerSec * totalTime);
      lp[2] = lightHeight;

      osg::Vec3 ep;
      Transform trans;
      GetCamera()->GetTransform(&trans);
      trans.GetTranslation(ep);


      mBumpMapDrawable->SetUniforms(lp, ep);      

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
         mWireframe = !mWireframe;
         mBumpMapDrawable->SetWireframe(mWireframe);
      }
   }

	~TestBumpMapApp()
	{

	}

private:

	RefPtr<FlyMotionModel>	                  mMotionModel;
   RefPtr<BumpMapDrawable>                   mBumpMapDrawable;

   bool                                      mWireframe;

};

IMPLEMENT_MANAGEMENT_LAYER( TestBumpMapApp )

int main(int argc, char* argv[])
{
   
	SetDataFilePathList( GetDeltaRootPath() + "/examples/testBumpMap/;" +
		                  GetDeltaDataPathList()  );

	RefPtr<TestBumpMapApp> app = new TestBumpMapApp( "config.xml" );
	app->Config();
	app->Run();

	return 0;
}
