#include <dtABC/application.h>
#include <dtCore/object.h>
#include <dtCore/globals.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <ode/ode.h>
#include <cassert>
#include <queue>

using namespace dtCore;
using namespace dtABC;
using namespace dtUtil;

class Updater : public Base
{

public:
   Updater(Scene* scene) :
      mScene(scene)
   {
      assert( mScene.valid() );
      AddSender( mScene.get() );
   }
protected:
   virtual ~Updater()
   {
      RemoveSender( mScene.get() );
   }

public:

   virtual void OnMessage( MessageData* data )
   {
      if( data->message == "collision" )
      {
         
         //Scene::CollisionData* cd = static_cast< Scene::CollisionData* >( data->userData );

         //Log::GetInstance().LogMessage( Log::LOG_INFO, __FUNCTION__,
         //                               "Collision detected between %s and %s at (%f, %f, %f)",
         //                               dynamic_cast<Base*>(cd->mBodies[0])->GetName().c_str(),
         //                               dynamic_cast<Base*>(cd->mBodies[1])->GetName().c_str(),
         //                               cd->mLocation[0],
         //                               cd->mLocation[1],
         //                               cd->mLocation[2] );
         //
      }
   }
private:
   dtCore::RefPtr<Scene> mScene;
};

class TestPhysicsApp : public Application
{

public:

   float Random( float min, float max ) { return min + (max-min)*float(rand())/float(RAND_MAX); }

   TestPhysicsApp( const std::string& configFile = "config.xml" )
      :  Application( configFile ),
         mLimit(50)
   {
      RefPtr<Object> obj1 = new Object("Ground");
      RefPtr<Object> obj2 = new Object("FallingCrate");
      RefPtr<Object> obj3 = new Object("GroundCrate");

      obj1->LoadFile("models/dirt.ive");
      obj2->LoadFile("models/physics_crate.ive");
      obj3->LoadFile("models/physics_crate.ive");

      //position the camera
      Transform position;
      position.Set( 0.0f, -20.0f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f );
      GetCamera()->SetTransform(&position);

      //position first falling crate
      position.Set( 0.55f, 0.0f, 3.0f, 0.0f, 40.0f, 0.0f, 1.0f, 1.0f, 1.0f );
      obj2->SetTransform(&position);

      //position the crate on the ground
      position.Set( 0.0f, 0.f, 0.525f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f );
      obj3->SetTransform(&position);

      double lx = 1.0;
      double ly = 1.0;
      double lz = 1.0;

      //create collision meshes
      obj1->SetCollisionMesh(); 
      obj2->SetCollisionBox();
      obj3->SetCollisionBox();

      //set the mass for objects
      dMass mass;
      dMassSetBox( &mass, 1.0, lx, ly, lz ) ;
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

      GetScene()->SetGravity( 0.0f, 0.0f, -15.0f );

      updater = new Updater( GetScene() );

      omm = new OrbitMotionModel( GetKeyboard(), GetMouse() );
      omm->SetTarget( GetCamera() );

      //calculate and set focal distance for orbit motion model (origin -> camera)
      Transform trans;
      GetCamera()->GetTransform(&trans);

      osg::Vec3 camLoc;
      trans.GetTranslation(camLoc);

      osg::Vec3 origin( 0.0f, 0.0f, 0.0f );
      omm->SetDistance( (camLoc - origin).length() );
   }

protected:

