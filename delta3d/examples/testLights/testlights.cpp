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

   Object* warehouse = new Object( "Warehouse" );
   warehouse->LoadFile("room-int-walls.ive");
   AddDrawable( warehouse );



   // create a global spot light.
   SpotLight* globalSpot = new SpotLight( 1, "globalSpotlight", Light::GLOBAL);

   Transform trans = Transform( 5.0f, 10.0f, 2.0f, 0.0f, 0.0f, 0.0f );
   globalSpot->SetTransform( &trans );

   globalSpot->SetAmbient( 1.0f, 0.0f, 0.0f, 1.0f );
   globalSpot->SetDiffuse( 1.0f, 0.0f, 0.0f, 1.0f );
   globalSpot->SetSpotCutoff( 20.0f );
   globalSpot->SetSpotExponent( 50.0f );
  
   GetScene()->AddLight( globalSpot );



   // create a local positional light.
   PositionalLight* localPositional = new PositionalLight( 2, "localPositionalLight", Light::LOCAL);

   trans.Set( 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f );
   localPositional->SetTransform( &trans );

   localPositional->SetAmbient( 0.0f, 1.0f, 0.0f, 1.0f );
   localPositional->SetDiffuse( 0.0f, 1.0f, 0.0f, 1.0f );
   localPositional->SetAttenuation( 1.0f, 2.0f/20, 2.0f/osg::square(20) );

   GetScene()->AddLight( localPositional );

   // add a child to the local light
   Object* sphere = new Object( "Barrel" );
   sphere->LoadFile( "physics/sphere/happy_sphere.ive" );
   sphere->SetTransform( &trans );

   localPositional->AddLightChild ( sphere );





   // create a global positional light.
   PositionalLight* globalPositional = new PositionalLight( 3, "globalPositionalLight", Light::GLOBAL);

   trans.Set( 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f );
   globalPositional->SetTransform( &trans );

   globalPositional->SetAmbient( 0.0f, 1.0f, 0.0f, 1.0f );
   globalPositional->SetDiffuse( 0.0f, 1.0f, 0.0f, 1.0f );
   globalPositional->SetAttenuation( 1.0f, 2.0f/20, 2.0f/osg::square(20) );

   //GetScene()->AddLight( globalPositional );



   // create an infinite light
   InfiniteLight* myLight3 = new InfiniteLight( 4, "ilight", Light::GLOBAL );
   myLight3->SetAmbient( 0.0f, 0.0f, 1.0f, 1.0f );
   myLight3->SetDiffuse( 0.0f, 0.0f, 1.0f, 1.0f );

   //GetScene()->AddLight( myLight3 );



   //set camera
   trans.Set(0.3f, 0.6f, 1.2f, 0.0f, 0.0f, 0.0f );
   GetCamera()->SetTransform( &trans );

   //remove:
   sgVec3 camLoc;
   trans.GetTranslation( camLoc );

   sgVec3 origin = { 0.0f, 0.0f, 0.0f };

   OrbitMotionModel* omm = new OrbitMotionModel( GetKeyboard(), GetMouse() );
   omm->SetTarget( GetCamera() );
   omm->SetTarget( GetCamera() );
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

      SpotLight* globalSpot = static_cast<SpotLight*>(GetScene()->GetLight( 1 ));

      globalSpot->SetAmbient( redValue, greenValue, blueValue, alpha );
      globalSpot->SetDiffuse( redValue, greenValue, blueValue, alpha );

      Transform t;
      globalSpot->GetTransform( &t );
      t.SetRotation( redCount, 0.0f, 0.0f );
      
      globalSpot->SetTransform( &t );
      
      
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