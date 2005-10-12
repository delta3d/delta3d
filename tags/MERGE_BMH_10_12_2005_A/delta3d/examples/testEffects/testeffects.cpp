#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

#include "Producer/Timer"
#include <math.h>

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
      AddSender(System::Instance());

      mLastTime = mTimer.tick();

      srand( (unsigned int)mLastTime );
   }

   virtual void OnMessage(MessageData* data)
   {
      if(data->message == "preframe")
      {
         if(mKeyboard->GetKeyState(Producer::Key_Escape))
         {
            System::Instance()->Stop();
         }

         Producer::Timer_t currentTime = mTimer.tick();

         float delta = (float)mTimer.delta_s(mLastTime, currentTime);

         mLastTime = currentTime;

         float value = (float)rand() / RAND_MAX;

         if(value < delta*0.25f)
         {
            osg::Vec3 location;

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
            40*cosf(osg::DegreesToRadians(mAngle)), 
            100 + 40*sinf(osg::DegreesToRadians(mAngle)),
            0,
            mAngle,
            0,
            -45.0,
            1.0f, 1.0f, 1.0f
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
            mCameraHeading, mCameraPitch, 0.0f,
            1.0f, 1.0f, 1.0f
            );

         mCamera->SetTransform(&mPosition);
      }
   }


private:

   RefPtr<Keyboard> mKeyboard;
   RefPtr<EffectManager> mEffectManager;
   RefPtr<Object> mEntity;
   RefPtr<Camera> mCamera;
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
      position.Set(0.f, -50.f, 0.f, 0.f, 0.f, 0.f, 1.0f, 1.0f, 1.0f);
      GetCamera()->SetTransform( &position );

      entity = new Object("UH-1N");
      entity->LoadFile("models/uh-1n.ive");
      AddDrawable( entity.get() );

      smoke = new ParticleSystem;
      smoke->LoadFile("effects/smoke.osg");

      entity->AddChild(smoke.get());


      effectManager = new EffectManager;

      effectManager->AddDetonationTypeMapping(
         HighExplosiveDetonation,
         "effects/explosion.osg"
         );

      effectManager->AddDetonationTypeMapping(
         SmokeDetonation,
         "effects/smoke.osg"
         );

      AddDrawable( effectManager.get() );

      updater = new Updater(GetKeyboard(), effectManager.get(), entity.get(), GetCamera());

   }

   RefPtr<Object> entity;
   RefPtr<ParticleSystem> smoke;
   RefPtr<EffectManager> effectManager;
   RefPtr<Updater> updater;

};

IMPLEMENT_MANAGEMENT_LAYER(TestEffectsApp)

int main( int argc, char **argv )
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testEffects/;" +
                        GetDeltaDataPathList()+ ";" + GetDeltaDataPathList()+"/effects/"  );

   RefPtr<TestEffectsApp> app = new TestEffectsApp( "config.xml" );
   
   app->Config();
   app->Run();

   return 0;
}
