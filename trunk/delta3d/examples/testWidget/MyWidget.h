/* -*-c++-*-
* testWidget - MyWidget (.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#ifndef DELTA_MYWIDGET
#define DELTA_MYWIDGET

#include <dtCore/keyboard.h>
#include <dtABC/widget.h>
#include <dtUtil/functor.h>

namespace dtCore
{
   class OrbitMotionModel;
   class Object;
}
class MyWidget : public dtABC::Widget
{
      enum  MyEvents
      {
         FILELOADED  = BIT(0),
      };

      typedef dtUtil::Functor<void,TYPELIST_0()> QuitFunctor;

      class EscapeListener : public dtCore::KeyboardListener
      {
         public:         
         EscapeListener( const MyWidget::QuitFunctor& quitFunctor );
         
         protected:
         virtual ~EscapeListener();
         
         public:
         virtual bool HandleKeyPressed( const dtCore::Keyboard* keyboard, int key );

         virtual bool HandleKeyReleased( const dtCore::Keyboard* keyboard, int key );

         virtual bool HandleKeyTyped( const dtCore::Keyboard* keyboard, int key );
         private:

         QuitFunctor mQuitFunctor;
      };

public:
                  MyWidget( const std::string& name = "MyWidget" );
protected:
   virtual        ~MyWidget();

   virtual  void  Config( const dtABC::WinData* data = NULL );
   virtual  void  OnMessage( dtCore::Base::MessageData* data );

private:
   inline   void  ObjectLoaded( dtCore::Object* obj );
   inline   void  InitInputDevices();

private:
            dtCore::RefPtr<dtCore::OrbitMotionModel> mMotionModel;
};

#endif // DELTA_MYWIDGET