   virtual ~TestPhysicsApp() {}

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
      if( dBodyID ID = obj->GetBodyID() )
      {
         if( dBodyIsEnabled( ID ) )
         {
            dReal const * V = dBodyGetLinearVel( ID );
            dBodyAddForce( ID, vScale*V[0], vScale*V[1], vScale*V[2] );
            dReal const * A = dBodyGetAngularVel( ID );
            dBodyAddTorque( ID, aScale*A[0], aScale*A[1], aScale*A[2] );
         }
      }
   }

   virtual void PostFrame( const double deltaFrameTime )
   {
      for(  std::deque< RefPtr<Object> >::iterator i = mObjects.begin(); 
            i!=mObjects.end(); 
            ++i )
      {
         DampenBody( i->get(), -0.03f, -0.04f );
      }
   }

   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, 
                              Producer::KeyboardKey key,
                              Producer::KeyCharacter character)
   {
      bool verdict(false);
      switch( key )
      {
         case Producer::Key_P:
         {
            System::Instance()->SetPause( !System::Instance()->GetPause() );
            verdict = true;
            break;
         }
         case Producer::Key_Escape :
         {
            while( !mToAdd.empty() )
            {
               mToAdd.pop();
            }

            while( !mToRemove.empty() )
            {
               mToRemove.pop();
            }

            while( !mObjects.empty() )
            {
               mObjects.pop_front();
            }
            
            Quit();
            verdict = true;
            break;
         }
         case Producer::Key_B :
         {
            if( mObjects.size() < mLimit )
            {
               RefPtr<Object> box = new Object("box");
               box->LoadFile( "models/physics_crate.ive" );
               
               Transform xform(  Random( -2.0f,2.0f ),
                                 Random( -2.0f, 2.0f ),
                                 Random( 5.0f, 10.0f ),
                                 Random( 0.0f, 180.0f ),
                                 Random( 0.0f, 90.0f ),
                                 Random( 0.0f, 90.0f ) );

               float randomScale = Random( 0.5f, 2.0f );
               xform.SetScale( randomScale, randomScale, randomScale );
               
               box->SetTransform(&xform);
            
               double lx = 1.0;
               double ly = 1.0;
               double lz = 1.0;
               
               box->SetCollisionBox();
               
               dMass mass;
               dMassSetBox( &mass, 1.0, lx, ly, lz );
               box->SetMass(&mass);
               
               box->EnableDynamics();
               
               mToAdd.push(box);
            }
            else
            {
               mToRemove.push( mObjects.front() );
            }
            verdict = true;
            break;
         }
         case Producer::Key_S :
         {
            if( mObjects.size() < mLimit )
            {
               RefPtr<Object> sphere = new Object("sphere");
               sphere->LoadFile( "models/physics_happy_sphere.ive" );
               
               Transform xform(  Random( -2.0f, 2.0f ),
                                 Random( -2.0f, 2.0f ),
                                 Random( 5.0f, 10.0f ),
                                 Random( 0.0f, 180.0f ),
                                 Random( 0.0f, 90.0f ),
                                 Random( 0.0f, 90.0f ) );
               
               float randomScale = Random( 0.5f, 2.0f );
               xform.SetScale( randomScale, randomScale, randomScale );
               
               sphere->SetTransform(&xform);
               
               double radius = 0.5;
               
               sphere->SetCollisionSphere();
               
               dMass mass;
               dMassSetSphere( &mass, 1.0, radius );
               sphere->SetMass(&mass);
               sphere->EnableDynamics();
               
               mToAdd.push(sphere);
            }
            else
            {         
               mToRemove.push( mObjects.front() );
            }
            verdict = true;
            break;
         }
         case Producer::Key_C :
         {
            if( mObjects.size() < mLimit )
            {
               RefPtr<Object> cyl = new Object("cylinder");
               cyl->LoadFile( "models/physics_barrel.ive" );
               
               Transform xform(  Random( -2.0f,2.0f),
                                 Random( -2.0f, 2.0f ),
                                 Random( 5.0f, 10.0f ),
                                 Random( 0.0f, 180.0f ),
                                 Random( 0.0f, 90.0f ),
                                 Random( 0.0f, 90.0f ) );
               
               float randomScale = Random( 0.5f, 2.0f );
               xform.SetScale( randomScale, randomScale, randomScale );
               
               cyl->SetTransform(&xform);
               
               double radius = 0.321; 
               double length = 1.0;            
               
               cyl->SetCollisionCappedCylinder();
               
               dMass mass;
               dMassSetCappedCylinder(&mass, 1.0, 2, radius, length);
               cyl->SetMass(&mass);
               
               cyl->EnableDynamics();
               
               mToAdd.push(cyl);
            }
            else
            {
               mToRemove.push( mObjects.front() );
            }
            verdict = true;
            break;
         } 
         default:
         {
            break;
         }
      }

      return verdict;
   }

   private:

   const unsigned int mLimit;

   std::queue< RefPtr<Object> > mToAdd;
   std::queue< RefPtr<Object> > mToRemove;
   std::deque< RefPtr<Object> > mObjects;

   RefPtr<Updater> updater;
   RefPtr<OrbitMotionModel> omm;
};


int main( int argc, char **argv )
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testPhysics/;" +
                        GetDeltaDataPathList()  );


   RefPtr<TestPhysicsApp> app = new TestPhysicsApp( "config.xml" );

   app->Config();
   app->Run();

   return 0;
}
