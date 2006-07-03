/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* @author Curtiss Murphy
*/

#include "testaarhud.h"

#include <dtCore/object.h>
#include <dtCore/globals.h>
#include <dtCore/flymotionmodel.h>
#include <dtUtil/exception.h>

#include <dtGame/binarylogstream.h>
#include <dtGame/logtag.h>
#include <dtGame/logkeyframe.h>
#include <dtGame/logstatus.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/loggermessages.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/clientgamemanager.h>
#include <dtGame/logcontroller.h>
#include <dtGame/logstatus.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/taskcomponent.h>
#include <dtActors/taskactor.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/transformableactorproxy.h>

#include <ctime>

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(ARRHUDException);
ARRHUDException ARRHUDException::INIT_ERROR("INIT_ERROR");
ARRHUDException ARRHUDException::RUNTIME_ERROR("RUNTIME_ERROR");

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(HUDState);
HUDState HUDState::MINIMAL("MINIMAL");
HUDState HUDState::MEDIUM("MEDIUM");
HUDState HUDState::MAXIMUM("MAXIMUM");
HUDState HUDState::NONE("NONE");
HUDState HUDState::HELP("HELP");


//////////////////////////////////////////////////////////////////////////
TestAARHUD::TestAARHUD(dtCore::DeltaWin *win, dtGame::ClientGameManager &clientGM,
                       dtGame::LogController &logController, dtGame::TaskComponent &taskComponent,
                       dtGame::ServerLoggerComponent &serverLogger)
 : dtGame::GMComponent("TestAARHUD"),
   mHUDState(&HUDState::MINIMAL),
   mLastHUDStateBeforeHelp(&HUDState::MINIMAL),
   mClientGM(&clientGM),
   mLogController(&logController),
   mTaskComponent(&taskComponent),
   mServerLoggerComponent(&serverLogger),
   mWin(win),
   mMainWindow(NULL),
   mGUI(NULL),
   mHUDOverlay(NULL),
   mHelpOverlay(NULL),
   mStateText(NULL),
   mSimTimeText(NULL),
   mSpeedFactorText(NULL),
   mNumMessagesText(NULL),
   mRecordDurationText(NULL),
   mNumTagsText(NULL),
   mLastTagText(NULL),
   mNumFramesText(NULL),
   mLastFrameText(NULL),
   mCurLogText(NULL),
   mCurMapText(NULL),
   mFirstTipText(NULL),
   mSecondTipText(NULL),
   mHelpTipText(NULL),
   mHelp1Text(NULL),
   mHelp2Text(NULL),
   mHelp3Text(NULL),
   mHelp4Text(NULL),
   mHelp5Text(NULL),
   mHelp6Text(NULL),
   mHelp7Text(NULL),
   mHelp8Text(NULL),
   mHelp9Text(NULL),
   mHelp10Text(NULL),
   mHelp11Text(NULL),
   mHelp12Text(NULL),
   mHelp13Text(NULL),
   mHelp14Text(NULL),
   mHelp15Text(NULL),
   mHelp16Text(NULL),
   mHelp17Text(NULL),
   mHelp18Text(NULL),
   mTasksHeaderText(NULL),
   mTaskTextList(),
   mRightTextXOffset(180.0f),
   mTextYTopOffset(10.0f),
   mTextYSeparation(2.0f),
   mTextHeight(20.0f)
{
   SetupGUI(win);
}

//////////////////////////////////////////////////////////////////////////
TestAARHUD::~TestAARHUD()
{
   mGUI->ShutdownGUI();
}

