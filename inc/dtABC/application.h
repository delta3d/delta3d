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
#include <dtCore/generickeyboardlistener.h>
#include <dtCore/genericmouselistener.h>
#include <dtUtil/configproperties.h>
#include <dtUtil/logtimeprovider.h>

#include <string>
#include <map>

///@cond IGNORE
namespace osgViewer
{
   class CompositeViewer;
   class Viewer;
}
///@endcond

namespace dtCore
{
   class Keyboard;
   class Mouse;
   class StatsHandler;
   class DeltaWin;
}

namespace dtABC
{
   struct ApplicationConfigData;
   class ApplicationConfigHandler;

   ///Base generic Application class

   /**
    * The Application class of the dtCore Application Base Class library
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
   class DT_ABC_EXPORT Application : public dtABC::BaseABC, public dtUtil::ConfigProperties, public dtUtil::LogTimeProvider
   {
      DECLARE_MANAGEMENT_LAYER(Application)
      typedef dtABC::BaseABC BaseClass;

   public:
      /**
       * ConfigProperty <tt>"System.SimFrameRate"</tt>.
       * <br>
       * Sets the fixed simulated frame rate of the system. This only matters if a fixed time step
       * is used.
       * @see dtCore::System
       * @see GetConfigPropertyValue();
       * @see USE_FIXED_TIME_STEP
       */
      static const std::string SIM_FRAME_RATE;

      /**
       * ConfigProperty <tt>"System.MaxTimeBetweenDraws"</tt>.
       * <br>
       * When using a fixed time step, it is possible that the time required to simulate could be
       * so great that the system would never have time to draw a frame.  This time is used an as
       * override so that it be guaranteed to at least draw a frame every so often.  This time is a
       * floating point number in seconds.
       * @see dtCore::System
       * @see GetConfigPropertyValue();
       */
      static const std::string MAX_TIME_BETWEEN_DRAWS;

      /**
       * ConfigProperty <tt>"System.UseFixedTimeStep"</tt>.
       * <br>
       * Set to <tt>"true"</tt> or <tt>"false"</tt>.
       * <br>
       * This value defaults to false, which will make the delta time be equivalent to the time since the
       * beginning of the last frame times the current time scale. If this is set to true, the delta
       * time will be a fixed value multiplied times the time scale.  This helps make things like motion models
       * physics, and other time-based calculations deterministic.  They also won't suffer from
       * anomalies that occur with frame hiccups.
       * @see dtCore::System
       * @see GetConfigPropertyValue();
       */
      static const std::string USE_FIXED_TIME_STEP;

      /**
       * ConfigProperty <tt>"ThreadPool.NumWorkerThreads"</tt>.
       * <br>
       * Set to the number of worker threads requested for the global ThreadPool service in dtUtil.
       * If set to <tt>"OFF"</tt>, no ThreadPool threads will be created.
       * <br>
       *
       * The default is to just call the thread pool init with default settings.
       * @see dtUtil::ThreadPool
       * @see GetConfigPropertyValue();
       */
      static const std::string NUM_WORKER_THREADS;

      Application(const std::string& configFilename = "", dtCore::DeltaWin* win = NULL);

      ///Start the Application
      void Run() override;

      //overridden the load config properties into settings.
      void Config() override;

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

      /// @return the instance of the keyboard listener used for callbacks
      const dtCore::GenericKeyboardListener* GetKeyboardListener() const { return mKeyboardListener.get(); }
      /// @return the instance of the keyboard listener used for callbacks
      dtCore::GenericKeyboardListener* GetKeyboardListener() { return mKeyboardListener.get(); }


