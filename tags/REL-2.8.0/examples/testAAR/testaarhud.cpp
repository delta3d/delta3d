/* -*-c++-*-
 * testAAR - testaarhud (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Curtiss Murphy
 */

#include "testaarhud.h"
#include "testaarexceptionenum.h"
#include <dtABC/baseabc.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtCore/deltawin.h>
#include <dtGame/logtag.h>
#include <dtGame/logstatus.h>
#include <dtGame/logcontroller.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/taskcomponent.h>
#include <dtGame/messagetype.h>
#include <dtActors/taskactor.h>
#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/CEGUIFont.h>


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(HUDState);
HUDState HUDState::HELP("HELP");
HUDState HUDState::MEDIUM("MEDIUM");
HUDState HUDState::MINIMAL("MINIMAL");
HUDState HUDState::MAXIMUM("MAXIMUM");
HUDState HUDState::NONE("NONE");

//////////////////////////////////////////////////////////////////////////
TestAARHUD::TestAARHUD(dtABC::BaseABC& app,
                       dtGame::LogController& logController, 
                       dtGame::TaskComponent& taskComponent,
                       dtGame::ServerLoggerComponent& serverLogger)
   : dtGame::GMComponent("TestAARHUD")
   , mHUDState(&HUDState::MINIMAL)
   , mLastHUDStateBeforeHelp(&HUDState::MINIMAL)
   , mLogController(&logController)
   , mTaskComponent(&taskComponent)
   , mServerLoggerComponent(&serverLogger)
   , mWin(app.GetWindow())
   , mMainWindow(NULL)
   , mGUI(NULL)
   , mHUDOverlay(NULL)
   , mHelpOverlay(NULL)
   , mStateText(NULL)
   , mSimTimeText(NULL)
   , mSpeedFactorText(NULL)
   , mNumMessagesText(NULL)
   , mRecordDurationText(NULL)
   , mNumTagsText(NULL)
   , mLastTagText(NULL)
   , mNumFramesText(NULL)
   , mLastFrameText(NULL)
   , mCurLogText(NULL)
   , mCurMapText(NULL)
   , mFirstTipText(NULL)
   , mSecondTipText(NULL)
   , mHelpTipText(NULL)
   , mHelp1Text(NULL)
   , mHelp2Text(NULL)
   , mHelp3Text(NULL)
   , mHelp4Text(NULL)
   , mHelp5Text(NULL)
   , mHelp6Text(NULL)
   , mHelp7Text(NULL)
   , mHelp8Text(NULL)
   , mHelp9Text(NULL)
   , mHelp10Text(NULL)
   , mHelp11Text(NULL)
   , mHelp12Text(NULL)
   , mHelp13Text(NULL)
   , mHelp14Text(NULL)
   , mHelp15Text(NULL)
   , mHelp16Text(NULL)
   , mHelp17Text(NULL)
   , mHelp18Text(NULL)
   , mHelp19Text(NULL)
   , mTasksHeaderText(NULL)
   , mTaskTextList()
   , mRightTextXOffset(180.0f)
   , mTextYTopOffset(10.0f)
   , mTextHeight(14.0f)
{
   SetupGUI(*app.GetCamera(), *app.GetKeyboard(), *app.GetMouse());
}

//////////////////////////////////////////////////////////////////////////
TestAARHUD::~TestAARHUD()
{
}

