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
     * Application *app = new Application("Mydatafile.xml");
     * app->Config();
     * app->Run();
     */
   class DT_ABC_EXPORT Application : public dtABC::BaseABC
   {
      DECLARE_MANAGEMENT_LAYER(Application)

   public:
      Application( const std::string& configFilename = "" );
      virtual ~Application();

      ///Start the Application
      virtual void Run();

      ///Generate a default configuration file
      static std::string GenerateDefaultConfigFile();

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


      /// Read the supplied config file, called from the constructor
      bool ParseConfigFile(const std::string& file);

      dtCore::DeltaWin::Resolution mOriginalRes;
   };
}

#endif // DELTA_APPLICATION
