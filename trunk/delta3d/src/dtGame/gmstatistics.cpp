/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008, BMH Associates, Inc.
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
 * Curtiss Murphy, Allen Danklefsen
 */

#include <prefix/dtgameprefix.h>
#include <dtGame/gmstatistics.h>
#include <dtGame/gamemanager.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <osg/Stats>
#include <sstream>
#include <cstdio>

namespace dtGame
{
   ////////////////////////////////////////////////////////////////////////////////
   /*                            Statistics Information                          */
   ////////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   GMStatistics::GMStatistics() 
      : mStatsLastFragmentDump(0)
      , mStatsNumProcMessages(0)
      , mStatsNumSendNetworkMessages(0)
      , mStatsNumFrames(0)
      , mStatsCumGMProcessTime(0)
      , mStatsCurFrameActorTotal(0.0f)
      , mStatsCurFrameCompTotal(0.0f)
      , mStatsNumActorsProcessed(0)
      , mStatsNumCompsProcessed(0)
      , mStatisticsInterval(0)
      , mPrintFileToConsole(false)
      , mDoStatsOnTheComponents(false)
      , mDoStatsOnTheActors(false)
      , mDoStatsForDisplay(false)
   {

   }

   //////////////////////////////////////////////////////////////////////////////
   GMStatistics::~GMStatistics() {}

   //////////////////////////////////////////////////////////////////////////////
   int GMStatistics::GetStatisticsInterval() const
   {
      return mStatisticsInterval;
   }

   //////////////////////////////////////////////////////////////////////////////
   const std::string& GMStatistics::GetFilePathToPrintDebugInformation() const
   {
      static const std::string emptyString;
      if (mPrintFileToConsole)
         return emptyString;

      return mFilePathToPrintDebugInformation;
   }

   //////////////////////////////////////////////////////////////////////////////
   bool GMStatistics::ShouldWeLogActors() const
   {
      return (ShouldWeLogStatsForDisplay() || (mStatisticsInterval > 0 && mDoStatsOnTheActors));
   }

   //////////////////////////////////////////////////////////////////////////////
   bool GMStatistics::ShouldWeLogComponents() const
   {
      return (ShouldWeLogStatsForDisplay() || (mStatisticsInterval > 0 && mDoStatsOnTheComponents));
   }

   //////////////////////////////////////////////////////////////////////////////
   bool GMStatistics::ShouldWeLogStatsForDisplay() const
   {
      return mDoStatsForDisplay;
   }

   //////////////////////////////////////////////////////////////////////////////
   bool GMStatistics::ShouldWeLogToConsole() const
   {
      return (mStatisticsInterval > 0 && mPrintFileToConsole);
   }

   //////////////////////////////////////////////////////////////////////////////
   void GMStatistics::UpdateDebugStats(const dtCore::UniqueId &uniqueIDToFind,
                                      const std::string& nameOfObject, float elapsedTime, bool isComponent, bool ticklocal)
   {
      int newItemCount = 0; // Becomes 1 on first entry for each frame

      std::map<dtCore::UniqueId, dtCore::RefPtr<LogDebugInformation> >::iterator itor =
         mDebugLoggerInformation.find(uniqueIDToFind);
      if (itor != mDebugLoggerInformation.end())
      {
         LogDebugInformation& debugInfo = *itor->second;
         if (ticklocal)
         {
            debugInfo.mTickLocalTime +=elapsedTime;
            debugInfo.mTimesThrough += 1;
         }
         newItemCount = (debugInfo.mTimesThroughThisFrame == 0) ? (1) : (0); // First time this frame
         debugInfo.mTimesThroughThisFrame += 1;
         debugInfo.mTotalTime += elapsedTime;
      }
      else // First entry for this item. 
      {
         dtCore::RefPtr<LogDebugInformation> toPushDebugInfo = new LogDebugInformation(nameOfObject, uniqueIDToFind, isComponent);
         toPushDebugInfo->mTotalTime = elapsedTime;
         mDebugLoggerInformation.insert(std::make_pair(uniqueIDToFind, toPushDebugInfo));
         newItemCount = 1;
      }

      if (!isComponent)
      {
         mStatsCurFrameActorTotal += elapsedTime;
         mStatsNumActorsProcessed += newItemCount;
      }
      else
      {
         mStatsCurFrameCompTotal += elapsedTime;
         mStatsNumCompsProcessed += newItemCount;
      }

   }

