#include "dtChar/dtchar.h"
#include "dtCore/dt.h"
#include "dtABC/dtabc.h"
#include "gui_fl/guimgr.h"

using namespace dtCore;
using namespace dtABC;
using namespace dtChar;

class KeyController : public Base, public KeyboardListener
{
   DECLARE_MANAGEMENT_LAYER(KeyController)

public:

   KeyController(Character* character, Keyboard* keyboard)
      : Base("KeyController"),
      mCharacter(character),
      mKeyboard(keyboard)
   {
      AddSender(System::GetSystem());

      mKeyboard->AddKeyboardListener(this);
   }


   virtual void OnMessage(MessageData *data)
   {
      if(data->message == "preframe")
      {
         double delta = *(double*)data->userData;

         float rotation = mCharacter->GetRotation(),
            velocity = 0.0f;

         if(mKeyboard->GetKeyState(Producer::Key_Left))
         {
            rotation += (float)(delta*90.0);
         }

         if(mKeyboard->GetKeyState(Producer::Key_Right))
         {
            rotation -= (float)(delta*90.0);
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
               velocity += 1.0f;
            }
         }

         mCharacter->SetRotation(rotation);
         mCharacter->SetVelocity(velocity);
      }
   }


   virtual void KeyPressed(Keyboard* keyboard, 
      Producer::KeyboardKey key,
      Producer::KeyCharacter character)
   {
      switch(key)
      {
      case Producer::Key_Escape:
         System::GetSystem()->Stop();
         break;
      default:
         break;
      }
   }


   virtual void KeyReleased(Keyboard* keyboard, 
      Producer::KeyboardKey key,
      Producer::KeyCharacter character)
   {}


private:

   RefPtr<Character> mCharacter;
   RefPtr<Keyboard> mKeyboard;
        
};

IMPLEMENT_MANAGEMENT_LAYER(KeyController)


class FollowController : public Base
{
   DECLARE_MANAGEMENT_LAYER(FollowController)

public:


   FollowController(Character* character, Transformable* target)
      : Base("FollowController"),
      mCharacter(character),
      mTarget(target)
   {
      AddSender(System::GetSystem());
   }


   virtual void OnMessage(MessageData *data)
   {
      if(data->message == "preframe")
      {
         double delta = *(double*)data->userData;

         sgVec3 ownPos, targetPos, vector;

         Transform transform;

         mCharacter->GetTransform(&transform);

         transform.GetTranslation(ownPos);

         mTarget->GetTransform(&transform);

         transform.GetTranslation(targetPos);

         sgSubVec3(vector, targetPos, ownPos);

         vector[2] = 0.0f;

         float len = sgLengthVec3(vector),
            dir = sgATan2(vector[0], -vector[1]),
            rotation = mCharacter->GetRotation(),
            dR = dir - rotation;

         if(dR > 180.0f) dR -= 360.0f;
         else if(dR < -180.0f) dR += 360.0f;

         if(dR > 0.0f)
         {
            rotation += (dR > delta*90.0f ? delta*90.0f : dR);   
         }
         else if(dR < 0.0f)
         {
            rotation += (dR < -delta*90.0f ? -delta*90.0f : dR);
         }

         mCharacter->SetRotation(rotation);

         if(dR < -90.0f)
         {
            mCharacter->ExecuteActionWithAngle("ACT_LOOK", -90.0);
         }
         else if(dR > 90.0f)
         {
            mCharacter->ExecuteActionWithAngle("ACT_LOOK", 90.0);
         }
         else
         {
            mCharacter->ExecuteActionWithAngle("ACT_LOOK", dR);
         }

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

IMPLEMENT_MANAGEMENT_LAYER(FollowController)


class TestCharactersApp : public dtABC::Application
{

DECLARE_MANAGEMENT_LAYER(TestCharactersApp)
            
public:
   TestCharactersApp( std::string configFilename = "config.xml" )
      : Application( configFilename ) 
   {
   }

   void Config()
   {
      Application::Config();

      //adjust some lighting
      GetScene()->UseSceneLight(false);

      iLight1 = new InfiniteLight(1);
      iLight1->SetDirection( 0, -45,0 );
      iLight1->SetAmbient( 1, 1, 1, 0 );
      iLight1->SetDiffuse( 200, 200, 200, 1 );
      iLight1->SetSpecular( 255, 255, 255, 0 );
      iLight1->SetEnabled( true );

      iLight2 = new InfiniteLight(2);
      iLight2->SetDirection( 160, -20, 0 );
      iLight2->SetDiffuse( 205, 190, 112, 1 );
      iLight2->SetSpecular( 255, 255, 255, 0 );
      iLight2->SetEnabled( true );

      AddDrawable( iLight1.get() );
      AddDrawable( iLight2.get() );

      //position the camera
      Transform position;
      position.Set( -0.75f, -10.f, 0.5f, 0.f, 0.f, 0.f);
      GetCamera()->SetTransform( &position );

      osg::Vec3 camLoc;
      position.GetTranslation( camLoc );

      osg::Vec3 origin = osg::Vec3(0.0f, 0.0f, 0.0f);

      //setup a motion model
      omm = new OrbitMotionModel( GetKeyboard(), GetMouse() );
      omm->SetTarget( GetCamera() );

      float distnace = sqrt(  osg::square( camLoc[0]-origin[0] ) + 
                              osg::square( camLoc[1]-origin[1] ) + 
                              osg::square( camLoc[2]-origin[2] ) );

      omm->SetDistance( distnace );
      
      //load up a terrain
      terrain = new Object( "Terrain" );
      terrain->LoadFile( "models/dirt.ive" );
      AddDrawable( terrain.get() );

      //create some characters
      guy1 = new Character( "bob" );
      guy2 = new Character( "dave" );
      
      position.Set(0, 0, 0, 0, 0, 0);
      guy1->SetTransform(&position);

      position.Set(-2, 0, 0, 0, 0, 0);
      guy2->SetTransform(&position);

      guy1->LoadFile( "opfor/opfor.rbody" );
      guy2->LoadFile( "marine/marine.rbody" );

      AddDrawable( guy1.get() );
      AddDrawable( guy2.get() );

      //move bob with the keyboard
      kc = new KeyController( guy1.get(), GetWindow()->GetKeyboard() );

      //have dave follow bob
      fc = new FollowController( guy2.get(), guy1.get() );

   }

protected:

   RefPtr<Object> terrain;
   RefPtr<InfiniteLight> iLight1;
   RefPtr<InfiniteLight> iLight2;
   RefPtr<Character> guy1;
   RefPtr<Character> guy2;
   RefPtr<KeyController> kc;
   RefPtr<FollowController> fc;
   RefPtr<OrbitMotionModel> omm;
  
};

IMPLEMENT_MANAGEMENT_LAYER(TestCharactersApp)


        

int main()
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );
  
   RefPtr<TestCharactersApp> app = new TestCharactersApp( "config.xml" );

   app->Config();
   app->Run();

   return 0;
}
