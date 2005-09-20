#ifndef PROFMGR_INCLUDED
#define PROFMGR_INCLUDED

#include "dtCore/deltadrawable.h"
#include "dtCore/keyboard.h"
#include "dtCore/prof.h"
#include "dtCore/prof_internal.h"

#include <osgText/Text>
#include <osg/Geode>

namespace dtCore
{
   ///A class which handles the management of the code profiler

   /** The ProfileMgr will handle the data collecting and rendering
    *  using the IProf profiler (http://silverspaceship.com/src/iprof/).
    *
    *  To use this class, create an instance and add it to the Scene.  The
    *  data can be rendered using EnableRender() and the graphic chart can
    *  be rendered using RenderGraph().
    *
    *  To profile your code, include this header file and use the
    *  Prof(method_name) macro:
    *
    *  \code
    * #include "dtCore/profilemgr.h"
    * 
    * void myFunction(void)
    * {
    *    Prof(my_function_name);
    *    //code goes here
    * }
    *  \endcode
    *
    *  There are a number of a user interface controls to adjust the profiler
    *  displays.  The ProfileMgr can use its internal keyboard mapping or the
    *  user can call the ProfileMgr API directly.  To use the internal keyboard
    *  mapping, call UseDefaultKeyboardMapping().
    * 
    *  \li F10 = Render the profile data
    *  \li F11 = Change the report mode
    *  \li g   = Render the profile graph
    *  \li up  = Move the cursor up
    *  \li dn  = Move the cursor down
    *  \li ent = Select
    *  \li p   = Pause the data collection
    * 
    */
   class DT_EXPORT ProfileMgr : public dtCore::DeltaDrawable,
                                public dtCore::KeyboardListener
   {
   public:
      const static short MAX_NUM_PROF_RECORDS = 20;

      ProfileMgr(float sx=0.f, float sy=1000.f);
      virtual ~ProfileMgr(void);

      ///Enable/disable the rendering of the profiler data
      void EnableRender(const bool enable=true);
      bool IsRendering(void) const {return mRenderEnabled;}

      ///Enable/disable the rendering of the profiler graph chart
      void RenderGraph( const bool enable=true);
      bool IsGraphRendered(void) const {return mGraphRendered;};

      ///Pause the gathering of profiler data
      void Pause(const bool pause=true) {mPaused = pause;}
      bool IsPaused(void) const {return mPaused;}

      ///Setup and use the default keyboard mappings
      void UseDefaultKeyboardMapping(const bool enable=true);
      bool IsUsingDefaultkeyboardMapping(void) const {return mUseDefaultKeyMap;}

      virtual void OnMessage(MessageData *data);

      ///increment to the next report mode
      void IncReportMode(void);

      ///move up a line
      inline void MoveCursorDown(void) { Prof_move_cursor(1); }

      ///move down a line
      inline void MoveCursorUp(void) { Prof_move_cursor(-1); }

      ///expand the currently selected line
      inline void Select(void) { Prof_select(); }

      /**
      * Called when a key is pressed.
      *
      * @param keyboard the source of the event
      * @param key the key pressed
      * @param character the corresponding character
      */
      virtual void KeyPressed(Keyboard* keyboard, 
                              Producer::KeyboardKey key,
                              Producer::KeyCharacter character);



   private:
      osg::ref_ptr<osgText::Text> mTextTitle[NUM_TITLE];
      osg::ref_ptr<osgText::Text> mTextHeader;
      osg::ref_ptr<osgText::Text> mTextRecord[MAX_NUM_PROF_RECORDS];
      osg::ref_ptr<osg::Geode> mGraph; ///<The graph chart geode

      bool mRenderEnabled; ///<are we rendering the text?
      bool mGraphRendered; ///<is the graph being rendered?
      bool mPaused; ///<is the stat gathering paused?
      float mSX; ///<The starting x screen coordinate of the rendered data
      float mSY; ///<The starting y screen coordinate of the rendered data
      Prof_Report_Mode mProfMode; ///<current profile mode
      bool mUseDefaultKeyMap; ///<are we using the default keyboard mapping?

      ///refresh the text
      void UpdateText(void); 

      ///Get the colors based on the supplied factor
      int GetColors( float factor, osg::Vec4 *textColor, osg::Vec4 *glowColor);

      ///calc the pixel size of the supplied string
      float TextWidth(const std::string str) const
      {
         return (mTextTitle[0]->getFontWidth() * str.length() );
      }


      class osgProfileGraph : public osg::Drawable
      {
      public:
         osgProfileGraph(const osgProfileGraph& drawable,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY){}

         osgProfileGraph()
         {
            this->setSupportsDisplayList(false);
            this->setUseDisplayList(false);
         }

         virtual ~osgProfileGraph() {}


         virtual Object* cloneType() const { return new osgProfileGraph(); }
         virtual Object* clone(const osg::CopyOp& copyop) const { return new osgProfileGraph(*this,copyop); }        

         virtual void drawImplementation(osg::State& state) const
         { 
            Prof_draw_graph_gl(20.f, 50.f, 5.f, 8.f);
         }
      };//osgProfileGraph
   };//ProfileMgr
}
#endif
