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

#ifndef DELTA_APPLICATION
#define DELTA_APPLICATION

#include <dtABC/baseabc.h>
#include <dtABC/export.h>
#include <dtABC/applicationconfigdata.h>           // for return type, member

namespace dtCore
{
   class GenericKeyboardListener;
}

namespace dtABC
{
   ///Base generic Application class

   /** The Application class of the dtCore Application Base Class library
     * is the base level class for most applications.  It contains the basic
     * components required for applications.
     * An optional XML configuration file can be supplied on the constructor which
     * contains the attributes for the internal DeltaWin, Camera, and Scene.  
     * A default Config File can be created by calling GenerateDefaultConfigFile().
     * This file will contain the default parameters and can be edited, then 
     * supplied to the constructor.
     *
     * Typical use:
     * \code
     * Application *app = new Application("Mydatafile.xml");
     * app->Config();
     * app->Run();
     * \endcode     
     */
   class DT_ABC_EXPORT Application : public dtABC::BaseABC
   {
      DECLARE_MANAGEMENT_LAYER(Application)

   public:
      Application( const std::string& configFilename = "" );

   protected:
      virtual ~Application();

   public:
      ///Start the Application
      virtual void Run();

      /// Generate a default configuration file.
      /// This method writes out all the default attributes from the internal Application
      /// members and writes them out to a .xml file ("config.xml").
      /// @param the file path to be used when writing.
      /// @return the file path to the newly created file, as seen by the delta3d resource management tool,
      /// unless the file already exists, then the path to the existing file is returned.
      static std::string GenerateDefaultConfigFile(const std::string& filename="config.xml");

      /// Called when a key is pressed.
      /// @param keyboard the source of the event
      /// @param key the key pressed
      /// @param character the corresponding character
      virtual bool KeyPressed(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character);

      /// @return the instance of the listener used for callbacks
      const dtCore::GenericKeyboardListener* GetKeyboardListener() const { return mKeyboardListener.get(); }
      /// @return the instance of the listener used for callbacks
      dtCore::GenericKeyboardListener* GetKeyboardListener() { return mKeyboardListener.get(); }

   protected:
      ///override for preframe 
      virtual  void  PreFrame( const double deltaFrameTime );

      ///override for frame
      virtual  void   Frame( const double deltaFrameTime );

      ///override for postframe
      virtual  void  PostFrame( const double deltaFrameTime );

      ///Create basic instances and set up system hooks
      virtual void CreateInstances(const std::string& name="defaultWin", int x=100, int y=100, int width=640, int height=480, bool cursor=true, bool fullScreen=false );

   public:
      /// the publicized default settings for a generated config file.
      static ApplicationConfigData GetDefaultConfigData();

   private:
      /// Read the supplied config file, called from the constructor
      /// Read an existing data file and setup the internal class
      /// members with attributes from the data file.
      /// @param file the name of the data file to be parsed.
      /// @return true, if both parsing and applying went well.
      bool ParseConfigFile(const std::string& file);

      /// A utility to apply the parsed data to the Application instance
      class AppXMLApplicator
      {
      public:
         /// the method to apply the data
         /// @param data The data to be applied
         /// @param app The application to apply the data to
         /// @return true, if all went well.
         bool operator ()(const ApplicationConfigData& data, Application* app);
      };

      dtCore::RefPtr<dtCore::GenericKeyboardListener> mKeyboardListener;
   };

}

#endif // DELTA_APPLICATION
