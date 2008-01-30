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

#include <dtCore/base.h>        // for base class
#include <string>
#include <map>

#include <dtABC/export.h>
#include <dtCore/refptr.h>      // for members
#include <dtCore/camera.h>      // for accessor
#include <dtCore/view.h>      // for accessor

namespace dtDAL
{
   class Map;
}

namespace dtCore
{
   class DeltaDrawable;
   class Keyboard;
   class Mouse;
   class DeltaWin;
   class Scene;
}

namespace dtABC
{

class DT_ABC_EXPORT BaseABC : public dtCore::Base
{
   DECLARE_MANAGEMENT_LAYER(BaseABC)

   public:
      BaseABC( const std::string& name = "BaseABC" );
      
   protected:
      virtual ~BaseABC();

   public:
      ///configure the internal components
      virtual void Config();
   
      ///Quit the application (call's system quit)
      virtual void Quit();
   
      ///Add a visual object to the Scene
      virtual void AddDrawable( dtCore::DeltaDrawable* obj );
   
      ///Remove a visual object from the Scene
      virtual void RemoveDrawable( dtCore::DeltaDrawable* obj );
   
   
      ///Get the first view or create a default View
      dtCore::View* GetView() { return  mViewList[0].get(); }
      ///Get the (const) first view
      const dtCore::View* GetView() const { return  mViewList[0].get(); }

      ///Set the first view
      void SetView(dtCore::View * view)
      { mViewList[0] = view; }
      
      ///Get the default Application Camera
      dtCore::Camera* GetCamera() { return GetView()->GetCamera(); }
      ///Get the default (const) Application Camera
      const dtCore::Camera* GetCamera() const { return GetView()->GetCamera(); }
      ///Get the default Application Camera
      void SetCamera(dtCore::Camera * camera);
      
      ///Get the default Application Window
      dtCore::DeltaWin* GetWindow();
      ///Get the default Application Window
      void SetWindow(dtCore::DeltaWin * win);
      
      ///Get the default Application Scene
      dtCore::Scene* GetScene() { return GetView()->GetScene(); }
      ///Set the default Application Scene
      void SetScene(dtCore::Scene* scene) { GetView()->SetScene(scene); }


      ///Get the default Application Keyboard
      dtCore::Keyboard* GetKeyboard() { return GetView()->GetKeyboard(); }
      ///Set the default Application Scene
      void SetKeyboard(dtCore::Keyboard* keyboard) { GetView()->SetKeyboard(keyboard); }
         
      ///Get the default Application Mouse
      dtCore::Mouse* GetMouse() {  return GetView()->GetMouse();}
      ///Set the default Application Scene
      void SetMouse(dtCore::Mouse* mouse) { GetView()->SetMouse(mouse); }


      /// Loads a map by name into an application.  If the map is already opened, the currently
      /// loaded map will be reused. If there is a Camera contained within your Map, the default
      /// Camera in BaseABC will be disabled.
      /// @param name The name of the map to load.
      /// @param addBillBoards pass true to add the billboards for any proxies that have the drawmode set to add the billboards.
      /// @return the map that was loaded into the scene.
      /// @throws ExceptionEnum::MapLoadParsingError if an error occurs reading the map file.
      /// @throws ExceptionEnum::ProjectFileNotFound if the map does not exist.
      /// @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
      dtDAL::Map& LoadMap( const std::string& name, bool addBillBoards = false );

      /// Loads a map into the scene held by BaseABC. If there is a Camera contained within your Map, the default
      /// Camera in BaseABC will be disabled.
      /// @param map The map to load into the scene
      /// @param addBillBoards pass true to add the billboards for any proxies that have the drawmode set to add the billboards.
      /// @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
      void LoadMap( dtDAL::Map& map, bool addBillBoards = false );

   protected:
   
      ///Override for preframe
      virtual void PreFrame( const double deltaSimTime ) = 0L;
   
      ///Override for frame
      virtual void Frame( const double deltaSimTime ) = 0L;
   
      ///Override for postframe
      virtual void PostFrame( const double deltaSimTime ) = 0L;
   
      /// Base override to receive messages.
      /// This method should be called from derived classes
      /// @param data the message to receive
      virtual void OnMessage( MessageData *data );

      ///Create the basic instances
      virtual void CreateInstances();
   
      virtual dtCore::View * CreateDefaultView();

      dtCore::RefPtr<dtCore::DeltaWin> mWindow;

      typedef std::vector<dtCore::RefPtr<dtCore::View> > ViewList;
      ViewList mViewList;
   
   private:
      void KeyPressed(const dtCore::Keyboard* keyboard, int); ///< private & unimplemented by design.
      void HandleKeyPressed(const dtCore::Keyboard* keyboard, int); ///< private & unimplemented by design.
      void KeyReleased(const dtCore::Keyboard* keyboard, int); ///< private & unimplemented by design.
      void HandleKeyReleased(const dtCore::Keyboard* keyboard, int); ///< private & unimplemented by design.
};
}

#endif // DELTA_BASEABC
