#include "testlights.h"


using namespace   dtABC;
using namespace   dtCore;

IMPLEMENT_MANAGEMENT_LAYER( TestLightsApp )

float TestLightsApp::countOne = 0.0f;
float TestLightsApp::countTwo = 0.0f;
float TestLightsApp::countThree = 0.0f;

TestLightsApp::TestLightsApp( std::string configFilename )
:  Application( configFilename )
{}

TestLightsApp::~TestLightsApp()
{}


void
TestLightsApp::Config()
{
   // turn off scene light so we only see the lights we create
   GetScene()->UseSceneLight( false );

   // load up a warehouse
   mWarehouse = new Object( "Warehouse" );
   mWarehouse->LoadFile( "warehouse/room-int-walls.ive" );
   AddDrawable( mWarehouse.get() );



   Transform trans;

   // create a global spot light.
   mGlobalSpot = new SpotLight( 1, "GlobalSpotlight" );
   trans.Set( 5.0f, 10.0f, 2.0f, 0.0f, 0.0f, 0.0f );
   mGlobalSpot->SetTransform( &trans );
   mGlobalSpot->SetSpotCutoff( 20.0f );
   mGlobalSpot->SetSpotExponent( 50.0f );

   GetScene()->AddLight( mGlobalSpot.get() );



   // add a child to the local light
   mSphere = new Object( "HappySphere" );
   mSphere->LoadFile( "physics/sphere/happy_sphere.ive" );



   // create a local positional light.
   mLocalPositional = new PositionalLight( 2, "LocalPositionalLight", Light::LOCAL );
   mLocalPositional->SetDiffuse( 1.0f, 1.0f, 0.0f, 1.0f ); // yellow light
   mLocalPositional->AddLightChild ( mSphere.get() ); //light only the sphere

   GetScene()->AddLight( mLocalPositional.get() );
   mLocalPositional->SetEnabled( false );



   // create a global positional light.
   mGlobalPositional = new PositionalLight( 3, "GlobalPositionalLight" );
   mGlobalPositional->SetDiffuse( 1.0f, 1.0f, 0.0f, 1.0f ); // yellow light
   mGlobalPositional->AddChild( mSphere.get() ); //move sphere along with light

   GetScene()->AddLight( mGlobalPositional.get() );
   mGlobalPositional->SetEnabled( false );



   // create an infinite light
   mGlobalInfinite = new InfiniteLight( 4, "GlobalInfiniteLight" );

   GetScene()->AddLight( mGlobalInfinite.get() );
   mGlobalInfinite->SetEnabled( false );



   //set camera stuff
   trans.Set( 30.0f, -20.0f, 25.0f, 40.0f, -33.0f, 0.0f );
   GetCamera()->SetTransform( &trans );

   sgVec3 camLoc; 
   sgVec3 origin = { 0.0f, 0.0f, 0.0f };

   trans.GetTranslation( camLoc );

   mOmm = new OrbitMotionModel( GetKeyboard(), GetMouse() );
   mOmm->SetTarget( GetCamera() );
   mOmm->SetDistance( sgDistanceVec3( camLoc, origin ) );

}

void
TestLightsApp::KeyPressed( Keyboard*               keyboard, 
                           Producer::KeyboardKey   key,
                           Producer::KeyCharacter  character )
{
   switch( key )
   {
   case Producer::Key_Escape:
      Quit();
      break;
   case Producer::Key_1: 
      mGlobalSpot->SetEnabled( !mGlobalSpot->GetEnabled() );
      break;
   case Producer::Key_2:
      mLocalPositional->SetEnabled( !mLocalPositional->GetEnabled() );
      break;
   case Producer::Key_3: 
      mGlobalPositional->SetEnabled( !mGlobalPositional->GetEnabled() );
      break;
   case Producer::Key_4:
      mGlobalInfinite->SetEnabled( !mGlobalInfinite->GetEnabled() );
      break;
   default:
      break;
   }
}

void 
TestLightsApp::OnMessage( Base::MessageData* data )
{
   if(data->message == "preframe")
   {
      //increment some values at different rates
      countOne +=0.5f;
      countTwo += 0.6f;
      countThree += 0.7f;

      //cap at 360
      if( countOne > 360.0f ) countOne -= 360.0f;
      if( countTwo > 360.0f ) countTwo -= 360.0f;
      if( countThree > 360.0f ) countThree -= 360.0f;
    
      //scale values to 0.0-1.0
      float redValue = ( sgCos( countOne ) + 1.0f ) / 2.0f;
      float greenValue = ( sgCos( countTwo ) + 1.0f ) / 2.0f;
      float blueValue = ( sgCos( countThree ) + 1.0f ) / 2.0f;

      // modify all global lights
      mGlobalSpot->SetDiffuse( redValue, greenValue, blueValue, 1.0f ); //change color

      //rotate the spotlight
      Transform trans;
      mGlobalSpot->GetTransform( &trans );
      trans.SetRotation( countOne, 0.0f, 0.0f );
      mGlobalSpot->SetTransform( &trans );

      mGlobalPositional->SetAttenuation( 1.0f, greenValue/2.0f, blueValue/2.0f ); //change attenutation
      
      //move the global positional light in a circle
      float tx = 2*sgCos( countOne ) + 3.0f;
      float ty = 2*sgSin( countOne ) + 7.0f;
      trans.Set( tx, ty, 2.0f, 0.0f, 0.0f, 0.0f ); 
      mGlobalPositional->SetTransform( &trans );
         
      mGlobalInfinite->SetDiffuse( redValue, greenValue, blueValue, 1.0f ); //change color
      mGlobalInfinite->SetDirection( countOne, countTwo, countThree ); //change direction
   }
}


int 
main( int argc, const char* argv[] )
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );

   osg::ref_ptr<TestLightsApp> app = new TestLightsApp( "config.xml" );
   app->Config();
   app->Run();

   return 0;
}
