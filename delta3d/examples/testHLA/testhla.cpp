#include "dtCore/dt.h"
#include "dtABC/dtabc.h"
#include "dtHLA/dthla.h"

using namespace dtCore;
using namespace dtHLA;
using namespace dtABC;


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
               ( rand() < RAND_MAX/2) ? HighExplosiveDetonation : SmokeDetonation );
         }

         mAngle = mAngle + 45.0f*delta;

         if(mAngle > 360) mAngle -= 360.0f;

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

IMPLEMENT_MANAGEMENT_LAYER( Updater )

class TestHLAApp : public Application
{

  DECLARE_MANAGEMENT_LAYER( TestHLAApp )

public:
   TestHLAApp( std::string configFile = "config.xml" )
      : Application( configFile )
   {
   }

   ~TestHLAApp()
   {
      mRtic->LeaveFederationExecution();
   }

   void Config()
   {
      Application::Config();

      Transform position;
      position.Set(0.f, -50.f, 0.f, 0.f, 0.f, 0.f);
      GetCamera()->SetTransform( &position );

      Entity* entity = new Entity;
      entity->LoadFile("UH-1N/UH-1N.ive");
      entity->SetEntityIdentifier(
         EntityIdentifier(1, 1, 1)
         );

      EntityType helicopter(1, 2, 225, 22, 3, 11, 0);
      entity->SetEntityType(helicopter);

      AddDrawable( entity );

      EffectManager* effectManager = new EffectManager;

      effectManager->AddDetonationTypeMapping(
         HighExplosiveDetonation,
         "explosion.osg"
         );

      effectManager->AddDetonationTypeMapping(
         SmokeDetonation,
         "smoke.osg"
         );

      AddDrawable( effectManager );

      mRtic = new RTIConnection;
      mRtic->SetScene( GetScene() );
      mRtic->SetEffectManager(effectManager);
      mRtic->SetGeoOrigin(34.154, -116.197, 0.0);

      Updater* updater = new Updater( GetKeyboard(), effectManager, entity, GetCamera() );

      mRtic->JoinFederationExecution();
      mRtic->RegisterMasterEntity(entity);
      mRtic->AddEntityTypeMapping(
         helicopter,
         "UH-1N/UH-1N.ive"
         );  
   }

   RTIConnection* mRtic;

};

IMPLEMENT_MANAGEMENT_LAYER( TestHLAApp )

int main( int argc, char **argv )
{
   SetDataFilePathList("..;" + GetDeltaDataPathList());

   TestHLAApp* app = new TestHLAApp( "config.xml" );

   app->Config();
   app->Run();

   delete app;

   return 0;
}
