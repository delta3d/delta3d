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

#ifndef GM_STATISTICS
#define GM_STATISTICS

#include <dtCore/timer.h>
#include <dtCore/uniqueid.h>
#include <dtCore/refptr.h>
#include <osg/Referenced>
#include <string>
#include <map>

// this is purposely not exported, should only be used by the GM
namespace dtGame
{
   class GameManager;

   class GMStatistics
   {
      friend class GameManager;

   public:
         GMStatistics();
         ~GMStatistics();


         /// Used for statistics information, should never have to call yourself.
         void UpdateDebugStats(const dtCore::UniqueId& uniqueIDToFind, const std::string& nameOfObject, float realTimeElapsed,
                  bool isComponent, bool ticklocal);

         /**
          * Gets the interval (in seconds) used for writing out GM Statistics. This
          * is usually a debug setting that can be used to see how much work the GM is doing
          * as compared to how much work your scene is doing.  If this is > 0, and the
          * appropriate log level is on, the GM will output statistics periodically
          * Default is 0.
          */
         int GetStatisticsInterval() const;

         /**
          * @return the path to the file to print the logging output.
          */
         const std::string& GetFilePathToPrintDebugInformation() const;

         /**
          * Records statistics about different components and actors.
          * Sets the interval (in seconds) used for writing out GM Statistics. This
          * is usually a debug setting that can be used to see how much work the GM is doing
          * as compared to how much work your scene is doing.  If this is > 0, and the
          * appropriate log level is on, the GM will output statistics periodically
          * @param logComponents log timing for components
          * @param logActors log timing for actors
          * @param statisticsInterval The new interval (in seconds). Make sure  > 0
          * @param toConsole true to print to console, false to print to file
          * @param path if toConsole == false, print to this file.
          */
         void DebugStatisticsTurnOn(bool logComponents, bool logActors,
                  const int statisticsInterval, bool toConsole = true,
                  const std::string& path = "gamemanagerDebugInfo.txt");

         /// Turn off statistics information - params to log before stopping, and if user wants to clear history
         void DebugStatisticsTurnOff(const GameManager& ourGm, bool logLastTime = false, bool clearList = false);

         /// print out the information from member vars
         void DebugStatisticsPrintOut(const float gmPercentTime, const GameManager& ourGm);

         /**
          * Internal timer statistics calculation.  Computes what percent the partial time
          * is of the total time. Basically (1.0 - ((total - partial) / total)) * 100.
          * Result is truncated to something like: 98.5, 42.3, ...
          * @param total The total value used to determine the percentage
          * @param partial The partial amount that we are using for the percentage
          */
         float ComputeStatsPercent(const float total, const float partial) const;

         /// GM calls this for checking to see to do stats
         bool ShouldWeLogActors() const;

         /// GM calls this for checking to see to do stats
         bool ShouldWeLogComponents() const;

         bool ShouldWeLogToConsole() const;

         ///If fragment time occured, dump out the GM statistics
         void FragmentTimeDump(dtCore::Timer_t& frameTickStart, const GameManager& ourGm);

      private:

         class LogDebugInformation : public osg::Referenced
         {
            public:

               LogDebugInformation(const std::string& name, const dtCore::UniqueId &uniqueID, bool isComponent)
                  : mTotalTime(0.0f)
                  , mTickLocalTime(0.0f)
                  , mTimesThrough(1)
                  , mNameOfLogInfo(name)
                  , mUniqueID(uniqueID)
                  , mIsComponent(isComponent)
               {
               }

               float             mTotalTime;
               float             mTickLocalTime;
               unsigned int      mTimesThrough;
               std::string       mNameOfLogInfo;
               dtCore::UniqueId  mUniqueID;
               bool              mIsComponent;

            protected:

               virtual ~LogDebugInformation() { }
         };

         ////////////////////////////////////////////////
         // statistics data
         dtCore::Timer        mStatsTickClock;
         dtCore::Timer_t      mStatsLastFragmentDump;
         long                 mStatsNumProcMessages;
         long                 mStatsNumSendNetworkMessages;
         long                 mStatsNumFrames;
         dtCore::Timer_t      mStatsCumGMProcessTime;
         float                mStatsCurFrameActorTotal; 
         float                mStatsCurFrameCompTotal; 
         int                  mStatisticsInterval;                                  ///< how often we print the information out.
         std::string          mFilePathToPrintDebugInformation;                     ///< where the file is located at that we print out to
         bool                 mPrintFileToConsole;                                  ///< if the information goes to console or file
         bool                 mDoStatsOnTheComponents;                              ///< do we fill in the information for the components.
         bool                 mDoStatsOnTheActors;                                  ///< Do we fill in information for the actors

         std::map<dtCore::UniqueId, dtCore::RefPtr<LogDebugInformation> > mDebugLoggerInformation; ///< hold onto all the information.
         ////////////////////////////////////////////////
   };
}

#endif
