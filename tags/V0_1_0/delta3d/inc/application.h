#pragma once
#include "baseabc.h"
#include "tinyxml.h"



namespace dtABC
{
   ///Base generic Application class

   /** The Application class of the dtCore Application Base Class library
     * is the base level class for most applications.  It contains the basic
     * components required for applications.
     * An optional XML configuration file can be supplied on the constructor which
     * contains the attributes for the internal Window, Camera, and Scene.  If no
     * config file is supplied, a default file ("config.xml") is saved
     * which may be customized and passed in on the next run.
     *
     * Typical use:
     * Application *app = new Application("Mydatafile.xml");
     * app->Config();
     * app->Run();
     */
   class Application : public dtABC::BaseABC
   {
      DECLARE_MANAGEMENT_LAYER(Application)

   public:
                     Application( std::string configFilename = "" );
      virtual        ~Application();

               ///Start the Application
               void  Run( void );

               void  DisplayDebugGUI( const bool enable = true );

   protected:
      ///override for preframe 
      virtual  void  PreFrame( const double deltaFrameTime );

      ///override for frame
      virtual  void   Frame( const double deltaFrameTime );

      ///override for postframe
      virtual  void  PostFrame( const double deltaFrameTime );

   private:
      ///Create basic instances and set up system hooks
      virtual  void  CreateInstances( void );
               void  ParseConfigFile( TiXmlElement* rootNode );
               void  GenerateConfigFile( void );
   };

/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2

#if defined(_DEBUG)
   #define _AUTOLIBNAME "tinyxmld.lib"
   #define _AUTOLIBNAME1 "dtabcD.lib"
   #define _AUTOLIBNAME2 "gui_fld.lib"
#else 
   #define _AUTOLIBNAME "tinyxml.lib"
   #define _AUTOLIBNAME1 "dtabc.lib"  
   #define _AUTOLIBNAME2 "gui_fl.lib"
#endif

#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME )
   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
#endif

#pragma  comment( lib, _AUTOLIBNAME )
#pragma  comment( lib, _AUTOLIBNAME1 )
#pragma  comment( lib, _AUTOLIBNAME2 )
}