/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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

#include <dtABC/export.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/deltawin.h>
#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <dtCore/deltadrawable.h>
#include <dtDAL/project.h>

namespace dtDAL
{
   class Map;
}

namespace   dtABC
{
   class DT_ABC_EXPORT BaseABC  :  public   dtCore::Base,  public   dtCore::KeyboardListener,  public   dtCore::MouseListener
   {
      DECLARE_MANAGEMENT_LAYER(BaseABC)

   public:
                     BaseABC( const std::string& name = "BaseABC" );
      virtual        ~BaseABC();

      ///configure the internal components
      virtual  void  Config();

      ///Quit the application (call's system quit)
      virtual  void  Quit();

      ///Add a visual object to the Scene
      virtual  void  AddDrawable( dtCore::DeltaDrawable* obj );

      ///Remove a visual object from the Scene
      virtual  void  RemoveDrawable( dtCore::DeltaDrawable* obj );
      
      ///Get the default Application Window
      dtCore::DeltaWin*    GetWindow()    { return mWindow.get(); }

      ///Get the default Application Camera
      dtCore::Camera*         GetCamera()    { return mCamera.get(); }

      ///Get the default Application Scene
      dtCore::Scene*          GetScene()     { return mScene.get();  }

      ///Get the default Application Keyboard
      dtCore::Keyboard*       GetKeyboard()  { return mKeyboard.get(); }

      ///Get the default Application Mouse
      dtCore::Mouse*          GetMouse()     { return mMouse.get(); }
      
      /**
       * Loads a map by name into an application.  If the map is already opened, the currently
       * loaded map will be reused. If there is a Camera contained within your Map, the default
       * Camera in BaseABC will be disabled.
       * @param name The name of the map to load.
       * @param addBillBoards pass true to add the billboards for any proxies that have the drawmode set to add the billboards.
       * @return the map that was loaded into the scene.
       * @throws ExceptionEnum::MapLoadParsingError if an error occurs reading the map file.
       * @throws ExceptionEnum::ProjectFileNotFound if the map does not exist.
       * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
       */
      dtDAL::Map& LoadMap( const std::string& name, bool addBillBoards = false )
      {
         dtDAL::Map& map = dtDAL::Project::GetInstance().GetMap(name);
         LoadMap( map, addBillBoards );
         return map;
      }
      
      /**
       * Loads a map into the scene held by BaseABC. If there is a Camera contained within your Map, the default
       * Camera in BaseABC will be disabled.
       * @param map The map to load into the scene
       * @param addBillBoards pass true to add the billboards for any proxies that have the drawmode set to add the billboards.
       * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
       */
      void LoadMap( dtDAL::Map& map, bool addBillBoards = false );

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
      dtCore::RefPtr<dtCore::DeltaWin>         mWindow; ///<built-in Window
      dtCore::RefPtr<dtCore::Camera>           mCamera; ///<built-in Camera
      dtCore::RefPtr<dtCore::Scene>            mScene; ///<built-in Scene
      dtCore::RefPtr<dtCore::Keyboard>         mKeyboard; ///<built-in Keyboard
      dtCore::RefPtr<dtCore::Mouse>            mMouse;  ///<built-in Mouse
   };
};

#endif // DELTA_BASEABC
