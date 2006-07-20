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
         virtual bool HandleKeyPressed( const dtCore::Keyboard* keyboard, 
                                        Producer::KeyboardKey key,
                                        Producer::KeyCharacter character );

         virtual bool HandleKeyReleased( const dtCore::Keyboard* keyboard, 
                                         Producer::KeyboardKey key,
                                         Producer::KeyCharacter character );

         virtual bool HandleKeyTyped( const dtCore::Keyboard* keyboard, 
                                      Producer::KeyboardKey key,
                                      Producer::KeyCharacter character );
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
