/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#ifndef DELTA_STATS
#define DELTA_STATS




#include <osgUtil/Statistics>
#include <osg/Timer>
#include <osgUtil/SceneView>
#include <osgText/Text>
#include <osg/Projection>
#include <osg/Switch>

#include "dtCore/export.h"

namespace dtCore
{
   ///Used to gather and display statistical information regarding the display

   /** This class is used internally by the Scene to gather and report 
     * statistics on the frame rate, primitive totals, etc.
     */
   class DT_EXPORT Stats
   {
   public:
      Stats(osgUtil::SceneView *sv);
      ~Stats() {};

      void Init(osgUtil::RenderStage* stg);
      void Draw();
      void SetTime(int type);
      void SelectType(osgUtil::Statistics::statsType type);
      void SelectNextType(void);
      double FrameSeconds() { return mTimer.delta_s(mLastFrameTick,mFrameTick); }
      double FrameRate() { return 1.0/FrameSeconds(); }
      double GetTime(int type=0);
      double ClockSeconds() { return mTimer.delta_s(mInitialTick,ClockTick()); }

      enum timeType
      {
         TIME_BEFORE_APP,
         TIME_AFTER_APP,
         TIME_BEFORE_CULL,
         TIME_AFTER_CULL,
         TIME_BEFORE_DRAW,
         TIME_AFTER_DRAW,
      };

   protected:
      osg::ref_ptr<osgUtil::RenderStage> mStage;
      float mFrameRate;
      int mPrintStats;
      osg::Timer   mTimer;
      osg::Timer_t mInitialTick;
      osg::Timer_t mLastFrameTick;
      osg::Timer_t mFrameTick;
      osg::Timer_t mRegTimes[6];

      struct
      {
         float timeApp, timeCull, timeDraw, timeFrame;
         osg::Timer_t frameend;
      } times[3]; // store up to 3 frames worth of times

      // time from the current frame update and the previous one in seconds.
      // time since initClock() in seconds.
      // update the number of ticks since the last frame update.
      osg::Timer_t UpdateFrameTick();

      // initialize the clock.
      long InitClock();

      void InitTexts();
      void ShowStats();
      int WritePrims( const int ypos, osgUtil::Statistics& stats);
      void Display();

      // system tick.
      inline osg::Timer_t ClockTick() {return mTimer.tick();}
      inline osg::Timer_t FrameTick() {return mFrameTick;}

      osg::ref_ptr<osgUtil::SceneView> mSV;
      osg::ref_ptr<osgText::Text> mFrameRateCounterText;
      osg::ref_ptr<osgText::Text> mUpdateTimeText;
      osg::ref_ptr<osgText::Text> mCullTimeText;
      osg::ref_ptr<osgText::Text> mDrawTimeText;
      osg::ref_ptr<osgText::Text> mFrameRateTimeText;
      osg::ref_ptr<osgText::Text> mPrimTotalsText;
      osg::ref_ptr<osgText::Text> mPrimTypesText;
      osg::ref_ptr<osgText::Text> mPrimText;
      osg::ref_ptr<osgText::Text> mVerticesText;
      osg::ref_ptr<osgText::Text> mTrianglesText;
      osg::ref_ptr<osgText::Text> mDcText;

      osg::ref_ptr<osg::Projection> mProjection;
      osg::ref_ptr<osg::Switch> mSwitch;
   private:
      void EnableTextNodes(int statsType);
   };
}


#endif // DELTA_STATS
