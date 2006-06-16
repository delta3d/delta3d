#include <dtChar/dtchar.h>
#include <dtCore/dt.h>
#include <dtABC/dtabc.h>
#include <iostream>

#include <dtCore/keyboard.h>   // for base class

using namespace dtCore;
using namespace dtABC;
using namespace dtChar;

class KeyController : public dtCore::Base
{
public:
   KeyController(Character* character, Keyboard* keyboard) : Base("KeyController"),
      mCharacter(character),
      mKeyboard(keyboard)
   {
      AddSender(System::Instance());
   }

   void OnMessage(dtCore::Base::MessageData* data)
   {
      if(data->message == "preframe")
      {
         const double delta = *static_cast<const double*>(data->userData);

         float rotation = mCharacter->GetRotation(),
            velocity = 0.0f;

         if(mKeyboard->GetKeyState(Producer::Key_Left))
         {
            rotation += float(delta*90.0);
         }

         if(mKeyboard->GetKeyState(Producer::Key_Right))
         {
            rotation -= float(delta*90.0);
         }

         if(mKeyboard->GetKeyState(Producer::Key_Up))
         {
            if(mKeyboard->GetKeyState(Producer::Key_Shift_R) ||
               mKeyboard->GetKeyState(Producer::Key_Shift_L))
            {
               velocity += 4.0f;
            }
            else
            {
               velocity += 1.5f;
            }
         }

         mCharacter->SetRotation(rotation);
         mCharacter->SetVelocity(velocity);
      }
   }

private:
   RefPtr<Character> mCharacter;
   RefPtr<Keyboard> mKeyboard;
};

class FollowController : public Base
{
public:
   FollowController(Character* character, Transformable* target) : Base("FollowController"),
      mCharacter(character),
      mTarget(target)
   {
      AddSender(System::Instance());
   }

   virtual void OnMessage(MessageData *data)
   {
      if(data->message == "preframe")
      {
         const double delta = *static_cast<const double*>(data->userData);

         Transform transform;

         osg::Vec3 ownPos;
         mCharacter->GetTransform(&transform);
         transform.GetTranslation(ownPos);

         osg::Vec3 targetPos;
         mTarget->GetTransform(&transform);
         transform.GetTranslation(targetPos);

         osg::Vec3 vector = targetPos - ownPos;

         vector[2] = 0.0f;

         float len = vector.length();
         float dir = osg::RadiansToDegrees(atan2(vector[0], -vector[1]));
         float rotation = mCharacter->GetRotation();
         float dR = dir - rotation;

         if( dR > 180.0f )
         {
            dR -= 360.0f;
         }
         else if( dR < -180.0f )
         {
            dR += 360.0f;
         }

         if(dR > 0.0f)
         {
            rotation += (dR > delta*90.0f ? delta*90.0f : dR);   
         }
         else if(dR < 0.0f)
         {
            rotation += (dR < -delta*90.0f ? -delta*90.0f : dR);
         }

         mCharacter->SetRotation(rotation);

         if(len > 5.0f)
         {
            mCharacter->SetVelocity(4.0f);
         }
         else if(len > 2.5f)
         {
            mCharacter->SetVelocity(1.0f);
         }
         else
         {
            mCharacter->SetVelocity(0.0f);
         }
      }
   }

private:
   RefPtr<Character> mCharacter;
   RefPtr<Transformable> mTarget;
};

class TestCharactersApp : public dtABC::Application
{
            
public:
   TestCharactersApp( const std::string& configFilename = "config.xml" )
      : Application( configFilename ) 
   {
   }

   void Config()
   {
      Application::Config();
      //adjust some lighting
      GetScene()->UseSceneLight(false);

      mInfiniteLight = new InfiniteLight(0);
      mInfiniteLight->SetAzimuthElevation( 90.0f, 45.0f );
      mInfiniteLight->SetDiffuse( 255.0f, 255.0f, 255.0f, 1.0f );
      mInfiniteLight->SetSpecular( 255.0f, 255.0f, 255.0f, 1.0f );
      mInfiniteLight->SetEnabled( true );

      AddDrawable( mInfiniteLight.get() );      

      //position the camera
      Transform position;
      position.SetTranslation( -0.75f, -10.f, 5.0f );
      GetCamera()->SetTransform( &position );

      osg::Vec3 camLoc;
      position.GetTranslation( camLoc );

      osg::Vec3 origin;

      //setup a motion model
      mMotionModel = new OrbitMotionModel( GetKeyboard(), GetMouse() );
      mMotionModel->SetTarget( GetCamera() );

      float distance = sqrt(  osg::square( camLoc[0] - origin[0] ) + 
                              osg::square( camLoc[1] - origin[1] ) + 
                              osg::square( camLoc[2] - origin[2] ) );

      mMotionModel->SetDistance( distance );
      
      //load up a terrain
      mTerrain = new Object( "Terrain" );
      mTerrain->LoadFile( "models/terrain_simple.ive" );
      AddDrawable( mTerrain.get() );

      //create some characters
      mOpFor = new Character( "Bob" );
      mMarine = new Character( "Dave" );
      
      position.SetTranslation( 0.0f, 0.0f, 3.5f );
      mOpFor->SetTransform( &position);

      position.SetTranslation( -2.0f, 0.0f, 3.5f );
      mMarine->SetTransform( &position );

      mOpFor->LoadFile( "opfor/opfor.rbody" );
      mMarine->LoadFile( "marine/marine.rbody" );

      AddDrawable( mOpFor.get() );
      AddDrawable( mMarine.get() );

      //move Bob with the keyboard
      mKeyController = new KeyController( mOpFor.get(), GetWindow()->GetKeyboard() );

      //have Dave follow Bob
      mFollowController = new FollowController( mMarine.get(), mOpFor.get() );
   }

   bool KeyPressed(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character)
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
         case Producer::Key_Escape:
         {
            Quit();
            verdict = true;
            break;
         }
         default:
            break;
      }
      return verdict;
   }


protected:

   RefPtr<Object> mTerrain;
   RefPtr<InfiniteLight> mInfiniteLight;
   RefPtr<Character> mOpFor;
   RefPtr<Character> mMarine;
   RefPtr<KeyController> mKeyController;
   RefPtr<FollowController> mFollowController;
   RefPtr<OrbitMotionModel> mMotionModel;
  
};

int main()
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testCharacters/;" +
                        GetDeltaDataPathList()  );
  
   RefPtr<TestCharactersApp> app = new TestCharactersApp( "config.xml" );

   app->Config();
   app->Run();

   return 0;
}
