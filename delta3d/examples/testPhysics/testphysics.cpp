#include "dt.h"
#include "dtabc.h"
#include <ode/ode.h>

using namespace dtCore;
using namespace dtABC;
using namespace std;



class Updater : public Base
{
   DECLARE_MANAGEMENT_LAYER(Updater)

public:

   Updater(Scene* scene)
   {
      AddSender(scene);
   }

   virtual void OnMessage(MessageData *data)
   {
      if(data->message == "collision")
      {
         /*
         Scene::CollisionData* cd = 
         (Scene::CollisionData*)data->userData;

         Notify(NOTICE) << "collision detected between " <<
         dynamic_cast<Base*>(cd->mBodies[0])->GetName() << " and " <<
         dynamic_cast<Base*>(cd->mBodies[1])->GetName() << " at (" <<
         cd->mLocation[0] << ", " << cd->mLocation[1] << ", " <<
         cd->mLocation[2] << ")" << endl;
         */
      }
   }
};

IMPLEMENT_MANAGEMENT_LAYER(Updater)

class TestPhysicsApp : public Application
{
public:

   float random(float min,float max) { return min + (max-min)*(float)rand()/(float)RAND_MAX; }

   TestPhysicsApp( std::string configFile = "config.xml" )
      : Application( configFile )
   {
      //This is where we'll find our files to load

      Object *obj1 = new Object("ground");
      Object *obj2 = new Object("box");

      if (!obj1->LoadFile("ground.flt")) return;
      if (!obj2->LoadFile("physics/crate/crate.ive")) return; 

      //position first falling crate
      Transform position;
      position.Set(0.f, -10.f, 2.f, 0.f, 0.f, 0.f);
      GetCamera()->SetTransform(&position);

      position.Set(0.55, 0, 3, 0, 40, 0);
      obj2->SetTransform( &position );

      //set ODE parameters
      obj1->SetCollisionMesh();
      obj2->SetCollisionBox();

      dMass mass;
      dMassSetBox(&mass, 1, 1.0f, 1.0f, 1.0f);
      obj2->SetMass(&mass);

      //turn on the physics
      obj2->EnableDynamics();
      //obj2->RenderCollisionGeometry();

      //Add the Objects to the Scene to be rendered
      GetScene()->AddDrawable( obj1 );
      GetScene()->AddDrawable( obj2 );

      mObjects.push(obj2);

      GetScene()->SetGravity(0, 0, -15.f);

      Updater *updater = new Updater(GetScene());

      Transform trans;
      GetCamera()->GetTransform(&trans);

      sgVec3 camLoc;
      trans.GetTranslation( camLoc );

      sgVec3 origin = {0.0f, 0.0f, 0.0f};

      OrbitMotionModel* omm = new OrbitMotionModel( GetKeyboard(), GetMouse() );
      omm->SetTarget( GetCamera() );
      omm->SetDistance( sgDistanceVec3( camLoc, origin ) );
   }
   ~TestPhysicsApp(){}

protected:

   virtual void PreFrame( const double deltaFrameTiime )
   {
      while( !mToAdd.empty() )
      {
         GetScene()->AddDrawable( mToAdd.front() );
         mObjects.push( mToAdd.front() );
         mToAdd.pop();
      }
      
      while( !mToRemove.empty() )
      {
         GetScene()->RemoveDrawable( mToRemove.front() );
         mObjects.pop();
         mToRemove.pop();
      }
   
   }

   virtual void KeyPressed(dtCore::Keyboard* keyboard, 
      Producer::KeyboardKey key,
      Producer::KeyCharacter character)
   {
      if (key == Producer::Key_Escape)
      {
         this->Quit();
      }

      if (key == Producer::Key_B)
      {
         if( mObjects.size() < kLimit )
         {
            Object *box = new Object("box");
            box->LoadFile("physics/crate/crate.ive");

            Transform xform(random(-2.f,2.f),
               random(-2.f, 2.f),
               random(5.f, 10.f),
               random(0.f, 180.f),
               random(0.f, 90.f),
               random(0.f, 90.f));
            box->SetTransform(&xform);

            box->SetCollisionBox();
            //box->RenderCollisionGeometry();

            float lx = 1.0f;
            float ly = 1.0f;
            float lz = 1.0f;

            dMass mass;
            dMassSetBox(&mass, 1, lx, ly, lz);
            box->SetMass(&mass);

            box->EnableDynamics();
            
            mToAdd.push( box );
         }
         else
         {
            mToRemove.push( mObjects.front() );
         }
      }

      if (key == Producer::Key_S)
      {
         if( mObjects.size() < kLimit )
         {
            Object *sphere = new Object("sphere");
            sphere->LoadFile("physics/sphere/happy_sphere.ive");

            Transform xform(random(-2.f,2.f),
               random(-2.f, 2.f),
               random(5.f, 10.f),
               random(0.f, 180.f),
               random(0.f, 90.f),
               random(0.f, 90.f));
            sphere->SetTransform(&xform);

            sphere->SetCollisionSphere();
            //sphere->RenderCollisionGeometry();

            float radius = 0.5f;

            dMass mass;
            dMassSetSphere(&mass, 1, radius);
            sphere->SetMass(&mass);
            sphere->EnableDynamics();
     
             mToAdd.push( sphere );
         }
         else
         {         
            mToRemove.push( mObjects.front() );
         }
      }

      if (key == Producer::Key_C)
      {
         if( mObjects.size() < kLimit )
         {
            Object *cyl = new Object("cylinder");
            cyl->LoadFile("physics/barrel/barrel.ive");

            Transform xform(random(-2.f,2.f),
               random(-2.f, 2.f),
               random(5.f, 10.f),
               random(0.f, 180.f),
               random(0.f, 90.f),
               random(0.f, 90.f));
            cyl->SetTransform(&xform);

            cyl->SetCollisionCappedCylinder();
            //cyl->RenderCollisionGeometry();

            float radius = 0.321f; 
            float length = 1.0f;

            dMass mass;
            dMassSetCappedCylinder(&mass, 1, 2, radius, length);
            cyl->SetMass(&mass);

            cyl->EnableDynamics();

            mToAdd.push( cyl );
         }
         else
         {
            mToRemove.push( mObjects.front() );
         }
      }
   }

   static const unsigned int kLimit;

   static std::queue<Object*> mToAdd;
   static std::queue<Object*> mToRemove;
   static std::queue<Object*> mObjects;
};

const unsigned int TestPhysicsApp::kLimit = 50;

std::queue<Object*> TestPhysicsApp::mToAdd;
std::queue<Object*> TestPhysicsApp::mToRemove;
std::queue<Object*> TestPhysicsApp::mObjects;


int main( int argc, char **argv )
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );

   TestPhysicsApp *app = new TestPhysicsApp( "config.xml" );

   app->Config();
   app->Run();

   delete app;

   return 0;
}
