#include <stdlib.h>

#include "Producer/Timer"

#include "sg.h"

#include "camera.h"
#include "globals.h"
#include "keyboard.h"
#include "notify.h"

#include "rticonnection.h"
#include "system.h"
#include "dt.h"

using namespace dtCore;
using namespace dtHLA;

class Updater : public Base
{
   public:

      DECLARE_MANAGEMENT_LAYER(Updater)

      Updater(Keyboard* keyboard, EffectManager* effectManager, 
              Entity* entity, Camera* camera)
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

            if(value < delta*0.5f)
            {
               sgVec3 location;

               location[0] = 100*((float)rand() / RAND_MAX) - 50;
               location[1] = 100*((float)rand() / RAND_MAX) + 50;
               location[2] = 100*((float)rand() / RAND_MAX) - 50;

               mEffectManager->AddDetonation(
                  location,
                  (rand() < RAND_MAX/2) ? 
                     HighExplosiveDetonation : SmokeDetonation
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
      Entity* mEntity;
      Camera* mCamera;
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
   DeltaWin* win = new DeltaWin;
   Scene* scene = new Scene;

   Camera* cam = new Camera;
   cam->SetWindow( win );
   cam->SetScene( scene );
   Transform position;
   position.Set(0.f, -50.f, 0.f, 0.f, 0.f, 0.f);
   cam->SetTransform( &position );
   
   //This is where we'll find our files to load
   SetDataFilePathList("../../data/");

   Entity* entity = new Entity;

   entity->LoadFile("cessna.osg");

   entity->SetEntityIdentifier(
      EntityIdentifier(1, 1, 1)
   );

   EntityType helicopter(1, 2, 225, 22, 3, 11, 0);

   entity->SetEntityType(helicopter);

   scene->AddDrawable( entity );

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

   RTIConnection* rtic = new RTIConnection;

   rtic->SetScene(scene);

   rtic->SetEffectManager(effectManager);

   rtic->SetGeoOrigin(34.154, -116.197, 0.0);

   Keyboard* keyboard = win->GetKeyboard();

   Updater* updater = new Updater(keyboard, effectManager, entity, cam);

   System* system = System::GetSystem();

   system->Config();

   rtic->JoinFederationExecution();
   
   rtic->RegisterMasterEntity(entity);

   rtic->AddEntityTypeMapping(
      helicopter,
      "cessna.osg"
   );

   system->Run();

   rtic->LeaveFederationExecution();
   
   return 0;
}
