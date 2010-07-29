#ifndef databasepager_h__
#define databasepager_h__

#include <dtCore/export.h>
#include <dtCore/base.h>

#include <osg/Version>
#include <osgDB/DatabasePager>
#include <osg/ref_ptr>

namespace dtUtil
{
   class ConfigProperties;
}


namespace dtCore
{

   class DeltaDrawable;

   /** Used to control how any potential page-able files are loaded.
     * Currently based on the osgDB::DatabasePager which uses osg::PagedLOD
     * nodes.
     */
   class DT_CORE_EXPORT DatabasePager : public dtCore::Base
   {
   public:

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

      /**
       * Configuration property.
       * <br>
       * integer in tiles.
       * <br>
       * Setting this to non-zero sets the database pager to use a max tile based mechanism for loading tiles.
       * Setting it to 0 uses the older timeout expiring mechanism.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_MAX_PAGED_LOD;

      DatabasePager();

      /** Overloaded constructor to supply a custom internal pager to use.
        * @param pager : a custom DatabasePager to use instead of the default one
        */
      DatabasePager( osgDB::DatabasePager &pager );


      /** Search the supplied DeltaDrawable for any page-able nodes and register
        * them with the DatabasePager.
        * @param drawable : the DeltaDrawable which potentially contains page-able nodes.
        */
      void RegisterDrawable( dtCore::DeltaDrawable &drawable );

      /** Get the internal OSG database pager instance.
        * @note For advanced usage only
        */
      const osgDB::DatabasePager* GetOsgDatabasePager() const {return mDatabasePager.get();}

      /** Get the internal OSG database pager instance.
        * @note For advanced usage only
        */
      osgDB::DatabasePager* GetOsgDatabasePager() {return mDatabasePager.get();}


      /** Supply the properties to configure this instance.
        * @param config : the configuration properties to use
        */
      void SetConfiguration(dtUtil::ConfigProperties* config);

      /** Get the configuration properties used by this class.
        * @return The configuration properties (could be NULL)
        */
      dtUtil::ConfigProperties* GetConfiguration();

      /** Get the configuration properties used by this class.
      * @return The configuration properties (could be NULL)
      */
      const dtUtil::ConfigProperties* GetConfiguration() const;

      /**
      * Sets target frame rate for database pager, default 60
      * @param targetFR:  framerate for paging thread in Frames / Sec
      */
      void SetTargetFrameRate(double targetFR);

      /**
      * return target frame rate for database pager, default 60
      */
      double GetTargetFrameRate() const;

      /** 
      * Tell the database pager that a new frame has begun.  This will
      * put the thread to sleep so the CPU can work on the
      * rendering.
      * @see SignalEndFrame()
      */
      virtual void SignalBeginFrame(const osg::FrameStamp* framestamp) const;

      /** 
      * Tell the database pager that the frame has ended.  This will
      * wake up the thread for work, now that the other threads are idle.
      */
      virtual void SignalEndFrame() const;

      /** Merge changes to the scene graph.  Note: Must only be called
      * from the single thread update phase.
      */
#if OPENSCENEGRAPH_MAJOR_VERSION < 2 || (OPENSCENEGRAPH_MAJOR_VERSION == 2 && OPENSCENEGRAPH_MINOR_VERSION <= 6)
      virtual void UpdateSceneGraph(double currentFrameTime) const;
#else
      virtual void UpdateSceneGraph(const osg::FrameStamp* framestamp) const;
#endif

      /** 
      * Compile the rendering objects.  Should only be called from the 
      * draw thread and requires a valid OpenGL context
      */
      virtual void CompileGLObjects(osg::State& state, double& availableTime) const;

   protected:
      virtual ~DatabasePager();

   private:
      osg::ref_ptr<osgDB::DatabasePager> mDatabasePager;
      dtUtil::ConfigProperties* mConfigProperties; ///<deprecated 06/30/08

   };
}
#endif // databasepager_h__

