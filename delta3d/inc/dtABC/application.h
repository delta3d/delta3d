#ifndef DELTA_APPLICATION
#define DELTA_APPLICATION

#include "dtABC/baseabc.h"
#include "tinyxml.h"
#include "dtCore/deltawin.h"

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
   class DT_EXPORT Application : public dtABC::BaseABC
   {
      DECLARE_MANAGEMENT_LAYER(Application)

   public:
                     Application( std::string configFilename = "" );
      virtual        ~Application();

      ///Start the Application
      virtual  void  Run( void );

      ///Generate a default configuration file
      void  GenerateDefaultConfigFile( void );

   protected:
      ///override for preframe 
      virtual  void  PreFrame( const double deltaFrameTime );

      ///override for frame
      virtual  void   Frame( const double deltaFrameTime );

      ///override for postframe
      virtual  void  PostFrame( const double deltaFrameTime );


   private:
      ///Create basic instances and set up system hooks
      virtual  void  CreateInstances( std::string name="defaultWin", int x=100, int y=100, int width=640, int height=480, bool cursor=true, bool fullScreen=false );
              
               ///Read the supplied config file
               void  ParseConfigFile( TiXmlElement* rootNode );
              
               dtCore::Resolution mOriginalRes;
   };
}

#endif // DELTA_APPLICATION