      /**
       * Called when a mouse button is pressed.  Overwrite for custom functionality.
       * @param mouse Handle to the Mouse that triggered this
       * @param button The button index
       */
      virtual bool MouseButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);

      /**
       * Called when a mouse button is released.  Overwrite for custom functionality.
       * @param mouse Handle to the Mouse that triggered this
       * @param button The button index
       */
      virtual bool MouseButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);

      /**
       * Called when a mouse button has been "double-clicked".  Overwrite for custom
       * functionality.
       * @param mouse Handle to the Mouse that triggered this
       * @param button The button index
       * @param clickCount : The number of times the button was clicked
       */
      virtual bool MouseButtonDoubleClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount);

      /**
       * Called when a mouse is moved.  Overwrite for custom functionality.
       * @param mouse Handle to the Mouse that triggered this
       * @param x The left-right distance the mouse traveled
       * @param y The up-down distance the mouse traveled
       */
      virtual bool MouseMoved(const dtCore::Mouse* mouse, float x, float y);


      /**
       * Called when a mouse is dragged (button down).  Overwrite for custom functionality.
       * @param mouse Handle to the Mouse that triggered this
       * @param x The left-right distance the mouse traveled
       * @param y The up-down distance the mouse traveled
       */
      virtual bool MouseDragged(const dtCore::Mouse* mouse, float x, float y);

      /**
       * Called when a mouse scroll wheel moved.  Overwrite for custom functionality.
       * @param mouse Handle to the Mouse that triggered this
       * @param delta The amount of wheel scrolled
       */
      virtual bool MouseScrolled(const dtCore::Mouse* mouse, int delta);

      /// @return the instance of the mouse listener used for callbacks
      const dtCore::GenericMouseListener* GetMouseListener() const { return mMouseListener.get(); }
      /// @return the instance of the mouse listener used for callbacks
      dtCore::GenericMouseListener* GetMouseListener() { return mMouseListener.get(); }

      /// the publicized default settings for a generated config file.
      static ApplicationConfigData GetDefaultConfigData();

      /**
        * Get a ConfigProperty value by name.
        * @param name The name of the value to look for
        * @param defaultValue The default value to use if name isn't found
        * @return a string value that is paired with the given name.  The default is returned if the property is not set.
       */
      const std::string& GetConfigPropertyValue(const std::string& name, const std::string& defaultValue = "") const override;

      void GetConfigPropertiesWithPrefix(const std::string& prefix, std::vector<std::pair<std::string,std::string> >& resultOut, bool removePrefix = true) const override;

      /// Sets the value of a given config property.
      void SetConfigPropertyValue(const std::string& name, const std::string& value) override;

      /// Removes a property with the given name
      void RemoveConfigPropertyValue(const std::string& name) override;

      /// Returns whether a config property exists or not
      bool IsConfigPropertyDefined(const std::string& name) const override;

      /// Add a view to the Viewer
      void AddView(dtCore::View& view);

      /**
       * Remove a View from the Viewer.  This View will be removed during the
       * next PostFrame(), unless the "immediately" flag is set to true.  Views that
       * are removed immediately could crash the application if removed at the
       * incorrect time.
       * @param view The View to remove.
       * @param immediately : Advanced flag to indicate the View should be removed
       * immediately instead of deferring until the next PostFrame().  Use with
       * caution.
       */
      void RemoveView(dtCore::View& view, bool immediately = false);

      // Checks to see if a view is added the application.
      bool ContainsView(dtCore::View& view);

      ///Cycle through the statistics modes
      void SetNextStatisticsType();

      /// @return the instance of the osgViewer::CompositeViewer
      const osgViewer::CompositeViewer* GetCompositeViewer() const { return mCompositeViewer.get(); }

      /// @return the instance of the osgViewer::CompositeViewer
      osgViewer::CompositeViewer* GetCompositeViewer() { return mCompositeViewer.get(); }

      /******** Log Time Provider functions ********/
	  const dtUtil::DateTime& GetDateTime() override;
	  unsigned GetFrameNumber() override;
	  osg::Referenced* AsReferenced() override;
      /******** Log Time Provider functions end ********/

     ///called on the OSG Composite Viewer
     void StartThreading();

     ///called on the OSG Composite Viewer
     void StopThreading();
          
     /// returns true if the application is running in threaded mode with active threads
     bool GetThreadsActive() const;

   protected:
      virtual ~Application();

      void EventTraversal(const double deltaSimTime) override;

      ///override for preframe
      void PreFrame(const double deltaSimTime) override;

      ///override for frame
      void Frame(const double deltaSimTime) override;

      ///override for postframe
      void PostFrame(const double deltaSimTime) override;

      ///override for pause -- replaces PreFrame when game is paused.
      void Pause(const double deltaRealTime) override;

      ///Create basic instances and set up system hooks
      virtual void CreateInstances(const ApplicationConfigData& configData);/*const std::string& name = "defaultWin", int x = 100, int y = 100,
         int width = 640, int height = 480, bool cursor = true, bool fullScreen = false, bool realizeUponCreate = true);*/

      /**
       * Read the supplied config file, called from the constructor
       * Read an existing data file and setup the internal class
       * members with attributes from the data file.
       * @param file the name of the data file to be parsed.
       * @param handler The XML config file handler used to support the parsing
       * @return true if the file was read and parsed correctly, false otherwise
       */
      bool ParseConfigFile(const std::string& file, ApplicationConfigHandler& handler) const;

      /**
       * Apply the config file data to the previously created Application members.
       * @param handler The handler which was used to parse the config file
       * @return True if the data was applied correctly, false otherwise
       */
      bool ApplyConfigData(const ApplicationConfigHandler &handler);

       /**
        * Forces the application to re-read the set of config properties it handles.
        * This is virtual so a subclass can add new properties.
        */
      virtual void ReadSystemProperties();

   private:
      ///private method to remove a view. Must *not* be called during event traversal.
      void RemoveViewImpl(dtCore::View& view);

      bool mFirstFrame;
      bool mThreadPoolInitialized;
      dtCore::RefPtr<osgViewer::CompositeViewer> mCompositeViewer;

      dtCore::RefPtr<dtCore::GenericKeyboardListener> mKeyboardListener;
      dtCore::RefPtr<dtCore::GenericMouseListener>    mMouseListener;

      typedef std::map<std::string, std::string> AppConfigPropertyMap;
      AppConfigPropertyMap mConfigProperties;

      dtCore::StatsHandler *mStats; ///<for stats rendering/controlling
      ViewList mViewsToDelete; ///<list of Views to be removed at the end of the frame

      // Used to provide to the logger
      dtUtil::DateTime mCurrentFrameTime;
   };

} // namespace dtABC

#endif // DELTA_APPLICATION
