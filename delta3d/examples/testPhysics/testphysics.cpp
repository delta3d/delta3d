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
      if (!obj2->LoadFile("box.flt")) return;

      Weather* weather = new Weather();   
      GetScene()->AddDrawable( weather->GetEnvironment() );    

      Transform position;
      position.Set(0.f, -10.f, 2.f, 0.f, 0.f, 0.f);
      GetCamera()->SetTransform(&position);

      position.Set(0.55, 0, 3, 0, 40, 0);
      obj2->SetTransform( &position );

      obj1->SetCollisionMesh();
      obj2->SetCollisionBox();

      dMass mass;

      dMassSetBox(&mass, 1, 1, 1, 1);

      obj2->SetMass(&mass);

      obj2->EnableDynamics();
      obj2->RenderCollisionGeometry();

      //Add the Objects to the Scene to be rendered
      GetScene()->AddDrawable( obj1 );
      GetScene()->AddDrawable( obj2 );

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
         Object *box = new Object("box");
         Transform xform(random(-2.f,2.f),
            random(-2.f, 2.f),
            random(5.f, 10.f),
            random(0.f, 180.f),
            random(0.f, 90.f),
            random(0.f, 90.f));
         box->SetTransform(&xform);
         float lx = random(0.5, 3.f);
         float ly = random(0.5, 3.f);
         float lz = random(0.5, 3.f);
         box->SetCollisionBox(lx, ly, lz);
         box->RenderCollisionGeometry();

         dMass mass;
         dMassSetBox(&mass, 1, lx, ly, lz);
         box->SetMass(&mass);
         box->EnableDynamics();

         GetScene()->AddDrawable(box);
      }

      if (key == Producer::Key_S)
      {
         Object *sphere = new Object("sphere");

         Transform xform(random(-2.f,2.f),
            random(-2.f, 2.f),
            random(5.f, 10.f),
            random(0.f, 180.f),
            random(0.f, 90.f),
            random(0.f, 90.f));
         sphere->SetTransform(&xform);

         float radius = random(0.5, 2.f);
         sphere->SetCollisionSphere(radius);
         sphere->RenderCollisionGeometry();

         dMass mass;
         dMassSetSphere(&mass, 1, radius);
         sphere->SetMass(&mass);
         sphere->EnableDynamics();

         GetScene()->AddDrawable(sphere);
      }

      if (key == Producer::Key_C)
      {
         Object *cyl = new Object("cylinder");

         Transform xform(random(-2.f,2.f),
            random(-2.f, 2.f),
            random(5.f, 10.f),
            random(0.f, 180.f),
            random(0.f, 90.f),
            random(0.f, 90.f));
         cyl->SetTransform(&xform);

         float radius = random(0.5, 2.f);
         float length = random(0.5, 2.f);
         cyl->SetCollisionCappedCylinder(radius, length);
         cyl->RenderCollisionGeometry();

         dMass mass;
         dMassSetCappedCylinder(&mass, 1, 1, radius, length);
         cyl->SetMass(&mass);
         cyl->EnableDynamics();

         GetScene()->AddDrawable(cyl);
      }
   }
};



int main( int argc, char **argv )
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );

   TestPhysicsApp *app = new TestPhysicsApp( "config.xml" );

   app->Config();
   app->Run();

   delete app;

   return 0;
}
