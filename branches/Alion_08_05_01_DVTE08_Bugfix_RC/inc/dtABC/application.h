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

#include <dtCore/refptr.h>
#include <dtABC/baseabc.h>
#include <dtABC/export.h>

#include <dtUtil/configproperties.h>

#include <string>
#include <map>

///@cond 
namespace osgViewer
{
   class CompositeViewer;
   class Viewer;
}
///@endcond

namespace dtCore
{
   class GenericKeyboardListener;
   class StatsHandler;
   class DeltaWin;
}

namespace dtABC
{
   struct ApplicationConfigData;
   
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
   class DT_ABC_EXPORT Application : public dtABC::BaseABC, public dtUtil::ConfigProperties
   {
      DECLARE_MANAGEMENT_LAYER(Application)
      typedef dtABC::BaseABC BaseClass;

   public:
      /**
       * Configuration property.
       * <br>
       * Sets the fixed simulated frame rate of the system. This only matters if a fixed time step
       * is used.
       * @see dtCore::System
       */
      static const std::string SIM_FRAME_RATE;

      /**
       * Configuration property.
       * <br>
       * When using a fixed time step, it is possible that the time required to simulate could be
       * so great that the system would never have time to draw a frame.  This time is used an as
       * override so that it be guaranteed to at least draw a frame every so often.  This time is a
       * floating point number in seconds.
       * @see dtCore::System
       */
      static const std::string MAX_TIME_BETWEEN_DRAWS;
      
      /**
       * Configuration property.
       * <br>
       * Set to true or false.
       * <br>
       * This value defaults to false, which will make the delta time be equivalent to the time since the
       * beginning of the last frame times the current time scale. If this is set to true, the delta
       * time will be a fixed value multiplied times the time scale.  This helps make things like motion models
       * physics, and other time-based calculations deterministic.  They also won't suffer from
       * anomalies that occur with frame hiccups.
       * @see dtCore::System
       */
      static const std::string USE_FIXED_TIME_STEP;

      /**
       * Configuration property.
       * <br>
       * Set to true or false
       * <br>
       * This will set the database pager for the default view to precompile gl objects or not.
       * If they are not precomplied, they will be compiled when they are first viewed, which can
       * cause major frame hiccups in some cases.  It defaults to true.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_PRECOMPILE_OBJECTS;

      /**
       * Configuration property.
       * <br>
       * Unsigned integer
       * <br>
       * The max number of GL objects to compile per frame.  This number should be low.  The default is
       * 2.  Making it higher will make tiles page in sooner, but it will also cause more of a frame
       * hiccup.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_MAX_OBJECTS_TO_COMPILE_PER_FRAME;

      /**
       * Configuration property.
       * <br>
       * floating point number
       * <br>
       * The minimum amount of time to allocate from pre-compiling GL objects in a paged database
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_MIN_TIME_FOR_OBJECT_COMPILE;

      /**
       * Configuration property.
       * <br>
       * floating point in frames per second.
       * <br>
       * The target frame rate.  The pager uses this to time certain operations.  
       * If the system is set to use a fixed time step, the target frame rate is set to match the
       * fixed frame rate (SIM_FRAME_RATE). Otherwise, it defaults to 100.
       * Turning this number down can improve paging preformance somewhat.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_TARGET_FRAMERATE;

      /**
       * Configuration property.
       * <br>
       * DoNotModify, DisplayList, VBO or VertexArrays
       * <br>
       * This defaults to DoNotModify, but changing it will tell the pager to alter the drawing settings
       * on loaded pages.  DisplayList will force drawables to draw using display lists.  VBO will tell
       * drawables to draw using Vertex Buffer Objects.  VertexArrays will tell the drawables to draw using
       * standard vertex arrays with no performance tricks.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_DRAWABLE_POLICY;

      /**
       * Configuration property.
       * <br>
       * Set the thread priority to DEFAULT, MIN, LOW, NOMINAL, HIGH or MAX.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_THREAD_PRIORITY;

      /**
       * Configuration property.
       * <br>
       * floating point in seconds.
       * <br>
       * the time in seconds of not being rendered before the database pager deletes pages.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_EXPIRY_DELAY;

      Application( const std::string& configFilename = "", dtCore::DeltaWin *win = NULL );

      ///Start the Application
      virtual void Run();

      //overridden the load config properties into settings.
      virtual void Config();

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
      virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int kc);

      /// Called when a key is released.
      /// @param keyboard the source of the event
      /// @param key the key released
      virtual bool KeyReleased(const dtCore::Keyboard* keyboard, int kc);

      /// @return the instance of the listener used for callbacks
      const dtCore::GenericKeyboardListener* GetKeyboardListener() const { return mKeyboardListener.get(); }
      /// @return the instance of the listener used for callbacks
      dtCore::GenericKeyboardListener* GetKeyboardListener() { return mKeyboardListener.get(); }

      /// the publicized default settings for a generated config file.
      static ApplicationConfigData GetDefaultConfigData();

      /// @return a string value that is paired with the given name.  The default is returned if the property is not set.
      const std::string& GetConfigPropertyValue(const std::string& name, const std::string& defaultValue = "") const;
      
      /// Sets the value of a given config property.
      void SetConfigPropertyValue(const std::string& name, const std::string& value);

      /// Removes a property with the given name
      void RemoveConfigPropertyValue(const std::string& name);
      
      /// Add a view to the Viewer
      void AddView(dtCore::View &view);
      
      /// Remove a view to the Viewer
      void RemoveView(dtCore::View &view);

      ///Cycle through the statistics modes
      void SetNextStatisticsType();

   protected:

      virtual ~Application();
      ///override for preframe 
      virtual  void  PreFrame( const double deltaSimTime );

      ///override for frame
      virtual  void   Frame( const double deltaSimTime );

      ///override for postframe
      virtual  void  PostFrame( const double deltaSimTime );

      ///Create basic instances and set up system hooks
      virtual void CreateInstances(const std::string& name="defaultWin", int x=100, int y=100, int width=640, int height=480, bool cursor=true, bool fullScreen=false );

      /// Read the supplied config file, called from the constructor
      /// Read an existing data file and setup the internal class
      /// members with attributes from the data file.
      /// @param file the name of the data file to be parsed.
      /// @return true, if both parsing and applying went well.
      bool ParseConfigFile(const std::string& file);

      /// @return the instance of the osgViewer::CompositeViewer
      const osgViewer::CompositeViewer* GetCompositeViewer() const { return mCompositeViewer.get(); }

      /// @return the instance of the osgViewer::CompositeViewer
      osgViewer::CompositeViewer* GetCompositeViewer() { return mCompositeViewer.get(); }

      /**
       * Forces the application to re-read the set of config properties it handles.
       * This is virtual so a subclass can add new properties.
       */
      virtual void ReadSystemProperties();
   private:

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

      dtCore::RefPtr<osgViewer::CompositeViewer> mCompositeViewer;

      dtCore::RefPtr<dtCore::GenericKeyboardListener> mKeyboardListener;

      typedef std::map<std::string, std::string> AppConfigPropertyMap;
      AppConfigPropertyMap mConfigProperties;

      dtCore::StatsHandler *mStats; ///<for stats rendering/controlling
   };

}

#endif // DELTA_APPLICATION
