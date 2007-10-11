#include <dtABC/application.h>
#include <dtCore/effectmanager.h>
#include <dtCore/globals.h>
#include <dtCore/keyboard.h>
#include <dtCore/object.h>
#include <dtCore/particlesystem.h>
#include <cmath>
#include <dtCore/camera.h>
#include <dtCore/system.h>

using namespace dtCore;
using namespace dtABC;

const std::string kHighExplosiveDetonation = "HighExplosiveDetonation";
const std::string kSmokeDetonation = "SmokeDetonation";

class Updater : public Base
{
public:

   Updater( Keyboard* keyboard, EffectManager* effectManager, 
            Object* entity, dtCore::Camera* camera)
    : Base("Updater"),
      mKeyboard(keyboard),
      mEffectManager(effectManager),
      mEntity(entity),
      mCamera(camera),
      mAngle(0.0f),
      mCameraHeading(0.0f),
      mCameraPitch(0.0f)
   {
      AddSender( &dtCore::System::GetInstance() );
   }

   virtual void OnMessage( MessageData* data )
   {
      if( data->message == "preframe" )
      {
         if( mKeyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Escape) )
         {
            System::GetInstance().Stop();
         }

         const double delta = *static_cast<const double*>( data->userData );

         float value = float(rand()) / RAND_MAX;

         if( value < delta*0.25f )
         {
            osg::Vec3 location;

            location[0] = 100*( float(rand()) / RAND_MAX ) - 50;
            location[1] = 100*( float(rand()) / RAND_MAX ) + 50;
            location[2] = 100*( float(rand()) / RAND_MAX ) - 50;

            mEffectManager->AddDetonation( location, kHighExplosiveDetonation );
         }

         mAngle = mAngle + 45.0f*delta;

         if( mAngle > 360 )
         {
            mAngle -= 360.0f;
         }

         mPosition.SetTranslation(  40*cosf( osg::DegreesToRadians(mAngle) ), 
                                    100 + 40*sinf( osg::DegreesToRadians(mAngle) ),
                                    0 );
         mPosition.SetRotation( mAngle, 0, -45.0 );

         mEntity->SetTransform(mPosition);

         if( mKeyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Up) )
         {
            mCameraPitch += delta*45.0;
         }
         if( mKeyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Down) )
         {
            mCameraPitch -= delta*45.0;
         }
         if( mKeyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Left) )
         {
            mCameraHeading += delta*45.0;
         }
         if( mKeyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Right) )
         {
            mCameraHeading -= delta*45.0;
         }

         mPosition.SetTranslation( 0.0f, -50.0f, 0.0f );
         mPosition.SetRotation( mCameraHeading, mCameraPitch, 0.0f );

         mCamera->SetTransform(mPosition);
      }
   }


private:

   RefPtr<Keyboard> mKeyboard;
   RefPtr<EffectManager> mEffectManager;
   RefPtr<Object> mEntity;
   RefPtr<Camera> mCamera;
   Transform mPosition;
   float mAngle;
   float mCameraHeading;
   float mCameraPitch;
};

class TestEffectsApp : public dtABC::Application
{

public:
   TestEffectsApp( const std::string& configFilename = "config.xml" )
      : Application( configFilename )
   {
   }

   virtual void Config()
   {
      Application::Config();

      Transform position(0.0f, -50.0f, 0.0f);
      GetCamera()->SetTransform( position );

      entity = new Object("UH-1N");
      entity->LoadFile("models/uh-1n.ive");
      AddDrawable( entity.get() );

      smoke = new ParticleSystem;
      smoke->LoadFile("effects/smoke.osg");

      entity->AddChild(smoke.get());

      effectManager = new EffectManager;
      effectManager->AddDetonationTypeMapping(  kHighExplosiveDetonation,
                                                "effects/explosion.osg" );
      effectManager->AddDetonationTypeMapping(  kSmokeDetonation,
                                                "effects/smoke.osg" );

      AddDrawable( effectManager.get() );

      updater = new Updater(  GetKeyboard(), 
                              effectManager.get(), 
                              entity.get(), 
                              GetCamera());

   }

   RefPtr<Object> entity;
   RefPtr<ParticleSystem> smoke;
   RefPtr<EffectManager> effectManager;
   RefPtr<Updater> updater;

};

int main()
{
   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" + 
      dtCore::GetDeltaRootPath() + "/examples/data" + ";");

   RefPtr<TestEffectsApp> app = new TestEffectsApp( "config.xml" );
   
   app->Config();
   app->Run();

   return 0;
}
