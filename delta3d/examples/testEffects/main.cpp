#include "dt.h"

#include "Producer/Timer"

using namespace dtCore;

class Updater : public Base
{
   public:

      DECLARE_MANAGEMENT_LAYER(Updater)

      Updater(Keyboard* keyboard, EffectManager* effectManager, 
            Object* entity, dtCore::Camera* camera)
         : Base("updater"),
           mKeyboard(keyboard),
           mEffectManager(effectManager),
           mEntity(entity),
           mCamera(camera),
           mAngle(0.0f),
           mCameraHeading(0.0f),
           mCameraPitch(0.0f)
      {
         AddSender(System::GetSystem());

         mLastTime = mTimer.tick();

         srand( (unsigned int)mLastTime );
      }

      virtual void OnMessage(MessageData* data)
      {
         if(data->message == "frame")
         {
            if(mKeyboard->GetKeyState(Producer::Key_Escape))
            {
               System::GetSystem()->Stop();
            }

            Producer::Timer_t currentTime = mTimer.tick();

            float delta = (float)mTimer.delta_s(mLastTime, currentTime);

            mLastTime = currentTime;

            float value = (float)rand() / RAND_MAX;

            if(value < delta*0.25f)
            {
               sgVec3 location;

               location[0] = 100*((float)rand() / RAND_MAX) - 50;
               location[1] = 100*((float)rand() / RAND_MAX) + 50;
               location[2] = 100*((float)rand() / RAND_MAX) - 50;

               mEffectManager->AddDetonation(
                  location,
                  HighExplosiveDetonation
               );
            }

            mAngle = mAngle - 45.0f*delta;

            if(mAngle < 0) mAngle += 360.0f;

            mPosition.Set(
               40*sgCos(mAngle), 
               100 + 40*sgSin(mAngle),
               0,
               mAngle,
               0,
               -45.0
            );

            mEntity->SetTransform(&mPosition);

            if(mKeyboard->GetKeyState(Producer::Key_Up))
            {
               mCameraPitch += delta*45.0;
            }
            if(mKeyboard->GetKeyState(Producer::Key_Down))
            {
               mCameraPitch -= delta*45.0;
            }
            if(mKeyboard->GetKeyState(Producer::Key_Left))
            {
               mCameraHeading += delta*45.0;
            }
            if(mKeyboard->GetKeyState(Producer::Key_Right))
            {
               mCameraHeading -= delta*45.0;
            }

            mPosition.Set(
               0.0f, -50.0f, 0.0f,
               mCameraHeading, mCameraPitch, 0.0f
            );

            mCamera->SetTransform(&mPosition);
         }
      }


   private:

      Keyboard* mKeyboard;
      EffectManager* mEffectManager;
      Object* mEntity;
      dtCore::Camera* mCamera;
      Producer::Timer mTimer;
      Producer::Timer_t mLastTime;
      Transform mPosition;
      float mAngle;
      float mCameraHeading;
      float mCameraPitch;
};

IMPLEMENT_MANAGEMENT_LAYER(Updater)

int main( int argc, char **argv )
{
   dtCore::Window* win = new dtCore::Window;
   Scene* scene = new Scene;

   dtCore::Camera* cam = new dtCore::Camera;
   cam->SetWindow( win );
   cam->SetScene( scene );
   Transform position;
   position.Set(0.f, -50.f, 0.f, 0.f, 0.f, 0.f);
   cam->SetTransform( &position );
   
   //This is where we'll find our files to load
   SetDataFilePathList("../../data/");

   Object* entity = new Object("My Entity");

   entity->LoadFile("cessna.osg");

   scene->AddDrawable( entity );

   ParticleSystem* smoke = new ParticleSystem;
   
   smoke->LoadFile("smoke.osg");

   scene->AddDrawable(smoke);
   
   entity->AddChild(smoke);

   EffectManager* effectManager = new EffectManager;

   effectManager->AddDetonationTypeMapping(
      HighExplosiveDetonation,
      "explosion.osg"
   );

   effectManager->AddDetonationTypeMapping(
      SmokeDetonation,
      "smoke.osg"
   );

   scene->AddDrawable( effectManager );

   Keyboard* keyboard = win->GetKeyboard();

   Updater* updater = 
      new Updater(keyboard, effectManager, entity, cam);

   System* system = System::GetSystem();

   system->Config();
   system->Run();

   return 0;
}
