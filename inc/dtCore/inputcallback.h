/*
 *  inputcallback.h
 *  Delta3dEditor
 *
 *  Created by David Guthrie on 10/9/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef DELTA_INPUTCALLBACK

#include <dtCore/refptr.h>
#include <Producer/KeyboardMouse>   // for InputCallback's base class

namespace dtCore
{

   /// A class to support operating system callbacks for the keyboard and mouse.
   class InputCallback : public Producer::KeyboardMouseCallback
   {
   public:
      InputCallback(Keyboard* keyboard, Mouse* mouse);

      // virtual methods
      void mouseScroll(Producer::KeyboardMouseCallback::ScrollingMotion sm);
      void mouseMotion(float x, float y);
      void passiveMouseMotion(float x, float y);
      void buttonPress(float x, float y, unsigned int button);
      void doubleButtonPress(float x, float y, unsigned int button);
      void buttonRelease(float x, float y, unsigned int button);
      void keyPress(Producer::KeyCharacter kc);
      void keyRelease(Producer::KeyCharacter kc);
      void specialKeyPress(Producer::KeyCharacter kc);
      void specialKeyRelease(Producer::KeyCharacter kc);

      void SetKeyboard(Keyboard* kb);
      void SetMouse(Mouse* m);

   private:
      InputCallback();  ///< not implemented by design
      InputCallback& operator=( const InputCallback& );
      InputCallback(const InputCallback&);  ///< not implemented by design

      RefPtr<Keyboard> mKeyboard;
      RefPtr<Mouse> mMouse;
   };
}
#endif
