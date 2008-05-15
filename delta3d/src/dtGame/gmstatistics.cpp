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
* Allen Danklefsen
*/

#include <prefix/dtgameprefix-src.h>
#include <dtGame/gmstatistics.h>
#include <dtGame/gamemanager.h>
#include <sstream>

namespace dtGame
{
   ////////////////////////////////////////////////////////////////////////////////
   /*                            Statistics Information                          */
   ////////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   GMStatistics::GMStatistics() :
        mStatsLastFragmentDump(0)
      , mStatsNumProcMessages(0)
      , mStatsNumSendNetworkMessages(0)
      , mStatsNumFrames(0)
      , mStatsCumGMProcessTime(0)
      , mStatisticsInterval(0)
      , mPrintFileToConsole(false)
      , mDoStatsOnTheComponents(false)
      , mDoStatsOnTheActors(false)
   {

   }

   //////////////////////////////////////////////////////////////////////////////
   void GMStatistics::UpdateDebugStats(const dtCore::UniqueId &uniqueIDToFind,
                                      const std::string& nameOfObject, float elapsedTime, bool isComponent, bool ticklocal)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<LogDebugInformation> >::iterator itor =
         mDebugLoggerInformation.find(uniqueIDToFind);
      if (itor != mDebugLoggerInformation.end())
      {
         LogDebugInformation& debugInfo = *itor->second;
         if(ticklocal)
         {
            debugInfo.mTickLocalTime +=elapsedTime;
            debugInfo.mTimesThrough += 1;
            debugInfo.mTotalTime += elapsedTime;
         }
         else
         {
            debugInfo.mTotalTime += elapsedTime;
         }
      }
      else
      {
         dtCore::RefPtr<LogDebugInformation> toPushDebugInfo = new LogDebugInformation(nameOfObject, uniqueIDToFind, isComponent);
         toPushDebugInfo->mTotalTime = elapsedTime;
         mDebugLoggerInformation.insert(std::make_pair(uniqueIDToFind, toPushDebugInfo));
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
      if(logLastTime)
      {
         // temporarily 100, fill in to correct time if needbe.
         DebugStatisticsPrintOut(100, ourGm);
      }

      if(clearList)
      {
         mDebugLoggerInformation.clear();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GMStatistics::DebugStatisticsPrintOut(const float realTimeElapsed, const GameManager& ourGm)
   {
      // Lots of divides and unnecessary stuff in here to do if we don't need to
      // Just return. 
      if(!mDoStatsOnTheComponents && !mDoStatsOnTheActors)
         return;

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
         " Ntwrk], #Actors[" << ourGm.mActorProxyMap.size() << "/" << ourGm.mGameActorProxyMap.size() << 
         " Game/" << ourGm.mActorProxyMap.size() << "]" << std::endl;

      // reset values for next fragment
      mStatsNumFrames         = 0;
      mStatsNumProcMessages   = 0;
      mStatsCumGMProcessTime  = 0;
      mStatsNumSendNetworkMessages = 0;

      // Build up all the information in the stream
      std::map<dtCore::UniqueId, dtCore::RefPtr<LogDebugInformation> >::iterator iter = mDebugLoggerInformation.begin();
      if(mDoStatsOnTheComponents)
      {
         ss << "*************** STARTING LOGGING OF TIME IN COMPONENTS *****************" << std::endl;
         for(; iter != mDebugLoggerInformation.end(); ++iter)
         {
            LogDebugInformation& debugInfo = *iter->second;
            if(debugInfo.mIsComponent)
            {
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
         ss << "*************** ENDING LOGGING OF TIME IN COMPONENTS *****************" << std::endl;
      }

      // ACTORs
      if(mDoStatsOnTheActors)
      {
         int numIgnored = 0;
         float ignoredCumulativeTime = 0.0;

         ss << "********** STARTING LOGGING OF TIME IN ACTORS -- if (> 0.1%) ************" << std::endl;
         for(iter = mDebugLoggerInformation.begin(); iter != mDebugLoggerInformation.end(); ++iter)
         {
            LogDebugInformation& debugInfo = *iter->second;
            if(!debugInfo.mIsComponent)
            {
               float percentTime = ComputeStatsPercent(truncRealTime, debugInfo.mTotalTime);
               float truncTotalTime = ((int)(debugInfo.mTotalTime * 10000)) / 10000.0; // force data truncation to 4 places
               if (percentTime > 0.1)
               {
                  ss << "* Time[" << percentTime << "% / " << truncTotalTime << " Total], Name[" << 
                     debugInfo.mNameOfLogInfo.c_str() << "]" << 
                     ", UniqueId[" << debugInfo.mUniqueID.ToString().c_str() << "]" << std::endl;
               }
               else if (debugInfo.mTotalTime == 0.0)
                  debugDeleteList.push_back(iter->second);
               else 
               {
                  numIgnored ++;
                  ignoredCumulativeTime += truncTotalTime;
               }
               debugInfo.mTotalTime = 0;
               debugInfo.mTimesThrough = 0;
               debugInfo.mTickLocalTime = 0;
               cumulativeTime += truncTotalTime;
               numActors += 1;
            }
         }
         // ignored actors -- too much data with 500+ actors for one actor per line
         if (numIgnored > 0)
         {
            float percentTime = ComputeStatsPercent(truncRealTime, ignoredCumulativeTime);
            ss << "*** Ignored [" << numIgnored << "] actors for [" << ignoredCumulativeTime << 
               ", " << percentTime << "%]." << std::endl;
         }
         ss << "************ ENDING LOGGING OF TIME IN ACTORS *********************" << std::endl;
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
      if(mPrintFileToConsole)
      {
         if(ourGm.mLogger != NULL)
            ourGm.mLogger->LogMessage(__FUNCTION__, __LINE__, ss.str(), dtUtil::Log::LOG_ALWAYS);
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
            mDebugLoggerInformation.erase(deleteIter);
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
   void GMStatistics::FragmentTimeDump(dtCore::Timer_t& frameTickStart, const GameManager& ourGm)
   {
      // STATISTICS - If fragment time occured, dump out the GM statistics
      if (mStatisticsInterval > 0)
      {
         mStatsNumFrames++;

         // Compute GM process time.  Note - You can't use GetRealClockTime() for GM work time
         // because mClock on system is only updated at the start of the whole tick.
         dtCore::Timer_t frameTickStop = mStatsTickClock.Tick();
         double fragmentDelta = 
            mStatsTickClock.DeltaMicro(mStatsLastFragmentDump, frameTickStop);

         mStatsCumGMProcessTime += 
            dtCore::Timer_t(mStatsTickClock.DeltaMicro(frameTickStart, frameTickStop));

         if (fragmentDelta < 0) // handle wierd case of wrap around (just to be safe)
            mStatsLastFragmentDump = frameTickStop;

         else if (fragmentDelta >= (mStatisticsInterval * 1000000))
         {
            dtCore::Timer_t realTimeElapsed = 
               dtCore::Timer_t(mStatsTickClock.DeltaMicro(mStatsLastFragmentDump, frameTickStop));

            DebugStatisticsPrintOut(realTimeElapsed, ourGm);
            mStatsLastFragmentDump  = frameTickStop;
         }
      }
   }
}
