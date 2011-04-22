#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

#include "Producer/Timer"

using namespace dtCore;
using namespace dtABC;

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

class TestEffectsApp : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER(TestEffectsApp)

public:
   TestEffectsApp( std::string configFilename = "config.xml" )
      : Application( configFilename )
   {
   }

   void Config()
   {
      Application::Config();

      Transform position;
      position.Set(0.f, -50.f, 0.f, 0.f, 0.f, 0.f);
      GetCamera()->SetTransform( &position );

      Object* entity = new Object("UH-1N");
      entity->LoadFile("UH-1N/UH-1N.ive");
      AddDrawable( entity );

      ParticleSystem* smoke = new ParticleSystem;
      smoke->LoadFile("smoke.osg");
      AddDrawable(smoke);

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

      AddDrawable( effectManager );

      Updater* updater = new Updater(GetKeyboard(), effectManager, entity, GetCamera());

   }
};

IMPLEMENT_MANAGEMENT_LAYER(TestEffectsApp)

int main( int argc, char **argv )
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );

   TestEffectsApp* app = new TestEffectsApp( "config.xml" );
   
   app->Config();
   app->Run();

   delete app;

   return 0;
}