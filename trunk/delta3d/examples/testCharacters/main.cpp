#include "dt.h"
#include "dtchar.h"

/*
#include "camera.h"
#include "globals.h"
#include "notify.h"
#include "object.h"
#include "system.h"
#include "character.h"
#include "keyboard.h"
#include "mouse.h"
*/

using namespace dtCore;
using namespace dtChar;

/**
 * Controls a character in response to keyboard commands.
 */
class KeyController : public Base, public KeyboardListener
{
   DECLARE_MANAGEMENT_LAYER(KeyController)
   
   public:
   
      /**
       * Constructor.
       *
       * @param character the character to control
       * @param keyboard the keyboard object
       */
      KeyController(Character* character, Keyboard* keyboard)
         : Base("KeyController"),
           mCharacter(character),
           mKeyboard(keyboard)
      {
         AddSender(System::GetSystem());
         
         mKeyboard->AddKeyboardListener(this);
      }
      
      /**
       * Message callback.
       *
       * @param data the message data
       */
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
      
      /**
       * Called when a key is pressed.
       *
       * @param keyboard the source of the event
       * @param key the key pressed
       * @param character the corresponding character
       */
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

      /**
       * Called when a key is released.
       *
       * @param keyboard the source of the event
       * @param key the key released
       */
      virtual void KeyReleased(Keyboard* keyboard, 
                                 Producer::KeyboardKey key,
                                 Producer::KeyCharacter character)
      {}
      
      
   private:
   
      /**
       * The character to control.
       */
      Character* mCharacter;
      
      /**
       * The keyboard object.
       */
      Keyboard* mKeyboard;
};

IMPLEMENT_MANAGEMENT_LAYER(KeyController)

/**
 * Makes a character follow a target.
 */
class FollowController : public Base
{
   DECLARE_MANAGEMENT_LAYER(FollowController)
   
   public:
   
      /**
       * Constructor.
       *
       * @param character the character to control
       * @param target the target to follow
       */
      FollowController(Character* character, Transformable* target)
         : Base("FollowController"),
           mCharacter(character),
           mTarget(target)
      {
         AddSender(System::GetSystem());
      }
      
      /**
       * Message callback.
       *
       * @param data the message data
       */
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
   
      /**
       * The controlled character.
       */
      Character* mCharacter;
      
      /**
       * The target to follow.
       */
      Transformable* mTarget;
};

IMPLEMENT_MANAGEMENT_LAYER(FollowController)

class OrbitMotionModel : public MouseListener
{
   public:

      OrbitMotionModel(Mouse* mouse, Transformable* target)
      {
         mMouse = mouse;
         mTarget = target;

         mMouse->AddMouseListener(this);

         mAzimuth = 0.0f;
         mElevation = 0.0f;
         mDistance = 10.0f;

         sgSetVec3(mCenter, 0, 0, 1);
         
         UpdateTargetTransform();
      }

      virtual void ButtonPressed(Mouse* mouse, MouseButton button)
      {
         mouse->GetPosition(&mLastX, &mLastY);
      }

      virtual void MouseDragged(Mouse* mouse, float x, float y)
      {
         float dX = x - mLastX, dY = y - mLastY;

         if(mouse->GetButtonState(LeftButton))
         {
            mAzimuth -= dX*90;
            mElevation -= dY*90;

            if(mElevation < -90.0f) mElevation = -90.0f;
            else if(mElevation > 90.0f) mElevation = 90.0f;
         }
         
         if(mouse->GetButtonState(MiddleButton))
         {
            mDistance -= dY*20.0f;
            
            if(mDistance < 1.0f) mDistance = 1.0f;
         }
         
         if(mouse->GetButtonState(RightButton))
         {
            Transform transform;
            
            mTarget->GetTransform(&transform);
            
            sgMat4 mat;
            
            transform.Get(mat);
            
            sgVec3 offset = {-dX*mDistance*0.25f, 0, -dY*mDistance*0.25f};
            
            sgXformVec3(offset, mat);
            
            sgAddVec3(mCenter, offset);
         }
         
         UpdateTargetTransform();
         
         mLastX = x;
         mLastY = y;
      }

      virtual void MouseScrolled(Mouse* mouse, int delta)
      {
         mDistance -= delta*5;

         if(mDistance < 1.0f) mDistance = 1.0f;

         UpdateTargetTransform();
      }

   private:

      void UpdateTargetTransform()
      {
         Transform transform;
         
         transform.SetTranslation(
            mCenter[0] + mDistance * sgCos(mElevation) * sgSin(mAzimuth),
            mCenter[1] + mDistance * sgCos(mElevation) * -sgCos(mAzimuth),
            mCenter[2] + mDistance * sgSin(mElevation)
         );
         
         transform.SetRotation(
            mAzimuth, 
            -mElevation, 
            0.0f
         );
         
         mTarget->SetTransform(&transform);
      }

      Mouse* mMouse;

      Transformable* mTarget;

      float mAzimuth, mElevation, mDistance;

      sgVec3 mCenter;
      
      float mLastX, mLastY;
};

int main( int argc, char **argv )
{
   Window* win = new Window;
   Scene* scene = new Scene;
   System *sys = System::GetSystem();

   Camera* cam = new Camera;
   cam->SetWindow( win );
   cam->SetScene( scene );
   Transform position;
   position.Set(0.f, -10.f, 1.f, 0.f, 0.f, 0.f);
   cam->SetTransform( &position );
   
   //This is where we'll find our files to load
   SetDataFilePathList("../../data/");

   Character* guy1 = new Character("bob");
   Character* guy2 = new Character("dave");
   
   position.Set(0, 0, 1, 0, 0, 0);
   guy1->SetTransform(&position);
   
   position.Set(-2, 0, 0, 0, 0, 0);
   guy2->SetTransform(&position);
   
   if (!guy1->LoadFile("bizguy1/bizguy1.rbody"))    return(-1);

   if (!guy2->LoadFile("bizguy1/bizguy1.rbody"))    return(-1);
   
   Object* obj = new Object("ground");
   
   obj->LoadFile("ground.flt");
   
   //Add the Objects to the Scene to be rendered
   scene->AddDrawable( obj );
   scene->AddDrawable( guy1 );
   scene->AddDrawable( guy2 );
   
   KeyController* kc = new KeyController(guy1, win->GetKeyboard());
   FollowController* fc = new FollowController(guy2, guy1);
   
   ::OrbitMotionModel omm(win->GetMouse(), cam);
   
   sys->Config();
   sys->Run();

   return 0;
}