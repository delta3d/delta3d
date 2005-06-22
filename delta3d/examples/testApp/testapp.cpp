#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

using namespace dtCore;
using namespace dtABC;

typedef dtCore::RefPtr<Object> ObjectPtr;

struct ObjectPtrCompare : std::binary_function<ObjectPtr,ObjectPtr,bool>
{
   /** StatePtrCompare will make sure the State being added is
    * unique to the set based on its name AND based on the fact
    * that the State has a unique place in memory.
    * This makes sure that no one tried to submit a State that
    * had the same name as another State, or someone tried to
    * resubmit a State already in the set by changing its name.
    */
   bool operator()(const ObjectPtr& lhs,const ObjectPtr& rhs) const
   {
      return lhs.get() != rhs.get() && lhs->GetName() < rhs->GetName();
   }
};

typedef std::set<ObjectPtr,ObjectPtrCompare> ObjectPtrSet;

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
   sgVec3 camXYZ={ 0.f, -50.f, 20.f };
   sgVec3 lookAtXYZ = { 0.f, 0.f, 0.f };
   sgVec3 upVec = { 0.f, 0.f, 1.f };
   camPos.SetLookAt( camXYZ, lookAtXYZ, upVec );
   app->GetCamera()->SetTransform( &camPos );

   ObjectPtrCompare compare_them;
   compare_them(brdm,terrain);

   ObjectPtrSet objectSet;
   objectSet.insert(terrain);
   objectSet.insert(brdm);

   for( ObjectPtrSet::iterator iter = objectSet.begin(); iter != objectSet.end(); iter++ )
   {
      std::cout << (*iter)->GetName() << std::endl;    
   }
   app->Config();
   app->Run();

   return 0;
}
