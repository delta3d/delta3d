#include "testlights.h"
#include "dtCore/dt.h"
//#include "gui_fl/guimgr.h"

// namespaces
using namespace   dtABC;
using namespace   dtCore;

IMPLEMENT_MANAGEMENT_LAYER( TestLightsApp )

float TestLightsApp::redCount = 0.0f;
float TestLightsApp::greenCount = 0.0f;
float TestLightsApp::blueCount = 0.0f;

TestLightsApp::TestLightsApp( std::string configFilename /*= "config.xml"*/ )
:  Application(configFilename)
{
}

TestLightsApp::~TestLightsApp()
{
}

void
TestLightsApp::Config()
{
   GetScene()->UseSceneLight( false );

   Object* terrain = new Object( "Terrain" );
   terrain->LoadFile("room-int-walls.ive");
   AddDrawable( terrain );

   // create a spot light.
   SpotLight* myLight1 = new SpotLight( 1, "spotlight", Light::GLOBAL);

   Transform l1 = Transform( 5.0f, 10.0f, 2.0f, 0.0f, 0.0f, 0.0f );
   myLight1->SetTransform(&l1);

   myLight1->SetAmbient( 1.0f, 0.0f, 0.0f, 1.0f );
   myLight1->SetDiffuse( 1.0f, 0.0f, 0.0f, 1.0f );
   myLight1->SetSpotCutoff( 20.0f );
   myLight1->SetSpotExponent( 50.0f );
  
   GetScene()->AddLight( myLight1 );

   // create a positional light.
   PositionalLight* myLight2 = new PositionalLight( 2, "poslight", Light::LOCAL);

   l1.Set( 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f );
   myLight2->SetTransform(&l1);

   myLight2->SetAmbient( 0.0f, 1.0f, 0.0f, 1.0f );
   myLight2->SetDiffuse( 0.0f, 1.0f, 0.0f, 1.0f );
   myLight2->SetAttenuation( 1.0f, 2.0f/20, 2.0f/osg::square(20) );

   Object* barrel = new Object( "Barrel" );
   barrel->LoadFile( "physics/sphere/happy_sphere.ive" );
   barrel->SetTransform(&l1);
   //AddDrawable( barrel );
   myLight2->AddLightChild ( barrel );

   GetScene()->AddLight( myLight2 );

   InfiniteLight* myLight3 = new InfiniteLight( 3, "ilight", Light::GLOBAL );
   myLight3->SetAmbient( 0.0f, 0.0f, 1.0f, 1.0f );
   myLight3->SetDiffuse( 0.0f, 0.0f, 1.0f, 1.0f );

   //GetScene()->AddLight( myLight3 );

   //set camera
   Transform position;
   position.Set(0.3f, 0.6f, 1.2f, 0.0f, 0.0f, 0.0f );
   GetCamera()->SetTransform( &position );

   //remove:
   sgVec3 camLoc;
   position.GetTranslation( camLoc );

   sgVec3 origin = { 0.0f, 0.0f, 0.0f };

   OrbitMotionModel* omm = new OrbitMotionModel( GetKeyboard(), GetMouse() );
   omm->SetTarget(GetCamera());
   omm->SetTarget(GetCamera());
   omm->SetDistance( 0 );

   //GUI *ui = new GUI(); 

}

void
TestLightsApp::KeyPressed( Keyboard*               keyboard, 
                           Producer::KeyboardKey   key,
                           Producer::KeyCharacter  character )
{
   switch(key)
   {
   case Producer::Key_Escape:
      Quit();
      break;
   }
}

void 
TestLightsApp::OnMessage( Base::MessageData *data )
{
   if(data->message == "preframe")
   {
      
      Transform trans;
      GetCamera()->GetTransform( &trans );
      float x,y,z,h,p,r;
      trans.Get(&x,&y,&z,&h,&p,&r);
      //Notify(ALWAYS,"(%f,%f,%f) (%f,%f,%f)", x, y, z, h, p, r );

      redCount = redCount + 0.5f;
      greenCount = greenCount + 0.6f;
      blueCount = blueCount + 0.7f;

      if( redCount > 360.0f )
         redCount = 0;

      if( greenCount > 360.0f )
         greenCount = 0;

      if( blueCount > 360.0f )
         blueCount = 0;
    
      float redValue = (cos( (redCount) * SG_DEGREES_TO_RADIANS ) + 1.0f) / 2.0f;
      float greenValue = (cos( (greenCount) * SG_DEGREES_TO_RADIANS ) + 1.0f) / 2.0f;
      float blueValue = (cos( (blueCount) * SG_DEGREES_TO_RADIANS ) + 1.0f) / 2.0f;

      float red = 0.0f;
      float green = 0.0f;
      float blue = 0.0f;
      float alpha = 0.0f;

      SpotLight* plight = static_cast<SpotLight*>(GetScene()->GetLight( 1 ));

      plight->SetAmbient( redValue, greenValue, blueValue, alpha );
      plight->SetDiffuse( redValue, greenValue, blueValue, alpha );

      Transform t;
      plight->GetTransform( &t );
      t.SetRotation( redCount, 0.0f, 0.0f );
      
      plight->SetTransform( &t );
      
      
   }
}

int
main( int argc, const char* argv[] )
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );

   TestLightsApp* app = new TestLightsApp("config.xml");
   app->Config();
   app->Run();

   delete app;
   return   0L;
}