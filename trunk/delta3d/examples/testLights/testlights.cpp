#include "testlights.h"
#include <dtCore/globals.h>

using namespace dtABC;
using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER( TestLightsApp )

float TestLightsApp::countOne = 0.0f;
float TestLightsApp::countTwo = 0.0f;
float TestLightsApp::countThree = 0.0f;

TestLightsApp::TestLightsApp( const std::string& configFilename )
:  Application( configFilename )
{}

void TestLightsApp::Config()
{
   // turn off scene light so we only see the lights we create
   GetScene()->UseSceneLight( false );

   // load up a warehouse
   mWarehouse = new Object( "Warehouse" );
   mWarehouse->LoadFile( "models/warehouse.ive" );
   AddDrawable( mWarehouse.get() );

   Transform trans;

   // create a global spot light.
   mGlobalSpot = new SpotLight( 1, "GlobalSpotlight" );
   trans.Set( 5.0f, 8.0f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f );
   mGlobalSpot->SetTransform( &trans );
   mGlobalSpot->SetSpotCutoff( 20.0f );
   mGlobalSpot->SetSpotExponent( 50.0f );
   GetScene()->AddDrawable( mGlobalSpot.get() );

   // add a child to the local light
   mSphere = new Object( "HappySphere" );
   mSphere->LoadFile( "models/physics_happy_sphere.ive" );
   
   // we want the sphere 1 unit below light so we can see effect of local light and
   trans.Set( 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f ); 
   mSphere->SetTransform( &trans );

   // create a global positional light.
   mPositional = new PositionalLight( 3, "PositionalLight" );
   mPositional->SetDiffuse( 1.0f, 1.0f, 0.0f, 1.0f ); // yellow light
   mPositional->AddChild( mSphere.get() ); //move sphere along with light
   GetScene()->AddDrawable( mPositional.get() );
   mPositional->SetEnabled( false );

   // create an infinite light
   mGlobalInfinite = new InfiniteLight( 4, "GlobalInfiniteLight" );
   GetScene()->AddDrawable( mGlobalInfinite.get() );
   mGlobalInfinite->SetEnabled( false );

   // set camera stuff
   trans.Set( 30.0f, -20.0f, 25.0f, 40.0f, -33.0f, 0.0f, 1.0f, 1.0f, 1.0f );
   GetCamera()->SetTransform( &trans );

   osg::Vec3 camLoc, origin;
   trans.GetTranslation( camLoc );

   mOmm = new OrbitMotionModel( GetKeyboard(), GetMouse() );
   mOmm->SetTarget( GetCamera() );

   float distance( 0.0f );
   for( int i = 0; i < 3; i++ )
      distance += osg::square( camLoc[i] - origin[i] );
   distance = sqrt( distance );

   mOmm->SetDistance( distance );
}

void TestLightsApp::KeyPressed(  Keyboard*               keyboard, 
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
      mPositional->SetEnabled( !mPositional->GetEnabled() );
      break;
   case Producer::Key_3:
   {
      if( mPositional->GetLightingMode() == Light::GLOBAL )
         mPositional->SetLightingMode( Light::LOCAL );
      else
         mPositional->SetLightingMode( Light::GLOBAL );

      break;
   }
   case Producer::Key_4:
      mGlobalInfinite->SetEnabled( !mGlobalInfinite->GetEnabled() );
      break;
   default:
      break;
   }
}

void TestLightsApp::PreFrame( const double deltaFrameTime )
{
   // increment some values at different rates
   countOne +=50.0f*deltaFrameTime;
   countTwo += 60.0f*deltaFrameTime;
   countThree += 70.0f*deltaFrameTime;

   // cap at 360
   if( countOne > 360.0f ) countOne -= 360.0f;
   if( countTwo > 360.0f ) countTwo -= 360.0f;
   if( countThree > 360.0f ) countThree -= 360.0f;
   
   // scale values to 0.0-1.0
   float redValue = ( cos( osg::DegreesToRadians(countOne) ) + 1.0f ) / 2.0f;
   float greenValue = ( cos( osg::DegreesToRadians(countTwo) ) + 1.0f ) / 2.0f;
   float blueValue = ( cos( osg::DegreesToRadians(countThree) ) + 1.0f ) / 2.0f;
   
   // modify all global lights
   mGlobalSpot->SetDiffuse( redValue, greenValue, blueValue, 1.0f ); //change color
   
   // rotate the spotlight
   Transform trans;
   mGlobalSpot->GetTransform( &trans );
   trans.SetRotation( countOne, 0.0f, 0.0f );
   mGlobalSpot->SetTransform( &trans );
   
   mPositional->SetAttenuation( 1.0f, greenValue/2.0f, blueValue/2.0f ); //change attenutation
   
   // move the global positional light in a circle
   float tx = 1.5*cos( osg::DegreesToRadians(countOne) ) + 2.0f;
   float ty = 1.5*sin( osg::DegreesToRadians(countOne) ) + 5.0f;
   trans.Set( tx, ty, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f ); 
   mPositional->SetTransform( &trans );
   
   mGlobalInfinite->SetDiffuse( redValue, greenValue, blueValue, 1.0f ); //change color
   
   float th = countOne;
   float tp = countTwo;
   
   mGlobalInfinite->SetAzimuthElevation( th, tp ); //change direction
}


int main( int argc, const char* argv[] )
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testLights/;" +
                        GetDeltaDataPathList()  );


   RefPtr<TestLightsApp> app = new TestLightsApp( "config.xml" );
   app->Config();
   app->Run();

   return 0;
}
