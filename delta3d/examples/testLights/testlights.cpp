#include "testlights.h"
#include "dtCore/dt.h"
//#include "gui_fl/guimgr.h"

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
   terrain->LoadFile("room-int-walls.ive");
   AddDrawable( terrain );

   // create a spot light.
   SpotLight* myLight1 = new SpotLight( 1, "spotlight", Light::GLOBAL);

   Transform l1 = Transform( 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f );
   myLight1->SetTransform(&l1);

   myLight1->SetAmbient( 1.0f, 0.0f, 0.0f, 1.0f );
   myLight1->SetDiffuse( 1.0f, 0.0f, 0.0f, 1.0f );
   myLight1->SetSpotCutoff( 20.0f );
   myLight1->SetSpotExponent( 50.0f );
  
   GetScene()->AddLight( myLight1 );

   // create a positional light.
   PositionalLight* myLight2 = new PositionalLight( 2, "poslight", Light::GLOBAL);

   l1.Set( 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f );
   myLight2->SetTransform(&l1);

   myLight2->SetAmbient( 0.0f, 1.0f, 0.0f, 1.0f );
   myLight2->SetDiffuse( 0.0f, 1.0f, 0.0f, 1.0f );
   myLight2->SetAttenuation( 1.0f, 2.0f/20, 2.0f/osg::square(20) );

   Object* barrel = new Object( "Barrel" );
   barrel->LoadFile( "physics/sphere/happy_sphere.ive" );
   barrel->SetTransform(&l1);
   AddDrawable( barrel );

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

      /*
      PositionalLight* plight = static_cast<PositionalLight*>(GetScene()->GetLight( 2 ));
      
      float red = 0.0f;
      float g = 0.0f;
      float b = 0.0f;
      float a = 0.0f;
      plight->GetAmbient( &red, &g, &b, &a );

      red = red + 0.01f;
      g = g + 0.05f;
      b = b + 0.10f;

      if( red > 1.0f )
         red = red - 1.0f;

      if( g > 1.0f )
         g = g - 1.0f;

      if( b > 1.0f )
         b = b - 1.0f;

      plight->SetAmbient( red, g, b, a );
      plight->SetDiffuse( red, g, b, a );
      */
      
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