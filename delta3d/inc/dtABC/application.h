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
#include <dtCore/deltawin.h>

#include <xercesc/sax2/ContentHandler.hpp>  // for a base class
#include <xercesc/sax2/Attributes.hpp>      // for a parameter

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

   private:
      /** \brief A class to perform the necessary features while a Xerces SAX parser is operating.
        * Use this with the Xerces SAX2XMLReader.
        */
      class AppXMLContentHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler
      {
      public:
         AppXMLContentHandler(dtABC::Application* app): mApp(app) {}
         ~AppXMLContentHandler() {}

         // inherited pure virtual functions
         virtual void characters(const XMLCh* const chars, const unsigned int length) {}
         virtual void endDocument() {}
         virtual void endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname) {}
         virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}
         virtual void processingInstruction(const XMLCh* const target, const XMLCh* const data) {}
         virtual void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator) {}
         virtual void startDocument() {}
         virtual void startElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);
         virtual void startPrefixMapping(const	XMLCh* const prefix,const XMLCh* const uri) {}
         virtual void endPrefixMapping(const XMLCh* const prefix) {}
         virtual void skippedEntity(const XMLCh* const name) {}

      private:
         AppXMLContentHandler();   /// not implemented by design
         dtABC::Application* mApp;
      };

      friend class AppXMLContentHandler;

      /// defines API used to model the XML schema for the config file.
      class ConfigSchemaModel
      {
      public:
         static const std::string WINDOW;
         static const std::string NAME;
         static const std::string SCENE;
         static const std::string CAMERA;

         static const std::string X;
         static const std::string Y;
         static const std::string WIDTH;
         static const std::string HEIGHT;

         static const std::string PIXELDEPTH;
         static const std::string REFRESHRATE;
         static const std::string SHOWCURSOR;
         static const std::string FULLSCREEN;
         static const std::string CHANGEDISPLAYRESOLUTION;

         static const std::string WINDOWINSTANCE;
         static const std::string SCENEINSTANCE;
      };

      /// A class that writes config files for the dtABC::Application
      class AppConfigWriter
      {
      public:
         void operator ()(const std::string& filename);

         /// defines the API to obtain default values
         /// for the values used when generating an application config file.
         /// Also generates the xerces character types needed for string operations.
         class DefaultModel
         {
         public:
            DefaultModel();
            ~DefaultModel();

            XMLCh* WINDOW_NAME;
            XMLCh* WINDOW_X;
            XMLCh* WINDOW_Y;
            XMLCh* WINDOW_WIDTH;
            XMLCh* WINDOW_HEIGHT;

            XMLCh* REFRESH;
            XMLCh* PIXEL_DEPTH;
            XMLCh* SHOW_CURSOR;
            XMLCh* FULL_SCREEN;
            XMLCh* CHANGE_RESOLUTION;

            XMLCh* CAMERA_NAME;
            XMLCh* SCENE_NAME;
         };

         /// Defines the API to obtain values used when parsing the config file.
         /// Also generates the xerces character types needed for string operations.
         class SchemaModel
         {
         public:
            SchemaModel();
            ~SchemaModel();

            XMLCh* WINDOW;
            XMLCh* NAME;
            XMLCh* SCENE;
            XMLCh* CAMERA;

            XMLCh* X;
            XMLCh* Y;
            XMLCh* WIDTH;
            XMLCh* HEIGHT;

            XMLCh* PIXELDEPTH;
            XMLCh* REFRESHRATE;
            XMLCh* SHOWCURSOR;
            XMLCh* FULLSCREEN;
            XMLCh* CHANGEDISPLAYRESOLUTION;

            XMLCh* WINDOWINSTANCE;
            XMLCh* SCENEINSTANCE;
         };
      };

      /// Read the supplied config file, called from the constructor
      /// Read an existing data file and setup the internal class
      /// members with attributes from the data file.
      bool ParseConfigFile(const std::string& file);

      dtCore::DeltaWin::Resolution mOriginalRes;

      dtCore::RefPtr<dtCore::GenericKeyboardListener> mKeyboardListener;
   };

}


#endif // DELTA_APPLICATION
