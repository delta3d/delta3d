#include "dt.h"
#include "dtabc.h"

extern "C"
{
   #include <ode/ode.h>
}

using namespace dtCore;
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

class OrbitMotionModel : public MouseListener
{
   public:

      OrbitMotionModel(Mouse* mouse, Transformable* target)
      {
         mMouse = mouse;
         mTarget = target;

         mMouse->AddMouseListener(this);

         mAzimuth = 0.0f;
         mElevation = 0.0f;
         mDistance = 10.0f;

         sgSetVec3(mCenter, 0, 0, 1);
         
         UpdateTargetTransform();
      }

      virtual void ButtonPressed(Mouse* mouse, MouseButton button)
      {
         mouse->GetPosition(&mLastX, &mLastY);
      }

      virtual void MouseDragged(Mouse* mouse, float x, float y)
      {
         float dX = x - mLastX, dY = y - mLastY;

         if(mouse->GetButtonState(LeftButton))
         {
            mAzimuth -= dX*90;
            mElevation -= dY*90;

            if(mElevation < -90.0f) mElevation = -90.0f;
            else if(mElevation > 90.0f) mElevation = 90.0f;
         }
         
         if(mouse->GetButtonState(MiddleButton))
         {
            mDistance -= dY*20.0f;
            
            if(mDistance < 1.0f) mDistance = 1.0f;
         }
         
         if(mouse->GetButtonState(RightButton))
         {
            Transform transform;
            
            mTarget->GetTransform(&transform);
            
            sgMat4 mat;
            
            transform.Get(mat);
            
            sgVec3 offset = {-dX*mDistance*0.25f, 0, -dY*mDistance*0.25f};
            
            sgXformVec3(offset, mat);
            
            sgAddVec3(mCenter, offset);
         }
         
         UpdateTargetTransform();
         
         mLastX = x;
         mLastY = y;
      }

      virtual void MouseScrolled(Mouse* mouse, int delta)
      {
         mDistance -= delta*5;

         if(mDistance < 1.0f) mDistance = 1.0f;

         UpdateTargetTransform();
      }

   private:

      void UpdateTargetTransform()
      {
         Transform transform;
         
         transform.SetTranslation(
            mCenter[0] + mDistance * sgCos(mElevation) * sgSin(mAzimuth),
            mCenter[1] + mDistance * sgCos(mElevation) * -sgCos(mAzimuth),
            mCenter[2] + mDistance * sgSin(mElevation)
         );
         
         transform.SetRotation(
            mAzimuth, 
            -mElevation, 
            0.0f
         );
         
         mTarget->SetTransform(&transform);
      }

      Mouse* mMouse;

      Transformable* mTarget;

      float mAzimuth, mElevation, mDistance;

      sgVec3 mCenter;
      
      float mLastX, mLastY;
};

class PhysicsApp : public dtABC::Application
{
public:

      float random(float min,float max) { return min + (max-min)*(float)rand()/(float)RAND_MAX; }

      PhysicsApp()
      : Application( "config.xml" )
      {
         //This is where we'll find our files to load

         Object *obj1 = new Object("ground");
         Object *obj2 = new Object("box");

         if (!obj1->LoadFile("ground.flt")) return;

         if (!obj2->LoadFile("box.flt")) return;

         Transform position;
         position.Set(5.f, -10.f, 2.f, 0.f, 0.f, 0.f);
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

         GetScene()->SetGravity(0, 0, -9.8f);

         ::OrbitMotionModel *omm = new ::OrbitMotionModel(GetMouse(), GetCamera());

         Updater *updater = new Updater(GetScene());
      }
      ~PhysicsApp(){}

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
   SetDataFilePathList("..;../../data/;../../../data/;" + GetDeltaDataPathList() );
            
   PhysicsApp *app = new PhysicsApp();
  app->Config();
  dtCore::SetNotifyLevel(DEBUG_INFO, WARN);
  app->Run();

   return 0;
}
