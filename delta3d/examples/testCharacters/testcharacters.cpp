#include "dt.h"
#include "dtchar.h"
#include "dtabc.h"

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
      }
   }


   virtual void KeyReleased(Keyboard* keyboard, 
      Producer::KeyboardKey key,
      Producer::KeyCharacter character)
   {}


private:

   Character* mCharacter;
   Keyboard* mKeyboard;
        
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

   Character* mCharacter;
   Transformable* mTarget;
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

      Transform position;
      position.Set(0.f, -10.f, 1.0f, 0.f, 0.f, 0.f);
      GetCamera()->SetTransform( &position );
      
      Object* terrain = new Object( "Terrain" );
      terrain->LoadFile( "dirt.ive" );
      AddDrawable( terrain );

      Weather* weather = new Weather();   
      AddDrawable( weather->GetEnvironment() );

      Character* guy1 = new Character( "bob" );
      Character* guy2 = new Character( "dave" );
      
      position.Set(0, 0, 0, 0, 0, 0);
      guy1->SetTransform(&position);

      position.Set(-2, 0, 0, 0, 0, 0);
      guy2->SetTransform(&position);

      guy1->LoadFile( "opfor/opfor.rbody" );
      guy2->LoadFile( "marine/marine.rbody" );

      AddDrawable( guy1 );
      AddDrawable( guy2 );

      KeyController* kc = new KeyController(guy1, GetWindow()->GetKeyboard());
      FollowController* fc = new FollowController(guy2, guy1);

      Transform trans;
      GetCamera()->GetTransform(&trans);

      sgVec3 camLoc;
      trans.GetTranslation( camLoc );

      sgVec3 origin = {0.0f, 0.0f, 0.0f};

      OrbitMotionModel* omm = new OrbitMotionModel(GetKeyboard(),GetMouse());
      omm->SetTarget(GetCamera());
      omm->SetDistance( sgDistanceVec3( camLoc, origin ) );
      
      
   }
  
};

IMPLEMENT_MANAGEMENT_LAYER(TestCharactersApp)


        

int main()
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );
  
   TestCharactersApp* app = new TestCharactersApp( "config.xml" );

   app->Config();
   app->Run();


   delete app;

   return 0;
}
