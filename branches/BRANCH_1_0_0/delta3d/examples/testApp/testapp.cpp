#include "dt.h"
#include "dtabc.h"

using namespace dtCore;
using namespace dtABC;

int main()
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );

   Application *app = new Application( "config.xml" );

   //load some terrain
   Object *terrain = new Object( "Terrain" );
   terrain->LoadFile( "dirt/dirt.ive" );
   app->AddDrawable( terrain );

   //load an object
   Object *brdm = new Object( "BRDM" );
   brdm->LoadFile( "BRDM/brdm-2.ive" );
   app->AddDrawable( brdm );
   Transform trans = Transform( 0.0f, 0.0f, 0.0f, 90.0f, 0.0f, 0.0f );
   brdm->SetTransform( &trans );
   
   //adjust the Camera position
   Transform camPos;
   sgVec3 camXYZ={ 0.f, -50.f, 20.f };
   sgVec3 lookAtXYZ = { 0.f, 0.f, 0.f };
   sgVec3 upVec = { 0.f, 0.f, 1.f };
   camPos.SetLookAt( camXYZ, lookAtXYZ, upVec );
   app->GetCamera()->SetTransform( &camPos );

   app->Config();
   app->Run();

   delete app;

   return 0;
}
