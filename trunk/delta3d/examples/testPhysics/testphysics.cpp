#include "dtCore/dt.h"
#include "dtABC/dtabc.h"
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

   DECLARE_MANAGEMENT_LAYER(TestPhysicsApp)
public:

   float random(float min,float max) { return min + (max-min)*(float)rand()/(float)RAND_MAX; }

   TestPhysicsApp( std::string configFile = "config.xml" )
      : Application( configFile )
   {

      RefPtr<Object> obj1 = new Object("Ground");
      RefPtr<Object> obj2 = new Object("FallingCrate");
      RefPtr<Object> obj3 = new Object("GroundCrate");

      //load the model files
      if (!obj1->LoadFile("models/dirt.ive")) return;
      if (!obj2->LoadFile("models/physics_crate.ive")) return; 
      if (!obj3->LoadFile("models/physics_crate.ive")) return; 

      //position the camera
      Transform position;
      position.Set(0.0f, -20.0f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
      GetCamera()->SetTransform(&position);

      //position first falling crate
      position.Set(0.55f, 0.0f, 3.0f, 0.0f, 40.0f, 0.0f, 1.0f, 1.0f, 1.0f);
      obj2->SetTransform( &position );

      //position the crate on the ground
      position.Set(0.0f, 0.f, 0.525f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
      obj3->SetTransform( &position );

      float lx = 1.0f;
      float ly = 1.0f;
      float lz = 1.0f;

      //create collision meshes
      obj1->SetCollisionMesh(); 
      obj2->SetCollisionBox();
      obj3->SetCollisionBox();

      //set the mass for objects
      dMass mass;
      dMassSetBox(&mass, 1, lx, ly, lz);
      obj2->SetMass(&mass);
      obj3->SetMass(&mass);

      //turn on the physics
      obj2->EnableDynamics();
      obj3->EnableDynamics();

      //Add the objects to the Scene to be rendered
      GetScene()->AddDrawable( obj1.get() );
      GetScene()->AddDrawable( obj2.get() );
      GetScene()->AddDrawable( obj3.get() );

      //put the falling crate in the vector of dropped objects
      mObjects.push_back(obj2);

      GetScene()->SetGravity(0, 0, -15.f);

      updater = new Updater(GetScene());

      omm = new OrbitMotionModel( GetKeyboard(), GetMouse() );
      omm->SetTarget( GetCamera() );

      //calculate and set focal distance for orbit motion model (origin -> camera)
      Transform trans;
      GetCamera()->GetTransform(&trans);

      osg::Vec3 camLoc;
      trans.GetTranslation( camLoc );

      osg::Vec3 origin (0.0f, 0.0f, 0.0f);
      omm->SetDistance( (camLoc - origin).length() );
   }

protected:

   virtual void PreFrame( const double deltaFrameTiime )
   {
      while( !mToAdd.empty() )
      {
         GetScene()->AddDrawable( mToAdd.front().get() );
         mObjects.push_back( mToAdd.front() );
         mToAdd.pop();
      }
      
      while( !mToRemove.empty() )
      {
         GetScene()->RemoveDrawable( mToRemove.front().get() );
         mObjects.pop_front();
         mToRemove.pop();
      }   
   }

   //apply some linear and angular velocity dampening
   void DampenBody( Object *obj, float vScale, float aScale )
   {
      dBodyID ID = obj->GetBodyID();

      if( !dBodyIsEnabled( ID ) ) {return;}

      dReal const * V = dBodyGetLinearVel( ID );
      dBodyAddForce( ID, vScale*V[0], vScale*V[1], vScale*V[2] );
      dReal const * A = dBodyGetAngularVel( ID );
      dBodyAddTorque( ID, aScale*A[0], aScale*A[1], aScale*A[2] );
   }

   virtual void PostFrame( const double deltaFrameTime )
   {
      std::deque< RefPtr<Object> >::iterator i;
      for (i= mObjects.begin(); i!=mObjects.end(); i++)
      {
         Object *obj = i->get();
         DampenBody(obj, -0.03f, -0.04f );
      }
   }

   virtual void KeyPressed(dtCore::Keyboard* keyboard, 
      Producer::KeyboardKey key,
      Producer::KeyCharacter character)
   {
      if (key == Producer::Key_Escape)
      {
         while( !mToAdd.empty() )    mToAdd.pop();
         while( !mToRemove.empty() ) mToRemove.pop();
         while( !mObjects.empty() )  mObjects.pop_front();
 
         this->Quit();
      }
      
      if (key == Producer::Key_B)
      {
         if( mObjects.size() < kLimit )
         {
            RefPtr<Object> box = new Object("box");
            box->LoadFile("models/physics_crate.ive");
 
            Transform xform(random(-2.f,2.f),
               random(-2.f, 2.f),
               random(5.f, 10.f),
               random(0.f, 180.f),
               random(0.f, 90.f),
               random(0.f, 90.f)      
               );

            float randomScale = random(0.5f, 2.0f);
            xform.SetScale( randomScale, randomScale, randomScale );

            box->SetTransform(&xform);

            float lx = 1.0f;
            float ly = 1.0f;
            float lz = 1.0f;

            box->SetCollisionBox();

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
            RefPtr<Object> sphere = new Object("sphere");
            sphere->LoadFile("models/physics_happy_sphere.ive");

            Transform xform(random(-2.f,2.f),
               random(-2.f, 2.f),
               random(5.f, 10.f),
               random(0.f, 180.f),
               random(0.f, 90.f),
               random(0.f, 90.f));

            float randomScale = random(0.5f, 2.0f);
            xform.SetScale( randomScale, randomScale, randomScale );

            sphere->SetTransform(&xform);

            float radius = 0.5f;

            sphere->SetCollisionSphere();
            
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
            RefPtr<Object> cyl = new Object("cylinder");
            cyl->LoadFile("models/physics_barrel.ive");

            Transform xform(random(-2.f,2.f),
               random(-2.f, 2.f),
               random(5.f, 10.f),
               random(0.f, 180.f),
               random(0.f, 90.f),
               random(0.f, 90.f));

            float randomScale = random(0.5f, 2.0f);
            xform.SetScale( randomScale, randomScale, randomScale );

            cyl->SetTransform(&xform);

            float radius = 0.321f; 
            float length = 1.0f;            

            cyl->SetCollisionCappedCylinder();

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

   static std::queue< RefPtr<Object> > mToAdd;
   static std::queue< RefPtr<Object> > mToRemove;
   static std::deque< RefPtr<Object> > mObjects;

   protected:
   RefPtr<Updater> updater;
   RefPtr<OrbitMotionModel> omm;
};

IMPLEMENT_MANAGEMENT_LAYER(TestPhysicsApp)

const unsigned int TestPhysicsApp::kLimit = 50;

std::queue< RefPtr<Object> > TestPhysicsApp::mToAdd;
std::queue< RefPtr<Object> > TestPhysicsApp::mToRemove;
std::deque< RefPtr<Object> > TestPhysicsApp::mObjects;


int main( int argc, char **argv )
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testPhysics/;" +
                        GetDeltaDataPathList()  );


   RefPtr<TestPhysicsApp> app = new TestPhysicsApp( "config.xml" );

   app->Config();
   app->Run();

   return 0;
}
