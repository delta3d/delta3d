#include <dtABC/application.h>
#include <dtCore/camera.h>
#include <dtCore/globals.h>
#include <dtCore/keyboard.h>
#include <dtCore/system.h>
#include <dtHLA/rticonnection.h>

#include <cmath>

using namespace dtCore;
using namespace dtHLA;
using namespace dtABC;

class Updater : public Base
{
public:

   Updater( Keyboard* keyboard, EffectManager* effectManager, 
            Entity* entity, Camera* camera ) : 
      Base("updater"),
      mKeyboard(keyboard),
      mEffectManager(effectManager),
      mEntity(entity),
      mCamera(camera),
      mAngle(0.0f),
      mCameraHeading(0.0f),
      mCameraPitch(0.0f)
   {
      AddSender( System::Instance() );
      srand( unsigned( time(0) ) );
   }

protected:

   virtual ~Updater() {}

public:

   float Random() { return float(rand()) / RAND_MAX; }

   virtual void OnMessage(MessageData* data)
   {
      if(data->message == "frame")
      {
         if(mKeyboard->GetKeyState(Producer::Key_Escape))
         {
            System::Instance()->Stop();
         }

         const float delta = float(*static_cast<const double*>(data->userData));

         if( Random() < delta*0.5f )
         {
            osg::Vec3 location(  100.0f*Random() - 50.0f,
                                 100.0f*Random() + 50.0f,
                                 100.0f*Random() - 50.0f );

            mEffectManager->AddDetonation(
               location,
               ( rand() < RAND_MAX/2) ? "HighExplosiveDetonation" : "SmokeDetonation" );
         }

         mAngle = mAngle + 45.0f*delta;

         if(mAngle > 360.0f)
         {
            mAngle -= 360.0f;
         }

         mPosition.SetTranslation(  40.0f*cosf(osg::DegreesToRadians(mAngle)), 
                                    100.0f + 40.0f*sinf(osg::DegreesToRadians(mAngle)),
                                    0.0f );
         mPosition.SetRotation( mAngle, 0.0f, -45.0f );

         mEntity->SetTransform(&mPosition);

         if(mKeyboard->GetKeyState(Producer::Key_Up))
         {
            mCameraPitch += delta*45.0f;
         }
         if(mKeyboard->GetKeyState(Producer::Key_Down))
         {
            mCameraPitch -= delta*45.0f;
         }
         if(mKeyboard->GetKeyState(Producer::Key_Left))
         {
            mCameraHeading += delta*45.0f;
         }
         if(mKeyboard->GetKeyState(Producer::Key_Right))
         {
            mCameraHeading -= delta*45.0f;
         }

         mPosition.SetTranslation( 0.0f, -50.0f, 0.0f );
         mPosition.SetRotation( mCameraHeading, mCameraPitch, 0.0f );

         mCamera->SetTransform(&mPosition);
      }
   }

private:

   RefPtr<Keyboard> mKeyboard;
   RefPtr<EffectManager> mEffectManager;
   RefPtr<Entity> mEntity;
   RefPtr<Camera> mCamera;
   Transform mPosition;
   float mAngle;
   float mCameraHeading;
   float mCameraPitch;
};

class TestHLAApp : public Application
{

public:
   TestHLAApp( const std::string& configFile = "config.xml" )
      :  Application( configFile ),
         mEntity( new Entity() ),
         mEffectManager( new EffectManager() ),
         mRtic( new RTIConnection() ),
         mUpdater(0)
   {
   }

protected:

   virtual ~TestHLAApp()
   {
      mRtic->LeaveFederationExecution();
   }

public:

   void Config()
   {
      Application::Config();

      Transform position;
      position.SetTranslation(0.0f, -50.0f, 0.0f);
      GetCamera()->SetTransform( &position );

      mEntity->LoadFile("models/uh-1n.ive");
      mEntity->SetEntityIdentifier( EntityIdentifier(1, 1, 1) );

      EntityType helicopter(1, 2, 225, 22, 3, 11, 0);
      mEntity->SetEntityType(helicopter);

      AddDrawable( mEntity.get() );

      mEffectManager->AddDetonationTypeMapping(
         "HighExplosiveDetonation",
         "effects/explosion.osg"
         );

      mEffectManager->AddDetonationTypeMapping(
         "SmokeDetonation",
         "effects/smoke.osg"
         );

      AddDrawable( mEffectManager.get() );

      mRtic->SetScene( GetScene() );
      mRtic->SetEffectManager( mEffectManager.get() );
      mRtic->SetGeoOrigin(34.154, -116.197, 0.0);

      mUpdater = new Updater( GetKeyboard(), mEffectManager.get(), mEntity.get(), GetCamera() );

      mRtic->JoinFederationExecution();
      mRtic->RegisterMasterEntity( mEntity.get() );
      mRtic->AddEntityTypeMapping( helicopter, "models/uh-1n.ive" );  
   }

   RefPtr<Entity>          mEntity;
   RefPtr<EffectManager>   mEffectManager;
   RefPtr<RTIConnection>   mRtic;
   RefPtr<Updater>         mUpdater;
};

int main( int argc, char **argv )
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testHLA/;" +
                        GetDeltaDataPathList() + ";" + GetDeltaDataPathList()+"/effects/"  );

   RefPtr<TestHLAApp> app = new TestHLAApp( "config.xml" );

   app->Config();
   app->Run();

   return 0;
}
