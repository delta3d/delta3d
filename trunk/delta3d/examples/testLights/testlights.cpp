#include "testlights.h"
#include "dtCore/dt.h"

// namespaces
using namespace   dtABC;
using namespace   dtCore;

IMPLEMENT_MANAGEMENT_LAYER( TestLightsApp )

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
   //terrain->LoadFile( "room/shoothouse-1story-sans-lites.ive" );
   //terrain->LoadFile( "Darken/csar-noshadow.ive" );
   //terrain->LoadFile("bsp_gamelevel.ive");
   terrain->LoadFile( "dirt/dirt.ive" );
   AddDrawable( terrain );

   Object* happy1 = new Object( "Happy1" );
   happy1->LoadFile( "physics/sphere/happy_sphere.ive" );
   Transform t = Transform( -1.0f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f );
   happy1->SetTransform(&t);
   AddDrawable( happy1 );

   Object* happy2 = new Object( "Happy2" );
   happy2->LoadFile( "physics/sphere/happy_sphere.ive" );
   t.Set( 25.0f, 18.0f, 1.0f, 0.0f, 0.0f, 0.0f );
   happy2->SetTransform( &t );
   AddDrawable( happy2 );

   // create a spot light.
   Light* myLight1 = new Light( 1, "spotlight", Light::GLOBAL);
   myLight1->SetPosition( 0.0f, 0.0f, 20.0f );
   myLight1->SetAmbient( 1.0f, 0.0f, 0.0f, 1.0f );
   myLight1->SetDiffuse( 1.0f, 0.0f, 0.0f, 1.0f );
   myLight1->SetSpotCutoff( 20.0f );
   myLight1->SetSpotExponent( 50.0f );
   myLight1->SetDirection( 1.0f, 1.0f, -1.0f );
   AddDrawable( myLight1 );

   // create a local light.
   Light* myLight2 = new Light( 2, "local light", Light::GLOBAL );
   myLight2->SetPosition( 3.0f, -8.0f, 2.0f );
   myLight2->SetAmbient( 0.0f ,1.0f ,1.0f ,1.0f );
   myLight2->SetDiffuse( 0.0f, 1.0f, 1.0f, 1.0f );
   myLight2->SetConstantAttenuation( 1.0f );
   myLight2->SetLinearAttenuation( 2.0f/20 );
   myLight2->SetQuadraticAttenuation( 2.0f/osg::square(20) );
   AddDrawable( myLight2 );

   Transform position;
   position.Set(3.0f, -8.0f, 2.0f, 0.0f, 0.0f, 0.0f );
   GetCamera()->SetTransform( &position );

   //remove:
   sgVec3 camLoc;
   position.GetTranslation( camLoc );

   sgVec3 origin = { 0.0f, 0.0f, 0.0f };

   OrbitMotionModel* omm = new OrbitMotionModel( GetKeyboard(), GetMouse() );
   omm->SetTarget(GetCamera());
   omm->SetTarget(GetCamera());
   omm->SetDistance( sgDistanceVec3( camLoc, origin ) );

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