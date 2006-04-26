#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

using namespace dtCore;
using namespace dtABC;

int main()
{

   SetDataFilePathList( GetDeltaRootPath() + "/examples/testApp/;" +
                        GetDeltaDataPathList()  );
                        
   RefPtr<Application> app = new Application( "config.xml" );

   //load some terrain
   RefPtr<Object> terrain = new Object( "Terrain" );
   terrain->LoadFile( "models/dirt.ive" );
   app->AddDrawable( terrain.get() );

   //load an object
   RefPtr<Object> brdm = new Object( "BRDM" );
   brdm->LoadFile( "models/brdm.ive" );
   app->AddDrawable( brdm.get() );
   Transform trans = Transform( 0.0f, 0.0f, 0.0f, 90.0f, 0.0f, 0.0f );
   brdm->SetTransform( &trans );
   
   //adjust the Camera position
   Transform camPos;
   osg::Vec3 camXYZ( 0.f, -50.f, 20.f );
   osg::Vec3 lookAtXYZ ( 0.f, 0.f, 0.f );
   osg::Vec3 upVec ( 0.f, 0.f, 1.f );
   camPos.SetLookAt( camXYZ, lookAtXYZ, upVec );
   app->GetCamera()->SetTransform( &camPos );

   app->Config();
   app->Run();

   return 0;
}
