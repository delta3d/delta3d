#include "testlights.h"
#include "dtCore/dt.h"
#include "gui_fl/guimgr.h"

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
   //terrain->LoadFile( "csar_lighting/csar-noshadow.ive" );
   terrain->LoadFile( "dirt/dirt.ive" );
   //terrain->LoadFile("room.ive");
   AddDrawable( terrain );

   // create a spot light.
   Light* myLight1 = new Light( 1, "spotlight", Light::GLOBAL);

   Transform l1 = Transform( -2.0f, -8.0f, -0.7f, 0.0f, 0.0f, 0.0f );
   myLight1->SetTransform(&l1);

   myLight1->SetAmbient( 1.0f, 0.0f, 0.0f, 1.0f );
   myLight1->SetDiffuse( 1.0f, 0.0f, 0.0f, 1.0f );
   myLight1->SetSpotCutoff( 20.0f );
   myLight1->SetSpotExponent( 50.0f );
   AddDrawable( myLight1 );

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

   GUI *ui = new GUI(); 

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