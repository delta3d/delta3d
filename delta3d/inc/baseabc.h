#ifndef DELTA_BASEABC
#define DELTA_BASEABC

#include <string>

#include "keyboard.h"
#include "mouse.h"
#include "window.h"
#include "camera.h"
#include "system.h"
#include "drawable.h"


namespace   dtABC
{
   class DT_EXPORT BaseABC  :  public   dtCore::Base,  public   dtCore::KeyboardListener,  public   dtCore::MouseListener
   {
      DECLARE_MANAGEMENT_LAYER(BaseABC)

   public:
                     BaseABC( std::string name = "BaseABC" );
      virtual        ~BaseABC();

      ///configure the internal components
      virtual  void  Config( void );

      ///Quit the application (call's system quit)
      virtual  void  Quit( void );

      ///Add a visual object to the Scene
      virtual  void  AddDrawable( dtCore::Drawable* obj );

      ///Remove a visual object from the Scene
      virtual  void  RemoveDrawable( dtCore::Drawable* obj );
      
      dtCore::Window*   GetWindow( void )    { return mWindow.get(); }
      dtCore::Camera*   GetCamera( void )    { return mCamera.get(); }
      dtCore::Scene*    GetScene( void )     { return mScene.get();  }
      dtCore::Keyboard* GetKeyboard( void )  { return mKeyboard.get(); }
      dtCore::Mouse*    GetMouse( void )     { return mMouse.get(); }

   protected:
      ///Override for preframe
      virtual  void  PreFrame( const double deltaFrameTime )   = 0L;

      ///Override for frame
      virtual  void  Frame( const double deltaFrameTime )      = 0L;

      ///Override for postframe
      virtual  void  PostFrame( const double deltaFrameTime )  = 0L;

      /**
       * Base override to receive messages.
       * This method should be called from derived classes
       *
       * @param data the message to receive
       */
      virtual  void  OnMessage( MessageData *data );

      /**
       * KeyboardListener override
       * Called when a key is pressed.
       *
       * @param keyboard the source of the event
       * @param key the key pressed
       * @param character the corresponding character
       */
      virtual  void  KeyPressed( dtCore::Keyboard*          keyboard, 
                                 Producer::KeyboardKey   key,
                                 Producer::KeyCharacter  character );

      /**
       * KeyboardListener override
       * Called when a key is released.
       *
       * @param keyboard the source of the event
       * @param key the key pressed
       * @param character the corresponding character
       */
      virtual  void  KeyReleased(   dtCore::Keyboard*          keyboard, 
                                    Producer::KeyboardKey   key,
                                    Producer::KeyCharacter  character);

   protected:
      ///Create basic instances
      virtual  void  CreateInstances( void );

   protected:
      osg::ref_ptr<dtCore::Window>     mWindow;
      osg::ref_ptr<dtCore::Camera>     mCamera;
      osg::ref_ptr<dtCore::Scene>      mScene;
      osg::ref_ptr<dtCore::Keyboard>   mKeyboard;
      osg::ref_ptr<dtCore::Mouse>      mMouse;
   };
};

#endif // DELTA_BASEABC
