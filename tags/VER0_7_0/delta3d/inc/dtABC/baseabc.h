/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef DELTA_BASEABC
#define DELTA_BASEABC

#include <string>

#include "dtCore/keyboard.h"
#include "dtCore/mouse.h"
#include "dtCore/deltawin.h"
#include "dtCore/camera.h"
#include "dtCore/system.h"
#include "dtCore/deltadrawable.h"


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
      virtual  void  AddDrawable( dtCore::DeltaDrawable* obj );

      ///Remove a visual object from the Scene
      virtual  void  RemoveDrawable( dtCore::DeltaDrawable* obj );
      
      ///Get the default Application Window
      dtCore::DeltaWin*    GetWindow( void )    { return mWindow.get(); }

      ///Get the default Application Camera
      dtCore::Camera*         GetCamera( void )    { return mCamera.get(); }

      ///Get the default Application Scene
      dtCore::Scene*          GetScene( void )     { return mScene.get();  }

      ///Get the default Application Keyboard
      dtCore::Keyboard*       GetKeyboard( void )  { return mKeyboard.get(); }

      ///Get the default Application Mouse
      dtCore::Mouse*          GetMouse( void )     { return mMouse.get(); }

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
      osg::ref_ptr<dtCore::DeltaWin>      mWindow; ///<built-in Window
      osg::ref_ptr<dtCore::Camera>           mCamera; ///<built-in Camera
      osg::ref_ptr<dtCore::Scene>            mScene; ///<built-in Scene
      osg::ref_ptr<dtCore::Keyboard>         mKeyboard; ///<built-in Keyboard
      osg::ref_ptr<dtCore::Mouse>            mMouse;  ///<built-in Mouse
   };
};

#endif // DELTA_BASEABC