//////////////////////////////////////////////////////////////////////////
void TestAARHUD::ProcessMessage(const dtGame::Message& message)
{
   if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
   {
      TickHUD();
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAARHUD::SetupGUI(dtCore::DeltaWin *win)
{
   char clin[HUDCONTROLMAXTEXTSIZE]; // general buffer to print
   float curYPos;
   float helpTextWidth = 400;
   float taskTextWidth = 300;

   try {
      // Initialize CEGUI
      mGUI = new dtGUI::CEUIDrawable(win);

      std::string scheme = "gui/schemes/WindowsLookSkin.scheme";
      std::string path = osgDB::findDataFile(scheme);
      if(path.empty())
      {
         EXCEPT(ARRHUDException::INIT_ERROR,"Failed to find the scheme file.");
      }

      std::string dir = path.substr(0, path.length() - (scheme.length() - 3));
      dtUtil::FileUtils::GetInstance().PushDirectory(dir);
      CEGUI::SchemeManager::getSingleton().loadScheme(path);
      dtUtil::FileUtils::GetInstance().PopDirectory();

      CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();
      CEGUI::System::getSingleton().setDefaultFont("Tahoma-12");
      mMainWindow = wm->createWindow("DefaultGUISheet", "root");
      CEGUI::System::getSingleton().setGUISheet(mMainWindow);

      // MEDIUM FIELDS - on in Medium or max

      mHUDOverlay = static_cast<CEGUI::StaticImage*>(wm->createWindow
         ("WindowsLook/StaticImage", "medium_overlay"));
      mMainWindow->addChildWindow(mHUDOverlay);
      mHUDOverlay->setPosition(CEGUI::Point(0.0f, 0.0f));
      mHUDOverlay->setSize(CEGUI::Size(1.0f, 1.0f));
      mHUDOverlay->setFrameEnabled(false);
      mHUDOverlay->setBackgroundEnabled(false);

      // Main State - idle/playback/record
      mStateText = CreateText("State Text", mHUDOverlay, "", 10.0f, 20.0f, 120.0f, mTextHeight + 5);
      mStateText->setTextColours(CEGUI::colour(1.0, 0.1, 0.1));
      mStateText->setFont("Tahoma-12");

      // Core sim info
      mSimTimeText = CreateText("Sim Time", mHUDOverlay, "Sim Time",
         0.0f, 0.0f, mRightTextXOffset - 2, mTextHeight);
      mSpeedFactorText = CreateText("Speed Factor", mHUDOverlay, "Speed",
         0.0f, 0.0f, mRightTextXOffset - 2, mTextHeight);

      // Detailed record/playback info
      mRecordDurationText = CreateText(std::string("Duration"), mHUDOverlay, std::string("Duration"),
         0, 0, mRightTextXOffset - 2, mTextHeight);
      mNumMessagesText = CreateText(std::string("Num Msgs"), mHUDOverlay, std::string("Num Msgs"),
         0, 0, mRightTextXOffset - 2, mTextHeight);
      mNumTagsText = CreateText(std::string("Num Tags"), mHUDOverlay, std::string("Num Tags"),
         0, 0, mRightTextXOffset - 2, mTextHeight);
      mLastTagText = CreateText(std::string("Last Tag"), mHUDOverlay, std::string("LastTag:"),
         0, 0, mRightTextXOffset - 2, mTextHeight);
      mNumFramesText = CreateText(std::string("Num Frames"), mHUDOverlay, std::string("Num Frames"),
         0, 0, mRightTextXOffset - 2, mTextHeight);
      mLastFrameText = CreateText(std::string("Last Frame"), mHUDOverlay, std::string("Last Frame"),
         0, 0, mRightTextXOffset - 2, mTextHeight);
      mCurLogText = CreateText(std::string("Cur Log"), mHUDOverlay, std::string("Cur Log"),
         0, 0, mRightTextXOffset - 2, mTextHeight);
      mCurMapText = CreateText(std::string("Cur Map"), mHUDOverlay, std::string("Cur Map"),
         0, 0, mRightTextXOffset - 2, mTextHeight);

      // Core Tips at top of screen (HUD Toggle and Help)
      mFirstTipText = CreateText(std::string("First Tip"), mHUDOverlay, std::string("(F2 for MED HUD)"),
         0, mTextYTopOffset, 160, mTextHeight + 2);
      mFirstTipText->setHorizontalAlignment(CEGUI::HA_CENTRE);
      mSecondTipText = CreateText(std::string("Second Tip"), mHUDOverlay, std::string("  (F1 for Help)"),
         0, mTextYTopOffset + mTextHeight + 3, 160, mTextHeight + 2);
      mSecondTipText->setHorizontalAlignment(CEGUI::HA_CENTRE);


      // TASK FIELDS

      // task header
      curYPos = 70;
      mTasksHeaderText = CreateText(std::string("Task Header"), mHUDOverlay, std::string("Tasks:"),
         4, curYPos, taskTextWidth - 2, mTextHeight + 2);
      mTasksHeaderText->setFont("Tahoma-12");
      curYPos += 2;

      // 11 placeholders for tasks
      for (int i = 0; i < 11; i++)
      {
         snprintf(clin, HUDCONTROLMAXTEXTSIZE, "Task %i", i);
         curYPos += mTextHeight + 2;
         mTaskTextList.push_back(CreateText(std::string(clin), mHUDOverlay, std::string(clin),
            12, curYPos, taskTextWidth - 2, mTextHeight + 2));
      }

      // HELP FIELDS

      mHelpOverlay = static_cast<CEGUI::StaticImage*>(wm->createWindow
         ("WindowsLook/StaticImage", "Help Overlay"));
      mMainWindow->addChildWindow(mHelpOverlay);
      mHelpOverlay->setPosition(CEGUI::Point(0.0f, 0.0f));
      mHelpOverlay->setSize(CEGUI::Size(1.0f, 1.0f));
      mHelpOverlay->setFrameEnabled(false);
      mHelpOverlay->setBackgroundEnabled(false);
      mHelpOverlay->hide();

      // help tip
      mHelpTipText = CreateText(std::string("Help Tip"), mHelpOverlay, std::string("(F2 to Toggle HUD)"),
         0, mTextYTopOffset, 160, mTextHeight + 2);
      mHelpTipText->setHorizontalAlignment(CEGUI::HA_CENTRE);

      // HELP - Speed Settings
      curYPos = mTextYTopOffset;
      mHelp1Text = CreateText(std::string("Help1"), mHelpOverlay, std::string("[-] Slower (min 0.1X)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp2Text = CreateText(std::string("Help2"), mHelpOverlay, std::string("[+] Faster (max 10.0X)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp3Text = CreateText(std::string("Help3"), mHelpOverlay, std::string("[0] Normal Speed (1.0X)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp7Text = CreateText(std::string("Help7"), mHelpOverlay, std::string("[P] Pause/Unpause"),
         5, curYPos, helpTextWidth, mTextHeight + 2);

      // HELP - Camera Movement
      curYPos += mTextHeight * 2;
      mHelp17Text = CreateText(std::string("Help17"), mHelpOverlay, std::string("[Mouse] Turn Camera"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp13Text = CreateText(std::string("Help13"), mHelpOverlay, std::string("[A & D] Move Camera Left & Right"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp14Text = CreateText(std::string("Help14"), mHelpOverlay, std::string("[W & S] Move Camera Forward and Back"),
         5, curYPos, helpTextWidth, mTextHeight + 2);

      // HELP - Player Movement
      curYPos += mTextHeight * 2;
      mHelp15Text = CreateText(std::string("Help15"), mHelpOverlay, std::string("[J & L] Turn Player Left & Right"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp16Text = CreateText(std::string("Help16"), mHelpOverlay, std::string("[I & K] Move Player Forward and Back"),
         5, curYPos, helpTextWidth, mTextHeight + 2);

      // HELP - Idle, Record, and playback
      curYPos += mTextHeight * 2;
      mHelp4Text = CreateText(std::string("Help4"), mHelpOverlay, std::string("[1] Goto IDLE Mode (Ends record & playback)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp5Text = CreateText(std::string("Help5"), mHelpOverlay, std::string("[2] Begin RECORD Mode (From Idle ONLY)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp6Text = CreateText(std::string("Help6"), mHelpOverlay, std::string("[3] Begin PLAYBACK Mode (From Idle ONLY)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp18Text = CreateText(std::string("Help18"), mHelpOverlay, std::string("[< & >] Prev & Next Keyframe (From Playback ONLY)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);

      // HELP - Misc
      curYPos += mTextHeight * 2;
      mHelp8Text = CreateText(std::string("Help8"), mHelpOverlay, std::string("[B] Place Object"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp11Text = CreateText(std::string("Help11"), mHelpOverlay, std::string("[F] Insert Keyframe"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp12Text = CreateText(std::string("Help12"), mHelpOverlay, std::string("[T] Insert Tag"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp9Text = CreateText(std::string("Help9"), mHelpOverlay, std::string("[Enter] Toggle Statistics"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp10Text = CreateText(std::string("Help10"), mHelpOverlay, std::string("[Space] Update Logger Status"),
         5, curYPos, helpTextWidth, mTextHeight + 2);

      // finally, update our state - disable/hide to make it match current state
      UpdateState();

      // Note - don't forget to add the cegui drawable to the scene after this method, or you get nothing.
   }
   catch(CEGUI::Exception &e)
   {
      std::ostringstream oss;
      oss << "CEGUI while setting up AAR GUI: " << e.getMessage().c_str();
      EXCEPT(ARRHUDException::INIT_ERROR,oss.str());
   }

}

//////////////////////////////////////////////////////////////////////////
void TestAARHUD::TickHUD()
{
   int x(0), y(0), w(0), h(0);
   mWin->GetPosition(x, y, w, h);
   float curYPos;

   if (*mHUDState != HUDState::HELP)
   {
      char clin[HUDCONTROLMAXTEXTSIZE]; // general buffer to print

      // Playback State
      if (dtGame::LogStateEnumeration::LOGGER_STATE_IDLE == mLogController->GetLastKnownStatus().GetStateEnum())
         UpdateStaticText(mStateText, "IDLE", 1.0, 1.0, 1.0);
      else if (dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK == mLogController->GetLastKnownStatus().GetStateEnum())
         UpdateStaticText(mStateText, "PLAYBACK", 0.1, 1.0, 0.1);
      else // if (dtGame::LogStateEnumeration::LOGGER_STATE_RECORD == mLogController->GetLastKnownStatus().GetStateEnum())
         UpdateStaticText(mStateText, "RECORD", 1.0, 0.1, 0.1);

      // Sim Time
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "SimTime: %.2f", mClientGM->GetSimulationTime());
      curYPos = mTextYTopOffset;
      UpdateStaticText(mSimTimeText, clin, -1.0, -1.0, -1.0, w - mRightTextXOffset, curYPos);

      // speed factor
      curYPos += mTextHeight + 2;
      if (!mClientGM->IsPaused())
      {
         snprintf(clin, HUDCONTROLMAXTEXTSIZE, "Speed: %.2fX", mClientGM->GetTimeScale());
         UpdateStaticText(mSpeedFactorText, clin, 1.0, 1.0, 1.0, w - mRightTextXOffset, curYPos);
      }
      else
         UpdateStaticText(mSpeedFactorText, "Speed: *Paused*", 1.0, 0.1, 0.1, w - mRightTextXOffset, curYPos);

      UpdateMediumDetailData();
      UpdateHighDetailData((int)(w - mRightTextXOffset), curYPos);
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAARHUD::UpdateMediumDetailData()
{
   if (*mHUDState == HUDState::MEDIUM || *mHUDState == HUDState::MAXIMUM)
   {
      char clin[HUDCONTROLMAXTEXTSIZE]; // general buffer to print
      std::vector<dtCore::RefPtr<dtGame::GameActorProxy> > tasks;
      int numAdded = 0;
      int numComplete = 0;

      mTaskComponent->GetTopLevelTasks(tasks);

      // start our recursive method on each top level task
      for (int i = 0; i < (int) tasks.size(); i ++)
      {
         dtActors::TaskActorProxy *taskProxy =
            dynamic_cast<dtActors::TaskActorProxy *> (tasks[i].get());
         numAdded += RecursivelyAddTasks("", numAdded, taskProxy, numComplete);
      }

      // blank out any of our placeholder task text controls that were left over
      for (int i = numAdded; i < (int) mTaskTextList.size(); i ++)
         UpdateStaticText(mTaskTextList[i], "");

      // update our task header
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "Tasks (%i of %i):", numComplete, numAdded);
      if (numComplete < numAdded)
         UpdateStaticText(mTasksHeaderText, clin, 1.0, 1.0, 1.0);
      else
         UpdateStaticText(mTasksHeaderText, clin, 0.1, 1.0, 0.1);
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAARHUD::UpdateHighDetailData(int baseWidth, float &curYPos)
{
   if (*mHUDState == HUDState::MAXIMUM)
   {
      char clin[HUDCONTROLMAXTEXTSIZE]; // general buffer to print
      char lastTagStr[HUDCONTROLMAXTEXTSIZE];
      char numTagsStr[HUDCONTROLMAXTEXTSIZE];
      char lastFrameStr[HUDCONTROLMAXTEXTSIZE];
      char numFramesStr[HUDCONTROLMAXTEXTSIZE];

      // Determine num tags, num frames, etc...
      if (dtGame::LogStateEnumeration::LOGGER_STATE_IDLE == mLogController->GetLastKnownStatus().GetStateEnum())
      {
         snprintf(numTagsStr, HUDCONTROLMAXTEXTSIZE, "Num Tags: NA");
         snprintf(lastTagStr, HUDCONTROLMAXTEXTSIZE, "  (Last: NA)");
         snprintf(numFramesStr, HUDCONTROLMAXTEXTSIZE, "Num Frames: NA");
         snprintf(lastFrameStr, HUDCONTROLMAXTEXTSIZE, "  (Last: NA)");
      }
      else // compute data needed for both LOGGER_STATE_PLAYBACK or LOGGER_STATE_RECORD
      {
         // TAGS - num tags and last tag
         const std::vector<dtGame::LogTag> tags = mLogController->GetLastKnownTagList();
         snprintf(numTagsStr, HUDCONTROLMAXTEXTSIZE, "Num Tags: %u", unsigned(tags.size()));
         if (tags.size() > 0)
         {
            snprintf(lastTagStr, HUDCONTROLMAXTEXTSIZE, "  (Last: None)");
            for (int i = 0; i < (int) tags.size(); i ++)
            {
               const dtGame::LogTag tag = tags[i];
               //double tagTime = tags[i].GetSimTimeStamp();
               //double simTime = mClientGM->GetSimulationTime();

               if (tags[i].GetSimTimeStamp() <= mClientGM->GetSimulationTime())
               {
                  std::string temp = (tags[i]).GetName();
                  snprintf(lastTagStr, HUDCONTROLMAXTEXTSIZE, " (%s)", temp.c_str());//(tags[tags.size()-1]).GetName());
               }
            }
         }
         else
            snprintf(lastTagStr, HUDCONTROLMAXTEXTSIZE, " (----)");

         // FRAMES - num frames and last frame
         const std::vector<dtGame::LogKeyframe> frames = mLogController->GetLastKnownKeyframeList();
         snprintf(numFramesStr, HUDCONTROLMAXTEXTSIZE, "Num Frames: %u", unsigned(frames.size()));
         if (frames.size() > 0)
         {
            snprintf(lastFrameStr, HUDCONTROLMAXTEXTSIZE, "  (Last: None)");
            for (int i = 0; i < (int) frames.size(); i ++)
            {
               if (frames[i].GetSimTimeStamp() <= mClientGM->GetSimulationTime())
               {
                  std::string temp = (frames[i]).GetName();
                  snprintf(lastFrameStr, HUDCONTROLMAXTEXTSIZE, " (%s)", temp.c_str());
               }
            }
         }
         else
            snprintf(lastFrameStr, HUDCONTROLMAXTEXTSIZE, " (----)");
      }

      // Num Messages
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "Num Msgs: %lu", mLogController->GetLastKnownStatus().GetNumMessages());
      curYPos += (mTextHeight + 2) * 2;
      UpdateStaticText(mNumMessagesText, clin, -1.0, -1.0, -1.0, baseWidth, curYPos);

      // Record Duration
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "Duration: %.2f", mLogController->GetLastKnownStatus().GetCurrentRecordDuration());
      curYPos += mTextHeight + 2;
      UpdateStaticText(mRecordDurationText, clin, -1.0, -1.0, -1.0, baseWidth, curYPos);

      // Number of Tags
      curYPos += mTextHeight + 2;
      UpdateStaticText(mNumTagsText, numTagsStr, -1.0, -1.0, -1.0, baseWidth, curYPos);

      // Last Tag
      curYPos += mTextHeight + 2;
      UpdateStaticText(mLastTagText, lastTagStr, -1.0, -1.0, -1.0, baseWidth, curYPos);

      // Num Frames
      curYPos += mTextHeight + 2;
      UpdateStaticText(mNumFramesText, numFramesStr, -1.0, -1.0, -1.0, baseWidth, curYPos);

      // Num Frames
      curYPos += mTextHeight + 2;
      UpdateStaticText(mLastFrameText, lastFrameStr, -1.0, -1.0, -1.0, baseWidth, curYPos);

      // Current Log
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "LogFile: %s", mLogController->GetLastKnownStatus().GetLogFile().c_str());
      curYPos += mTextHeight + 2;
      UpdateStaticText(mCurLogText, clin, -1.0, -1.0, -1.0, baseWidth, curYPos);

      // Current Map
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "CurMap: %s", mLogController->GetLastKnownStatus().GetActiveMap().c_str());
      curYPos += mTextHeight + 2;
      UpdateStaticText(mCurMapText, clin, -1.0, -1.0, -1.0, baseWidth, curYPos);
   }
}

//////////////////////////////////////////////////////////////////////////
int TestAARHUD::RecursivelyAddTasks(const std::string &indent, int curIndex,
                                    const dtActors::TaskActorProxy *taskProxy, int &numCompleted)
{
   char clin[HUDCONTROLMAXTEXTSIZE];
   int totalNumAdded = 0;

   if (curIndex < (int) mTaskTextList.size())
   {
      // update the text for this task
      const dtActors::TaskActor *task = dynamic_cast<const dtActors::TaskActor *> (&(taskProxy->GetGameActor()));
      if (task->IsComplete())
      {
         numCompleted ++;

         snprintf(clin, HUDCONTROLMAXTEXTSIZE, "%s %s - %s - %.2f", indent.c_str(), task->GetName().c_str(),
            "Y", task->GetScore());
         UpdateStaticText(mTaskTextList[curIndex + totalNumAdded], clin, 0.0, 1.0, 0.0);
      }
      else
      {
         snprintf(clin, HUDCONTROLMAXTEXTSIZE, "%s %s - %s - %.2f", indent.c_str(), task->GetName().c_str(),
            "N", task->GetScore());
         UpdateStaticText(mTaskTextList[curIndex + totalNumAdded], clin, 1.0, 1.0, 1.0);
      }

      totalNumAdded += 1;

      // recurse for each child
      const std::vector<dtCore::RefPtr<dtActors::TaskActorProxy> > &children = taskProxy->GetAllSubTaskProxies();
      if (!children.empty())
      {
         for (int i = 0; i < (int) children.size(); i ++)
         {
            const dtActors::TaskActorProxy *childProxy = dynamic_cast<const dtActors::TaskActorProxy *> (children[i].get());
            totalNumAdded += RecursivelyAddTasks(indent + "     ", curIndex + totalNumAdded, childProxy, numCompleted);
         }
      }
   }

   return totalNumAdded;
}

//////////////////////////////////////////////////////////////////////////
void TestAARHUD::UpdateStaticText(CEGUI::StaticText *textControl, char *newText,
                                  float red, float blue, float green, float x, float y)
{
   if (textControl != NULL)
   {
      // text and color
      if (newText != NULL && textControl->getText() != std::string(newText))
      {
         textControl->setText(newText);
         if (red >= 0.00 && blue >= 0.0 && green >= 0.0)
            textControl->setTextColours(CEGUI::colour(red, blue, green));
      }
      // position
      if (x > 0.0 && y > 0.0)
      {
         CEGUI::Point position = textControl->getPosition();
         CEGUI::Point newPos(x, y);
         if (position != newPos)
            textControl->setPosition(newPos);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
HUDState & TestAARHUD::CycleToNextHUDState()
{
   if (*mHUDState == HUDState::MINIMAL) // MINIMAL - go to MEDIUM
   {
      mHUDState = &HUDState::MEDIUM;
      UpdateStaticText(mFirstTipText, "(F2 for MAX HUD)");
      UpdateStaticText(mHelpTipText, "(F2 for MED HUD)");
      mLastHUDStateBeforeHelp = mHUDState;
   }
   else if (*mHUDState == HUDState::MEDIUM) // MEDIUM - go to MAXIMUM
   {
      mHUDState = &HUDState::MAXIMUM;
      UpdateStaticText(mFirstTipText, "(F2 for NO HUD)");
      UpdateStaticText(mHelpTipText, "(F2 for MAX HUD)");
      mLastHUDStateBeforeHelp = mHUDState;
   }
   else if (*mHUDState == HUDState::MAXIMUM) // MAXIMUM, go to NONE
   {
      mHUDState = &HUDState::NONE;
      UpdateStaticText(mFirstTipText, "(F2 for MIN HUD)");
      UpdateStaticText(mHelpTipText, "(F2 for NO HUD)");
      mLastHUDStateBeforeHelp = mHUDState;
   }
   else if (*mHUDState == HUDState::NONE) // NONE - go to MINIMUM
   {
      mHUDState = &HUDState::MINIMAL;
      UpdateStaticText(mFirstTipText, "(F2 for MED HUD)");
      UpdateStaticText(mHelpTipText, "(F2 for MIN HUD)");
      mLastHUDStateBeforeHelp = mHUDState;
   }
   else // HELP - go to last state before Help
   {
      mHUDState = mLastHUDStateBeforeHelp;
    }

   // we've changed our state, so reset our hide/show status
   UpdateState();

   return *mHUDState;
}

//////////////////////////////////////////////////////////////////////////
void TestAARHUD::UpdateState()
{
   if (*mHUDState == HUDState::HELP)
   {
      mHUDOverlay->hide();
      mHelpOverlay->show();
   }
   else {
      mHUDOverlay->show();
      mHelpOverlay->hide();

      if (*mHUDState == HUDState::MINIMAL)
      {
         mStateText->show();
         mSimTimeText->show();
         mSpeedFactorText->show();
         mSecondTipText->show();

         mNumMessagesText->hide();
         mRecordDurationText->hide();
         mNumTagsText->hide();
         mLastTagText->hide();
         mNumFramesText->hide();
         mLastFrameText->hide();
         mCurLogText->hide();
         mCurMapText->hide();

         mTasksHeaderText->hide();
         for (unsigned int i = 0; i < mTaskTextList.size(); i ++)
            mTaskTextList[i]->hide();
      }
      else if (*mHUDState == HUDState::MEDIUM)
      {
         mStateText->show();
         mSimTimeText->show();
         mSpeedFactorText->show();
         mSecondTipText->show();

         mNumMessagesText->hide();
         mRecordDurationText->hide();
         mNumTagsText->hide();
         mLastTagText->hide();
         mNumFramesText->hide();
         mLastFrameText->hide();
         mCurLogText->hide();
         mCurMapText->hide();

         mTasksHeaderText->show();
         for (unsigned int i = 0; i < mTaskTextList.size(); i ++)
            mTaskTextList[i]->show();
      }
      else if (*mHUDState == HUDState::MAXIMUM)
      {
         mStateText->show();
         mSimTimeText->show();
         mSpeedFactorText->show();
         mSecondTipText->show();

         mNumMessagesText->show();
         mRecordDurationText->show();
         mNumTagsText->show();
         mLastTagText->show();
         mNumFramesText->show();
         mLastFrameText->show();
         mCurLogText->show();
         mCurMapText->show();

         mTasksHeaderText->show();
         for (unsigned int i = 0; i < mTaskTextList.size(); i ++)
            mTaskTextList[i]->show();
      }
      else // if (*mHUDState == HUDState::NONE)
      {
         mStateText->hide();
         mSimTimeText->hide();
         mSpeedFactorText->hide();
         mSecondTipText->hide();

         mNumMessagesText->hide();
         mRecordDurationText->hide();
         mNumTagsText->hide();
         mLastTagText->hide();
         mNumFramesText->hide();
         mLastFrameText->hide();
         mCurLogText->hide();
         mCurMapText->hide();

         mTasksHeaderText->hide();
         for (unsigned int i = 0; i < mTaskTextList.size(); i ++)
            mTaskTextList[i]->hide();
      }
   }
}

//////////////////////////////////////////////////////////////////////////
CEGUI::StaticText * TestAARHUD::CreateText(const std::string &name, CEGUI::StaticImage *parent, const std::string &text,
                                 float x, float y, float width, float height)
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();

   // create base window and set our default attribs
   CEGUI::StaticText* result = static_cast<CEGUI::StaticText*>(wm->createWindow("WindowsLook/StaticText", name));
   parent->addChildWindow(result);
   result->setMetricsMode(CEGUI::Absolute);
   result->setText(text);
   result->setPosition(CEGUI::Point(x, y));
   result->setSize(CEGUI::Size(width, height));
   result->setFrameEnabled(false);
   result->setBackgroundEnabled(false);
   result->setHorizontalAlignment(CEGUI::HA_LEFT);
   result->setVerticalAlignment(CEGUI::VA_TOP);
   result->show();

   return result;
}