//////////////////////////////////////////////////////////////////////////
void TestAARHUD::ProcessMessage(const dtGame::Message& message)
{
   if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
   {
      TickHUD();
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      //GetGameManager()->GetScene().AddChild(GetGUIDrawable()); //TODO need to display the UI *after* the map is loaded?
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAARHUD::SetupGUI(dtCore::Camera& cam, dtCore::Keyboard& keyboard, dtCore::Mouse& mouse)
{
   char clin[HUDCONTROLMAXTEXTSIZE]; // general buffer to print
   float curYPos;
   float helpTextWidth = 400;
   float taskTextWidth = 300;

   try
   {
      // Initialize CEGUI
      mGUI = new dtGUI::GUI(&cam, &keyboard, &mouse);

      mGUI->LoadScheme("WindowsLook.scheme");

      CEGUI::System::getSingleton().getDefaultFont()->setProperty("PointSize", "14");

      mMainWindow = mGUI->GetRootSheet();

      // MEDIUM FIELDS - on in Medium or max

      mHUDOverlay = mGUI->CreateWidget(mMainWindow, "WindowsLook/StaticImage", "medium_overlay");
      mHUDOverlay->setPosition(CEGUI::UVector2(cegui_reldim(0.0f),cegui_reldim(0.0f)));
      mHUDOverlay->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim(1.0f)));
      mHUDOverlay->setProperty("FrameEnabled", "false");
      mHUDOverlay->setProperty("BackgroundEnabled", "false");

      // Main State - idle/playback/record
      mStateText = CreateText("State Text", mHUDOverlay, "", 10.0f, 20.0f, 120.0f, mTextHeight + 5);
      mStateText->setProperty("TextColours", "tl:FFFF1919 tr:FFFF1919 bl:FFFF1919 br:FFFF1919");
      //mStateText->setFont("DejaVuSans-10");

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
      //mTasksHeaderText->setFont("DejaVuSans-10");
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

      mHelpOverlay = mGUI->CreateWidget(mMainWindow, "WindowsLook/StaticImage", "Help Overlay");
      mHelpOverlay->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)));
      mHelpOverlay->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim(1.0f)));
      mHelpOverlay->setProperty("FrameEnabled", "false");
      mHelpOverlay->setProperty("BackgroundEnabled", "false");
      mHelpOverlay->hide();

      // help tip
      mHelpTipText = CreateText(std::string("Help Tip"), mHelpOverlay, std::string("(F2 to Toggle HUD)"),
         0, mTextYTopOffset, 160, mTextHeight + 2);
      mHelpTipText->setHorizontalAlignment(CEGUI::HA_CENTRE);

      // HELP - Speed Settings
      curYPos = mTextYTopOffset;
      mHelp1Text = CreateText(std::string("Help1"), mHelpOverlay, std::string("\\[-] Slower (min 0.1X)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp2Text = CreateText(std::string("Help2"), mHelpOverlay, std::string("\\[+] Faster (max 10.0X)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp3Text = CreateText(std::string("Help3"), mHelpOverlay, std::string("\\[0] Normal Speed (1.0X)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp7Text = CreateText(std::string("Help7"), mHelpOverlay, std::string("\\[P] Pause/Unpause"),
         5, curYPos, helpTextWidth, mTextHeight + 2);

      // HELP - Camera Movement
      curYPos += mTextHeight * 2;
      mHelp17Text = CreateText(std::string("Help17"), mHelpOverlay, std::string("\\[Mouse] Turn Camera"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp13Text = CreateText(std::string("Help13"), mHelpOverlay, std::string("\\[A & D] Move Camera Left & Right"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp14Text = CreateText(std::string("Help14"), mHelpOverlay, std::string("\\[W & S] Move Camera Forward and Back"),
         5, curYPos, helpTextWidth, mTextHeight + 2);

      // HELP - Player Movement
      curYPos += mTextHeight * 2;
      mHelp15Text = CreateText(std::string("Help15"), mHelpOverlay, std::string("\\[J & L] Turn Player Left & Right"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp16Text = CreateText(std::string("Help16"), mHelpOverlay, std::string("\\[I & K] Move Player Forward and Back"),
         5, curYPos, helpTextWidth, mTextHeight + 2);

      // HELP - Idle, Record, and playback
      curYPos += mTextHeight * 2;
      mHelp4Text = CreateText(std::string("Help4"), mHelpOverlay, std::string("\\[1] Goto IDLE Mode (Ends record & playback)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp5Text = CreateText(std::string("Help5"), mHelpOverlay, std::string("\\[2] Begin RECORD Mode (From Idle ONLY)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp6Text = CreateText(std::string("Help6"), mHelpOverlay, std::string("\\[3] Begin PLAYBACK Mode (From Idle ONLY)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp18Text = CreateText(std::string("Help18"), mHelpOverlay, std::string("\\[< & >] Prev & Next Keyframe (From Playback ONLY)"),
         5, curYPos, helpTextWidth, mTextHeight + 2);

      // HELP - Misc
      curYPos += mTextHeight * 2;
      mHelp8Text = CreateText(std::string("Help8"), mHelpOverlay, std::string("\\[B] Place Object"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight * 2;
      mHelp19Text = CreateText(std::string("Help19"), mHelpOverlay, std::string("\\[G] Place Ignorable Object"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp11Text = CreateText(std::string("Help11"), mHelpOverlay, std::string("\\[F] Insert Keyframe"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp12Text = CreateText(std::string("Help12"), mHelpOverlay, std::string("\\[T] Insert Tag"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp9Text = CreateText(std::string("Help9"), mHelpOverlay, std::string("\\[Enter] Toggle Statistics"),
         5, curYPos, helpTextWidth, mTextHeight + 2);
      curYPos += mTextHeight + 2;
      mHelp10Text = CreateText(std::string("Help10"), mHelpOverlay, std::string("\\[Space] Update Logger Status"),
         5, curYPos, helpTextWidth, mTextHeight + 2);

      // finally, update our state - disable/hide to make it match current state
      UpdateState();
   }
   catch (CEGUI::Exception& e)
   {
      std::ostringstream oss;
      oss << "CEGUI while setting up AAR GUI: " << e.getMessage().c_str();
      throw AARHUDInitException(oss.str(), __FILE__, __LINE__);
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
      {
         UpdateStaticText(mStateText, "IDLE", 1.0f, 1.0f, 1.0f);
      }
      else if (dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK == mLogController->GetLastKnownStatus().GetStateEnum())
      {
         UpdateStaticText(mStateText, "PLAYBACK", 0.1f, 1.0f, 0.1f);
      }
      else if (dtGame::LogStateEnumeration::LOGGER_STATE_RECORD == mLogController->GetLastKnownStatus().GetStateEnum())
      {
         UpdateStaticText(mStateText, "RECORD", 1.0f, 0.1f, 0.1f);
      }
      else
      {
         UpdateStaticText(mStateText, "UNKNOWN", 1.0f, 1.0f, 1.0f);
      }

      // Sim Time
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "SimTime: %.2f", GetGameManager()->GetSimulationTime());
      curYPos = mTextYTopOffset;
      UpdateStaticText(mSimTimeText, clin, -1.0f, -1.0f, -1.0f, w - mRightTextXOffset, curYPos);

      // speed factor
      curYPos += mTextHeight + 2;
      if (!GetGameManager()->IsPaused())
      {
         snprintf(clin, HUDCONTROLMAXTEXTSIZE, "Speed: %.2fX", GetGameManager()->GetTimeScale());
         UpdateStaticText(mSpeedFactorText, clin, 1.0f, 1.0f, 1.0f, w - mRightTextXOffset, curYPos);
      }
      else
      {
         UpdateStaticText(mSpeedFactorText, "Speed: *Paused*", 1.0f, 0.1f, 0.1f, w - mRightTextXOffset, curYPos);
      }

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
      for (unsigned int i = 0; i < tasks.size(); i ++)
      {
         dtActors::TaskActorProxy *taskProxy =
            dynamic_cast<dtActors::TaskActorProxy *> (tasks[i].get());
         numAdded += RecursivelyAddTasks("", numAdded, taskProxy, numComplete);
      }

      // blank out any of our placeholder task text controls that were left over
      for (int i = numAdded; i < (int)mTaskTextList.size(); i ++)
      {
         UpdateStaticText(mTaskTextList[i], "");
      }

      // update our task header
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "Tasks (%i of %i):", numComplete, numAdded);
      if (numComplete < numAdded)
      {
         UpdateStaticText(mTasksHeaderText, clin, 1.0f, 1.0f, 1.0f);
      }
      else
      {
         UpdateStaticText(mTasksHeaderText, clin, 0.1f, 1.0f, 0.1f);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAARHUD::UpdateHighDetailData(int baseWidth, float& curYPos)
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
            for (unsigned int i = 0; i < tags.size(); i ++)
            {
               const dtGame::LogTag tag = tags[i];
               //double tagTime = tags[i].GetSimTimeStamp();
               //double simTime = mClientGM->GetSimulationTime();

               if (tags[i].GetSimTimeStamp() <= GetGameManager()->GetSimulationTime())
               {
                  std::string temp = (tags[i]).GetName();
                  snprintf(lastTagStr, HUDCONTROLMAXTEXTSIZE, " (%s)", temp.c_str());//(tags[tags.size()-1]).GetName());
               }
            }
         }
         else
         {
            snprintf(lastTagStr, HUDCONTROLMAXTEXTSIZE, " (----)");
         }

         // FRAMES - num frames and last frame
         const std::vector<dtGame::LogKeyframe> frames = mLogController->GetLastKnownKeyframeList();
         snprintf(numFramesStr, HUDCONTROLMAXTEXTSIZE, "Num Frames: %u", unsigned(frames.size()));
         if (frames.size() > 0)
         {
            snprintf(lastFrameStr, HUDCONTROLMAXTEXTSIZE, "  (Last: None)");
            for (unsigned int i = 0; i < frames.size(); i ++)
            {
               if (frames[i].GetSimTimeStamp() <= GetGameManager()->GetSimulationTime())
               {
                  std::string temp = (frames[i]).GetName();
                  snprintf(lastFrameStr, HUDCONTROLMAXTEXTSIZE, " (%s)", temp.c_str());
               }
            }
         }
         else
         {
            snprintf(lastFrameStr, HUDCONTROLMAXTEXTSIZE, " (----)");
         }
      }

      // Num Messages
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "Num Msgs: %lu", mLogController->GetLastKnownStatus().GetNumMessages());
      curYPos += (mTextHeight + 2) * 2;
      UpdateStaticText(mNumMessagesText, clin, -1.0f, -1.0f, -1.0f, baseWidth, curYPos);

      // Record Duration
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "Duration: %.2f", mLogController->GetLastKnownStatus().GetCurrentRecordDuration());
      curYPos += mTextHeight + 2;
      UpdateStaticText(mRecordDurationText, clin, -1.0f, -1.0f, -1.0f, baseWidth, curYPos);

      // Number of Tags
      curYPos += mTextHeight + 2;
      UpdateStaticText(mNumTagsText, numTagsStr, -1.0f, -1.0f, -1.0f, baseWidth, curYPos);

      // Last Tag
      curYPos += mTextHeight + 2;
      UpdateStaticText(mLastTagText, lastTagStr, -1.0f, -1.0f, -1.0f, baseWidth, curYPos);

      // Num Frames
      curYPos += mTextHeight + 2;
      UpdateStaticText(mNumFramesText, numFramesStr, -1.0f, -1.0f, -1.0f, baseWidth, curYPos);

      // Num Frames
      curYPos += mTextHeight + 2;
      UpdateStaticText(mLastFrameText, lastFrameStr, -1.0f, -1.0f, -1.0f, baseWidth, curYPos);

      // Current Log
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "LogFile: %s", mLogController->GetLastKnownStatus().GetLogFile().c_str());
      curYPos += mTextHeight + 2;
      UpdateStaticText(mCurLogText, clin, -1.0f, -1.0f, -1.0f, baseWidth, curYPos);

      // Current Map
      snprintf(clin, HUDCONTROLMAXTEXTSIZE, "CurMap: %s", mLogController->GetLastKnownStatus().GetActiveMaps()[0].c_str());
      curYPos += mTextHeight + 2;
      UpdateStaticText(mCurMapText, clin, -1.0f, -1.0f, -1.0f, baseWidth, curYPos);
   }
}

//////////////////////////////////////////////////////////////////////////
int TestAARHUD::RecursivelyAddTasks(const std::string& indent, int curIndex,
                                    const dtActors::TaskActorProxy* taskProxy, int& numCompleted)
{
   char clin[HUDCONTROLMAXTEXTSIZE];
   int totalNumAdded = 0;

   if (curIndex < (int) mTaskTextList.size())
   {
      // update the text for this task
      const dtActors::TaskActor* task = NULL;
      taskProxy->GetDrawable(task);
      if (task->IsComplete())
      {
         numCompleted ++;

         snprintf(clin, HUDCONTROLMAXTEXTSIZE, "%s %s - %s - %.2f", indent.c_str(), task->GetName().c_str(),
            "Y", task->GetScore());
         UpdateStaticText(mTaskTextList[curIndex + totalNumAdded], clin, 0.1f, 1.0f, 0.1f);
      }
      else
      {
         snprintf(clin, HUDCONTROLMAXTEXTSIZE, "%s %s - %s - %.2f", indent.c_str(), task->GetName().c_str(),
            "N", task->GetScore());
         UpdateStaticText(mTaskTextList[curIndex + totalNumAdded], clin, 1.0f, 1.0f, 1.0f);
      }

      totalNumAdded += 1;

      // recurse for each child
      std::vector<const dtActors::TaskActorProxy*> children;
      taskProxy->GetAllSubTasks(children);
      if (!children.empty())
      {
         for (int i = 0; i < (int)children.size(); i ++)
         {
            const dtActors::TaskActorProxy* childProxy = dynamic_cast<const dtActors::TaskActorProxy *> (children[i]);
            if (childProxy != NULL)
            {
               totalNumAdded += RecursivelyAddTasks(indent + "     ", curIndex + totalNumAdded, childProxy, numCompleted);
            }
         }
      }
   }

   return totalNumAdded;
}

//////////////////////////////////////////////////////////////////////////
void TestAARHUD::UpdateStaticText(CEGUI::Window* textControl, const std::string& newText,
                                    float red, float green, float blue, float x, float y)
{
   if (textControl != NULL)
   {
      // text and color
      if (textControl->getText() != newText)
      {
         textControl->setText(newText);
         if (red >= 0.00 && blue >= 0.0 && green >= 0.0)
         {
            textControl->setProperty("TextColours", 
               CEGUI::PropertyHelper::colourToString(CEGUI::colour(red, green, blue)));
            // how to do it with a string.  Use "FF00FF00" or "FFFFFFFF" for examples
            //String col = PropertyHelper::colourRectToString(ColourRect(PropertyHelper::stringToColour(String(color))));
            //textControl->setProperty("TextColours", col);         }
         }
      }
      // position
      if (x > 0.0 && y > 0.0)
      {
         CEGUI::UVector2 position = textControl->getPosition();
         CEGUI::UVector2 newPos(cegui_absdim(x), cegui_absdim(y));
         if (position != newPos)
            textControl->setPosition(newPos);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
HUDState& TestAARHUD::CycleToNextHUDState()
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
   else 
   {
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
         {
            mTaskTextList[i]->hide();
         }
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
         {
            mTaskTextList[i]->show();
         }
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
         {
            mTaskTextList[i]->show();
         }
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
         {
            mTaskTextList[i]->hide();
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
CEGUI::Window* TestAARHUD::CreateText(const std::string& name, CEGUI::Window* parent, const std::string& text,
                                 float x, float y, float width, float height)
{
   // create base window and set our default attribs
   CEGUI::Window* result = mGUI->CreateWidget(parent, "WindowsLook/StaticText", name);
   result->setText(text);
   result->setPosition(CEGUI::UVector2(cegui_absdim(x), cegui_absdim(y)));
   result->setSize(CEGUI::UVector2(cegui_absdim(width), cegui_absdim(height)));
   result->setProperty("FrameEnabled", "false");
   result->setProperty("BackgroundEnabled", "false");
   result->setHorizontalAlignment(CEGUI::HA_LEFT);
   result->setVerticalAlignment(CEGUI::VA_TOP);
   // set default color to white
   result->setProperty("TextColours", 
      CEGUI::PropertyHelper::colourToString(CEGUI::colour(1.0f, 1.0f, 1.0f)));
   result->show();

   return result;
}
