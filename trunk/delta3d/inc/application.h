#ifndef DELTA_APPLICATION
#define DELTA_APPLICATION

#include "baseabc.h"
#include "tinyxml.h"
#include "deltawin.h"

namespace dtABC
{
   ///Base generic Application class

   /** The Application class of the dtCore Application Base Class library
     * is the base level class for most applications.  It contains the basic
     * components required for applications.
     * An optional XML configuration file can be supplied on the constructor which
     * contains the attributes for the internal DeltaWin, Camera, and Scene.  If no
     * config file is supplied, a default file ("config.xml") is saved
     * which may be customized and passed in on the next run.
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
               void  ParseConfigFile( TiXmlElement* rootNode );
              

               dtCore::Resolution mOriginalRes;
   };
}

#endif // DELTA_APPLICATION