   //////////////////////////////////////////////////////////////////////////////
   void GMStatistics::DebugStatisticsTurnOn(bool logComponents, bool logActors,
                                           const int statisticsInterval, bool toConsole, const std::string& path)
   {
      mDoStatsOnTheComponents    = logComponents;
      mDoStatsOnTheActors        = logActors;
      mPrintFileToConsole        = toConsole;
      mFilePathToPrintDebugInformation = path;
      mStatisticsInterval        = statisticsInterval;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GMStatistics::DebugStatisticsTurnOff(const GameManager& ourGm,
                                             bool logLastTime, bool clearList)
   {
      mDoStatsOnTheComponents    = false;
      mDoStatsOnTheActors        = false;
      mStatisticsInterval        = 0;
      mFilePathToPrintDebugInformation.clear();

      if (logLastTime)
      {
         // temporarily 100, fill in to correct time if needbe.
         DebugStatisticsPrintOut(100, ourGm);
      }

      if (clearList)
      {
         mDebugLoggerInformation.clear();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GMStatistics::DebugStatisticsPrintOut(const float realTimeElapsed,
                                              const GameManager& ourGm,
                                              dtUtil::Log* logger)
   {
      // Lots of divides and unnecessary stuff in here to do if we don't need to
      // Just return.
      if (!mDoStatsOnTheComponents && !mDoStatsOnTheActors)
      {
         return;
      }

      // real time is milliseconds. Convert to seconds (*1000) and then trunc to 4 digits
      float truncRealTime = ((int)(realTimeElapsed / 10.0)) / 100000.0;
      float truncCumGMTime = ((int)((float) mStatsCumGMProcessTime / 10.0)) / 100000.0;
      float gmPercentTime = ComputeStatsPercent(truncRealTime, truncCumGMTime);
      float cumulativeTime = 0.0;
      int numComps = 0, numActors = 0;
      std::vector<dtCore::RefPtr<LogDebugInformation> > debugDeleteList;

      std::ostringstream ss;
      ss << "==========Printing Debug information===========" <<
   #ifdef _DEBUG
         "  [DEBUG BUILD]  " << std::endl;
   #else
         "  [RELEASE BUILD]  " << std::endl;
   #endif

      float fps = ((int)((mStatsNumFrames/truncRealTime) * 10.0)) / 10.0; // force data truncation to 1 place
      ss << "GM Stats: CurSimTime[" << ourGm.GetSimulationTime() << "], TimeInGM[" << gmPercentTime <<
         "%, " << truncCumGMTime << "s], ReportTime[" << truncRealTime <<
         "s], Ticks[" << mStatsNumFrames << "], FPS[" << fps <<
         "], #Msgs[" << mStatsNumProcMessages << " Local/" << mStatsNumSendNetworkMessages <<
         " Ntwrk], #Actors[" << ourGm.GetNumAllActors() << "/ Game/" <<
         ourGm.GetNumGameActors() << "]" << std::endl;

      // reset values for next fragment
      mStatsNumFrames         = 0;
      mStatsNumProcMessages   = 0;
      mStatsCumGMProcessTime  = 0;
      mStatsNumSendNetworkMessages = 0;

      // Build up all the information in the stream
      std::map<dtCore::UniqueId, dtCore::RefPtr<LogDebugInformation> >::iterator iter = mDebugLoggerInformation.begin();
      if (mDoStatsOnTheComponents)
      {
         float compTotalTime = 0.0f;
         ss << "*************** STARTING LOGGING OF TIME IN COMPONENTS *****************" << std::endl;
         for (; iter != mDebugLoggerInformation.end(); ++iter)
         {
            LogDebugInformation& debugInfo = *iter->second;
            if (debugInfo.mIsComponent)
            {
               compTotalTime += debugInfo.mTotalTime;
               float percentTime = ComputeStatsPercent(truncRealTime, debugInfo.mTotalTime);
               float truncTotalTime = ((int)(debugInfo.mTotalTime * 10000)) / 10000.0; // force data truncation to 4 places
               ss << "* Time[" << percentTime << "% / " << truncTotalTime << " Total], Name[" <<
                  debugInfo.mNameOfLogInfo.c_str() << "]" << std::endl;
               debugInfo.mTotalTime = 0;
               debugInfo.mTimesThrough = 0;
               debugInfo.mTickLocalTime = 0;
               cumulativeTime += truncTotalTime;
               numComps += 1;

               if (debugInfo.mTotalTime == 0.0)
                  debugDeleteList.push_back(iter->second);
            }
         }
         float percentCompTotalTime = ComputeStatsPercent(truncRealTime, compTotalTime);
         float truncTotalCompTime = ((int)(compTotalTime * 10000)) / 10000.0; // force data truncation to 4 places
         ss << "********** LOGGING - END OF COMPONENT TIME [" << percentCompTotalTime << 
            "% / " << truncTotalCompTime << "s Total] ***" << std::endl;
      }

      // ACTORs
      if (mDoStatsOnTheActors)
      {
         int numIgnored = 0;
         float ignoredCumulativeTime = 0.0;
         float actorTotalTime = 0.0f;

         ss << "********** STARTING LOGGING OF TIME IN ACTORS -- if (> 0.2%) ************" << std::endl;
         for (iter = mDebugLoggerInformation.begin(); iter != mDebugLoggerInformation.end(); ++iter)
         {
            LogDebugInformation& debugInfo = *iter->second;
            if (!debugInfo.mIsComponent)
            {
               actorTotalTime += debugInfo.mTotalTime;
               float percentTime = ComputeStatsPercent(truncRealTime, debugInfo.mTotalTime);
               float truncTotalTime = ((int)(debugInfo.mTotalTime * 10000)) / 10000.0; // force data truncation to 4 places
               if (percentTime > 0.2)
               {
                  ss << "* Time[" << percentTime << "% / " << truncTotalTime << "s], Name[" <<
                     debugInfo.mNameOfLogInfo.c_str() << "]" <<
                     ", Id[" << debugInfo.mUniqueID.ToString().c_str() << "]" << std::endl;
               }
               else if (debugInfo.mTotalTime == 0.0)
               {
                  debugDeleteList.push_back(iter->second);
               }
               else
               {
                  numIgnored ++;
                  ignoredCumulativeTime += truncTotalTime;
               }
               debugInfo.mTotalTime = 0.0f;
               debugInfo.mTimesThrough = 0.0f;
               debugInfo.mTickLocalTime = 0.0f;
               cumulativeTime += truncTotalTime;
               numActors += 1;
            }
         }
         // ignored actors -- too much data with 500+ actors for one actor per line
         if (numIgnored > 0)
         {
            float percentTime = ComputeStatsPercent(truncRealTime, ignoredCumulativeTime);
            ss << "*** Ignored [" << numIgnored << "] actors for [" << percentTime <<
               "%, " << ignoredCumulativeTime << "s]." << std::endl;
         }
         float percentActorTotalTime = ComputeStatsPercent(truncRealTime, actorTotalTime);
         float truncTotalActorTime = ((int)(actorTotalTime * 10000)) / 10000.0; // force data truncation to 4 places
         ss << "********** LOGGING - END OF ACTORS TIME [" << percentActorTotalTime << 
            "% / " << truncTotalActorTime << "s Total] ***" << std::endl;
      }
      // total stats
      if (mDoStatsOnTheComponents || mDoStatsOnTheActors)
      {
         float percentTime = ComputeStatsPercent(truncRealTime, cumulativeTime);
         ss << "* Cumulative Time [" << percentTime << "%, " << cumulativeTime << "s] for ";
         if (mDoStatsOnTheActors)
            ss << "[" << numActors << " actors]";
         if (mDoStatsOnTheComponents)
            ss << "[" << numComps << " comps]";
         ss << std::endl;
      }
      ss << "* Shared String Count: " << dtUtil::RefString::GetSharedStringCount() << "\n";
      ss << "============ Ending Debug information ==============" << std::endl;

      // Do the writing
      if (mPrintFileToConsole)
      {
         if (logger != NULL)
         {
            logger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, ss.str());
         }
      }
      else // print to file
      {
         FILE* temp = fopen(mFilePathToPrintDebugInformation.c_str(), "a");
         fprintf(temp, "%s", ss.str().c_str());
         fclose(temp);
      }

      // delete items that had no time this report. Easy way to clean up old components and actors
      for (unsigned int i = 0; i < debugDeleteList.size(); ++i)
      {
         LogDebugInformation &debugInfo = *debugDeleteList[i];
         std::map<dtCore::UniqueId, dtCore::RefPtr<LogDebugInformation> >::iterator deleteIter =
            mDebugLoggerInformation.find(debugInfo.mUniqueID);
         if (deleteIter != mDebugLoggerInformation.end())
         {
            mDebugLoggerInformation.erase(deleteIter);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   float GMStatistics::ComputeStatsPercent(const float total, const float partial) const
   {
      float returnValue = 0.0;

      if (total > 0)
      {
         returnValue = 1.0 - ((total - partial) / (float)total);
         returnValue = ((int)(returnValue * 1000)) / 10.0; // force data truncation
      }

      return returnValue;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GMStatistics::FragmentTimeDump(dtCore::Timer_t& frameTickStart, const GameManager& ourGm, 
      dtUtil::Log* logger)
   {
      osg::Stats* stats = dtCore::System::GetInstance().GetStats();
      // If ANYONE is interested in the stats, we have some work to do.
      if (ShouldWeLogStatsForDisplay() || ShouldWeLogActors() || ShouldWeLogComponents())
      {
         mStatsNumFrames++;

         // Compute GM process time.  Note - You can't use GetRealClockTime() for GM work time
         // because mClock on system is only updated at the start of the whole tick.
         dtCore::Timer_t frameTickStop = mStatsTickClock.Tick();
         double fragmentDelta = mStatsTickClock.DeltaMicro(mStatsLastFragmentDump, frameTickStop);

         dtCore::Timer_t timeForThisTick = dtCore::Timer_t(mStatsTickClock.DeltaMicro(frameTickStart, frameTickStop));
         mStatsCumGMProcessTime += timeForThisTick;

         // Update the visual statistics used when stats is on - via application.SetNextStatisticsType()
         if (ShouldWeLogStatsForDisplay() && stats != NULL)
         {
            int frameNumber = stats->getLatestFrameNumber();
            float timeThisTickInMillis = timeForThisTick / 1000.0f; // timethis tick is in micros.
            float actorTimeThisTickInMillis = mStatsCurFrameActorTotal * 1000.f; // actor total is in secs.
            float compTimeThisTickInMillis = mStatsCurFrameCompTotal * 1000.f; // comp total is in secs.
            stats->setAttribute(frameNumber, "GMTotalTime", timeThisTickInMillis);
            stats->setAttribute(frameNumber, "GMActorsTime", actorTimeThisTickInMillis);
            stats->setAttribute(frameNumber, "GMComponentsTime", compTimeThisTickInMillis);
            stats->setAttribute(frameNumber, "GMTotalNumActors", ourGm.GetNumAllActors());
            stats->setAttribute(frameNumber, "GMNumActorsProcessed", mStatsNumActorsProcessed);
            stats->setAttribute(frameNumber, "GMNumCompsProcessed", mStatsNumCompsProcessed);
         }

         // If we are doing print outs or console dumps.
         if (mStatisticsInterval > 0)
         {
            // handle weird case of wrap around (just to be safe)
            if (fragmentDelta < 0)
            {
               mStatsLastFragmentDump = frameTickStop;
            }
            else if (fragmentDelta >= (mStatisticsInterval * 1000000))
            {
               dtCore::Timer_t realTimeElapsed = dtCore::Timer_t(fragmentDelta);

               DebugStatisticsPrintOut(realTimeElapsed, ourGm, logger);
               mStatsLastFragmentDump  = frameTickStop;
            }
         }

         // Reset Variables for next frame.
         mStatsCurFrameActorTotal = 0.0f;
         mStatsCurFrameCompTotal = 0.0f; 
         mStatsNumActorsProcessed = 0;
         mStatsNumCompsProcessed = 0;
         // Reset frame specific log info
         std::map<dtCore::UniqueId, dtCore::RefPtr<LogDebugInformation> >::iterator iter = mDebugLoggerInformation.begin();
         for (; iter != mDebugLoggerInformation.end(); ++iter)
         {
            LogDebugInformation& debugInfo = *iter->second;
            debugInfo.mTimesThroughThisFrame = 0;
         }
      }

      // If the user turned on visual stats, then we will start tracking stats next frame.
      // See dtCore::System.cpp and dtCore::Stats.cpp - use via application.SetNextStatisticsType()
      bool gmVisualStatsAreOn = (stats != NULL && stats->collectStats("GMTotalTime"));
      // stats are now off, but were on before and we aren't logging for any other reason
      if (!gmVisualStatsAreOn && mDoStatsForDisplay && !ShouldWeLogComponents() && !ShouldWeLogActors())  
      {
         mDebugLoggerInformation.clear();
      }
      mDoStatsForDisplay = gmVisualStatsAreOn;

   }
}
