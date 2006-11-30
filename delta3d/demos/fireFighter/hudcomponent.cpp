/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * William E. Johnson II
 */
#include <fireFighter/hudcomponent.h>
#include <fireFighter/messagetype.h>
#include <fireFighter/messages.h>
#include <fireFighter/gamestate.h>
#include <fireFighter/exception.h>
#include <fireFighter/firehoseactor.h>
#include <fireFighter/firesuitactor.h>
#include <fireFighter/scbaactor.h>
#include <fireFighter/fireactor.h>
#include <fireFighter/helpwindow.h>
#include <dtCore/globals.h>
#include <dtCore/deltawin.h>
#include <dtActors/taskactorgameevent.h>
#include <dtUtil/fileutils.h>
#include <dtABC/application.h>
#include <dtGUI/ceuidrawable.h>
#include <dtGame/taskcomponent.h>
#include <dtDAL/gameevent.h>

#ifdef None
#undef None
#endif
#include <CEGUI.h>

using dtCore::RefPtr;

const std::string &HUDComponent::NAME = "HUDComponent";

HUDComponent::HUDComponent(dtCore::DeltaWin &win, const std::string &name) : 
   dtGame::GMComponent(name), 
   mStartWithObjectives(NULL), 
   mStart(NULL), 
   mQuit(NULL), 
   mReturnToMenu(NULL), 
   mMainWindow(NULL), 
   mWindowBackground(NULL), 
   mHUDBackground(NULL),
   mDebriefBackground(NULL),
   mIntroBackground(NULL),
   mGameItemImage(NULL), 
   mFireSuitIcon(NULL), 
   mFireHoseIcon(NULL), 
   mSCBAIcon(NULL),
   mInventoryUseFireSuitIcon(NULL),
   mInventoryUseFireHoseIcon(NULL),
   mInventoryUseSCBAIcon(NULL),
   mInventorySelectIcon(NULL), 
   mTargetIcon(NULL), 
   mAppHeader(NULL),
   mDebriefHeaderText(NULL),
   mIntroText(NULL),
   mShowObjectives(true), 
   mCurrentState(&GameState::STATE_UNKNOWN), 
   mFireSuitIconPos(cegui_reldim(.525f), cegui_reldim(.8f)), 
   mFireHoseIconPos(cegui_reldim(.688f), cegui_reldim(.8f)), 
   mSCBAIconPos(cegui_reldim(.85f), cegui_reldim(.8f)), 
   mTasksHeaderText(NULL), 
   mNumTasks(11), 
   mMissionCompletedText(NULL), 
   mMissionFailedText(NULL), 
   mMissionComplete(false), 
   mMissionFailed(false), 
   mFailedProxy(NULL), 
   mCompleteOrFail(NULL), 
   mFailReason(NULL), 
   mHUDOverlay(NULL)
{
   SetupGUI(win);
}

HUDComponent::~HUDComponent()
{
  
}

void HUDComponent::SetupGUI(dtCore::DeltaWin &win)
{
   mGUI = new dtGUI::CEUIDrawable(&win);

   try 
   {
      std::string scheme = "CEGUI/schemes/WindowsLook.scheme";
      std::string path = dtCore::FindFileInPathList(scheme);
      if(path.empty())
      {
         throw dtUtil::Exception(ExceptionEnum::CEGUI_EXCEPTION, 
            "Failed to find the scheme file.", __FILE__, __LINE__);
      }

      std::string dir = path.substr(0, path.length() - (scheme.length() - 5));
      dtUtil::FileUtils::GetInstance().PushDirectory(dir);
      CEGUI::SchemeManager::getSingleton().loadScheme(path);
      dtUtil::FileUtils::GetInstance().PopDirectory();

      CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();
      CEGUI::System::getSingleton().setDefaultFont("DejaVuSans-10");
      mMainWindow = wm->createWindow("DefaultGUISheet", "root");
      CEGUI::System::getSingleton().setGUISheet(mMainWindow);
   
      BuildMainMenu();
      BuildIntroMenu();
      BuildHUD();
      BuildEndMenu();
   }
   catch(const CEGUI::Exception &e)
   {
      throw dtUtil::Exception(ExceptionEnum::CEGUI_EXCEPTION, e.getMessage().c_str(), 
         __FILE__, __LINE__);
   }
}

void HUDComponent::OnAddedToGM()
{
   GetGameManager()->GetApplication().AddDrawable(mGUI.get());
}

void HUDComponent::ProcessMessage(const dtGame::Message &msg)
{
   if(msg.GetMessageType() == MessageType::GAME_STATE_CHANGED)
   {
      const GameStateChangedMessage &gscm = static_cast<const GameStateChangedMessage&>(msg);
      mCurrentState = &gscm.GetNewState();
      Refresh();
   }
   else if(msg.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      GetGameManager()->GetApplication().AddDrawable(mGUI.get());
   }
   else if(msg.GetMessageType() == MessageType::ITEM_INTERSECTED)
   {
      if(!msg.GetAboutActorId().ToString().empty())
      {
         dtGame::GameActorProxy *proxy = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
         if(dynamic_cast<FireActor*>(proxy->GetActor()) == NULL)
            ShowGameItemImage();
         else
            HideGameItemImage();
      }
      else
      {
         HideGameItemImage();
      }
   }
   else if(msg.GetMessageType() == MessageType::ITEM_ACQUIRED)
   {
      dtGame::GameActorProxy *proxy = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      AddItemToHUD(dynamic_cast<GameItemActor*>(proxy->GetActor()));
   }
   else if(msg.GetMessageType() == MessageType::ITEM_SELECTED)
   {
      dtGame::GameActorProxy *proxy = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      SetSelectedItem(dynamic_cast<GameItemActor*>(proxy->GetActor()));
   }
   else if(msg.GetMessageType() == MessageType::ITEM_ACTIVATED)
   {
      dtGame::GameActorProxy *proxy = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      SetActivatedItem(dynamic_cast<GameItemActor*>(proxy->GetActor()));
   }
   else if(msg.GetMessageType() == MessageType::ITEM_DEACTIVATED)
   {
      dtGame::GameActorProxy *proxy = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      SetDeactivatedItem(dynamic_cast<GameItemActor*>(proxy->GetActor()));
   }
   else if(msg.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
   {
      if(*mCurrentState == GameState::STATE_RUNNING)
      {
         UpdateMediumDetailData(mHUDBackground);
      }
      else if(*mCurrentState == GameState::STATE_DEBRIEF)
         RefreshDebriefScreen();
   }
   else if(msg.GetMessageType() == MessageType::MISSION_COMPLETE)
   {
      mMissionComplete = true;
      mMissionFailed = false;
      Refresh();
   }
   else if(msg.GetMessageType() == MessageType::MISSION_FAILED)
   {
      mMissionFailed = true;
      mMissionComplete = false;
      GetGameManager()->FindActorById(msg.GetAboutActorId(), mFailedProxy);
      Refresh();
   }
   else if(msg.GetMessageType() == MessageType::HELP_WINDOW_OPENED)
   {
      mHelpWindow->Enable(true);
      ShowMouse(true);
   }
   else if(msg.GetMessageType() == MessageType::HELP_WINDOW_CLOSED)
   {
      mHelpWindow->Enable(false);
      ShowMouse(false);
   }
}

void HUDComponent::ShowMainMenu()
{
   HideMenus();
   ShowMouse(true);
   mWindowBackground->show();
}

void HUDComponent::ShowHUD()
{
   if(mGUI->GetActiveTextureUnit() != 1)
      mGUI->SetActiveTextureUnit(1);
    
   HideMenus();
   ShowMouse(false);
   
   mHUDBackground->show();
   if(mMissionComplete)
      mMissionCompletedText->show();
   else if(mMissionFailed)
      mMissionFailedText->show();
}

void HUDComponent::ShowEndMenu()
{
   HideMenus();
   ShowMouse(true);

   mDebriefBackground->show();
}

void HUDComponent::ShowIntroMenu()
{
   if(mGUI->GetActiveTextureUnit() != 0)
      mGUI->SetActiveTextureUnit(0);

   HideMenus();
   mIntroBackground->show();
}

void HUDComponent::ShowGameItemImage()
{
   mGameItemImage->show();
}

void HUDComponent::HideGameItemImage()
{
   mGameItemImage->hide();
}

void HUDComponent::HideMenus()
{
   mWindowBackground->hide();
   mHUDBackground->hide();
   mDebriefBackground->hide();
   mIntroBackground->hide();
}

void HUDComponent::BuildMainMenu()
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();

   mWindowBackground = wm->createWindow("WindowsLook/StaticImage", "MenuBackgroundImage");
   mMainWindow->addChildWindow(mWindowBackground);
   mWindowBackground->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)));
   mWindowBackground->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim(1.0f)));
   mWindowBackground->setProperty("BackgroundEnabled", "false");
   mWindowBackground->setProperty("FrameEnabled", "false");
   mWindowBackground->setProperty("Image", "set:BackgroundImage image:BackgroundImage");

   mAppHeader = wm->createWindow("WindowsLook/StaticText", "applicationHeaderText");
   mAppHeader->setText("  Fire Fighter");
   mAppHeader->setProperty("BackgroundEnabled", "false");
   mAppHeader->setProperty("FrameEnabled", "false");
   mAppHeader->setSize(CEGUI::UVector2(cegui_reldim(0.2f), cegui_reldim(0.1f)));
   mAppHeader->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.1f)));
   mAppHeader->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mWindowBackground->addChildWindow(mAppHeader);

   mStartWithObjectives = static_cast<CEGUI::PushButton*>(wm->createWindow("WindowsLook/Button", "startWithObjectivesButton"));
   mStartWithObjectives->setText("Start With Objectives");
   mStartWithObjectives->setSize(CEGUI::UVector2(cegui_reldim(0.3f), cegui_reldim(0.1f)));
   mStartWithObjectives->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.4f)));
   mStartWithObjectives->setMouseCursor(NULL);
   mStartWithObjectives->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mWindowBackground->addChildWindow(mStartWithObjectives);

   mStart = static_cast<CEGUI::PushButton*>(wm->createWindow("WindowsLook/Button", "startButton"));
   mStart->setText("Start");
   mStart->setSize(CEGUI::UVector2(cegui_reldim(0.3f), cegui_reldim(0.1f)));
   mStart->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.3f)));
   mStart->setMouseCursor(NULL);
   mStart->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mWindowBackground->addChildWindow(mStart);

   mQuit = static_cast<CEGUI::PushButton*>(wm->createWindow("WindowsLook/Button", "quitButton"));
   mQuit->setText("Quit");
   mQuit->setSize(CEGUI::UVector2(cegui_reldim(0.3f), cegui_reldim(0.1f)));
   mQuit->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.8f)));
   mQuit->setMouseCursor(NULL);
   mQuit->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mWindowBackground->addChildWindow(mQuit);

   mStartWithObjectives->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&HUDComponent::OnStartWithObjectives, this));
   mStart->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&HUDComponent::OnStart, this));
   mQuit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&HUDComponent::OnQuit, this));

   mWindowBackground->hide();
}

void HUDComponent::BuildHUD()
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();

   mHUDBackground = wm->createWindow("WindowsLook/StaticImage", "HUDBackgroundImage");
   mMainWindow->addChildWindow(mHUDBackground);
   mHUDBackground->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)));
   mHUDBackground->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim(1.0f)));
   mHUDBackground->setProperty("BackgroundEnabled", "false");
   mHUDBackground->setProperty("FrameEnabled", "false");

   mHUDOverlay = wm->createWindow("WindowsLook/StaticImage", "HUDOverlay");
   mHUDBackground->addChildWindow(mHUDOverlay);
   mHUDOverlay->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)));
   mHUDOverlay->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim(1.0f)));
   mHUDOverlay->setProperty("BackgroundEnabled", "false");
   mHUDOverlay->setProperty("FrameEnabled", "false");
   //mHUDOverlay->setProperty("Image", "set:HUD_SCBAImage image:HUD_SCBAImage");

   mGameItemImage = wm->createWindow("WindowsLook/StaticImage", "gameItemImage");
   mGameItemImage->setSize(CEGUI::UVector2(cegui_reldim(0.15f), cegui_reldim(0.15f)));
   mGameItemImage->setPosition(CEGUI::UVector2(cegui_reldim(0.6f), cegui_reldim(0.4f)));
   mGameItemImage->setProperty("BackgroundEnabled", "false");
   mGameItemImage->setProperty("FrameEnabled", "false");
   mGameItemImage->setProperty("Image", "set:GameItemImage image:GameItemImage");
   mHUDOverlay->addChildWindow(mGameItemImage);
   mGameItemImage->hide();

   mFireSuitIcon = wm->createWindow("WindowsLook/StaticImage", "fireSuitIcon");
   mFireSuitIcon->setSize(CEGUI::UVector2(cegui_reldim(0.15f), cegui_reldim(0.15f)));
   mFireSuitIcon->setPosition(mFireSuitIconPos);
   mFireSuitIcon->setProperty("BackgroundEnabled", "false");
   mFireSuitIcon->setProperty("FrameEnabled", "false");
   mFireSuitIcon->setProperty("Image", "set:FireSuitImage image:FireSuitImage");
   mHUDOverlay->addChildWindow(mFireSuitIcon);
   mFireSuitIcon->hide();

   mFireHoseIcon = wm->createWindow("WindowsLook/StaticImage", "fireHoseIcon");
   mFireHoseIcon->setSize(CEGUI::UVector2(cegui_reldim(0.15f), cegui_reldim(0.15f)));
   mFireHoseIcon->setPosition(mFireHoseIconPos);
   mFireHoseIcon->setProperty("BackgroundEnabled", "false");
   mFireHoseIcon->setProperty("FrameEnabled", "false");
   mFireHoseIcon->setProperty("Image", "set:FireHoseImage image:FireHoseImage");
   mHUDOverlay->addChildWindow(mFireHoseIcon);
   mFireHoseIcon->hide();

   mSCBAIcon = wm->createWindow("WindowsLook/StaticImage", "SCBAIcon");
   mSCBAIcon->setSize(CEGUI::UVector2(cegui_reldim(0.15f), cegui_reldim(0.15f)));
   mSCBAIcon->setPosition(mSCBAIconPos);
   mSCBAIcon->setProperty("BackgroundEnabled", "false");
   mSCBAIcon->setProperty("FrameEnabled", "false");
   mSCBAIcon->setProperty("Image", "set:SCBAImage image:SCBAImage");
   mHUDOverlay->addChildWindow(mSCBAIcon);
   mSCBAIcon->hide();

   mInventoryUseFireSuitIcon = wm->createWindow("WindowsLook/StaticImage", "InventoryUseFireSuitIcon");
   mInventoryUseFireSuitIcon->setSize(CEGUI::UVector2(cegui_reldim(0.15f), cegui_reldim(0.15f)));
   mInventoryUseFireSuitIcon->setPosition(CEGUI::UVector2(mFireSuitIconPos.d_x, mFireSuitIconPos.d_y + cegui_reldim(.025f)));
   mInventoryUseFireSuitIcon->setProperty("BackgroundEnabled", "false");
   mInventoryUseFireSuitIcon->setProperty("FrameEnabled", "false");
   mInventoryUseFireSuitIcon->setProperty("Image", "set:InventoryUseImage image:InventoryUseImage");
   mHUDOverlay->addChildWindow(mInventoryUseFireSuitIcon);
   mInventoryUseFireSuitIcon->hide();

   mInventoryUseFireHoseIcon = wm->createWindow("WindowsLook/StaticImage", "InventoryUseFireHoseIcon");
   mInventoryUseFireHoseIcon->setSize(CEGUI::UVector2(cegui_reldim(0.15f), cegui_reldim(0.15f)));
   mInventoryUseFireHoseIcon->setPosition(CEGUI::UVector2(mFireHoseIconPos.d_x, mFireHoseIconPos.d_y + cegui_reldim(.025f)));
   mInventoryUseFireHoseIcon->setProperty("BackgroundEnabled", "false");
   mInventoryUseFireHoseIcon->setProperty("FrameEnabled", "false");
   mInventoryUseFireHoseIcon->setProperty("Image", "set:InventoryUseImage image:InventoryUseImage");
   mHUDOverlay->addChildWindow(mInventoryUseFireHoseIcon);
   mInventoryUseFireHoseIcon->hide();

   mInventoryUseSCBAIcon = wm->createWindow("WindowsLook/StaticImage", "InventoryUseSCBAIcon");
   mInventoryUseSCBAIcon->setSize(CEGUI::UVector2(cegui_reldim(0.15f), cegui_reldim(0.15f)));
   mInventoryUseSCBAIcon->setPosition(CEGUI::UVector2(mSCBAIconPos.d_x, mSCBAIconPos.d_y + cegui_reldim(.025f)));
   mInventoryUseSCBAIcon->setProperty("BackgroundEnabled", "false");
   mInventoryUseSCBAIcon->setProperty("FrameEnabled", "false");
   mInventoryUseSCBAIcon->setProperty("Image", "set:InventoryUseImage image:InventoryUseImage");
   mHUDOverlay->addChildWindow(mInventoryUseSCBAIcon);
   mInventoryUseSCBAIcon->hide();

   mInventorySelectIcon = wm->createWindow("WindowsLook/StaticImage", "InventorySelectIcon");
   mInventorySelectIcon->setSize(CEGUI::UVector2(cegui_reldim(0.15f), cegui_reldim(0.15f)));
   mInventorySelectIcon->setProperty("BackgroundEnabled", "false");
   mInventorySelectIcon->setProperty("FrameEnabled", "false");
   mInventorySelectIcon->setProperty("Image", "set:InventorySelectImage image:InventorySelectImage");
   mHUDOverlay->addChildWindow(mInventorySelectIcon);
   mInventorySelectIcon->hide();

   mTargetIcon = wm->createWindow("WindowsLook/StaticImage", "TargetIcon");
   mTargetIcon->setSize(CEGUI::UVector2(cegui_reldim(0.15f), cegui_reldim(0.15f)));
   mTargetIcon->setPosition(CEGUI::UVector2(cegui_reldim(0.45f), cegui_reldim(0.45f)));
   mTargetIcon->setProperty("BackgroundEnabled", "false");
   mTargetIcon->setProperty("FrameEnabled", "false");
   mTargetIcon->setProperty("Image", "set:TargetImage image:TargetImage");
   mHUDOverlay->addChildWindow(mTargetIcon);

   mMissionCompletedText = wm->createWindow("WindowsLook/StaticText", "MissionCompleteText");
   mMissionCompletedText->setText("Mission Completed. Press M to debrief");
   mMissionCompletedText->setProperty("TextColours", CEGUI::PropertyHelper::colourToString(CEGUI::colour(0.0f, 1.0f, 0.0f)));
   mMissionCompletedText->setProperty("BackgroundEnabled", "false");
   mMissionCompletedText->setProperty("FrameEnabled", "false");
   mMissionCompletedText->setSize(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim(0.1f)));
   mMissionCompletedText->setPosition(CEGUI::UVector2(cegui_reldim(0.1f), cegui_reldim(0.1f)));
   mMissionCompletedText->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mHUDOverlay->addChildWindow(mMissionCompletedText);
   mMissionCompletedText->hide();

   mMissionFailedText = wm->createWindow("WindowsLook/StaticText", "MissionFailedText");
   mMissionFailedText->setText("Mission Failed. Press M to debrief");
   mMissionFailedText->setProperty("TextColours", CEGUI::PropertyHelper::colourToString(CEGUI::colour(1.0f, 0.0f, 0.0f)));
   mMissionFailedText->setProperty("BackgroundEnabled", "false");
   mMissionFailedText->setProperty("FrameEnabled", "false");
   mMissionFailedText->setSize(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim(0.1f)));
   mMissionFailedText->setPosition(CEGUI::UVector2(cegui_reldim(0.1f), cegui_reldim(0.1f)));
   mMissionFailedText->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mHUDOverlay->addChildWindow(mMissionFailedText);
   mMissionFailedText->hide();

   mHelpWindow = new HelpWindow(mHUDBackground);
   mHelpWindow->GetCloseButton()->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&HUDComponent::OnHelpWindowClosed, this));


   float curYPos       = 20.0f;
   float mTextHeight   = 25.0f;
   float taskTextWidth = 500.0f;

   mTasksHeaderText = CreateText("Task Header", mHUDOverlay, "Tasks:", 
      4, curYPos, taskTextWidth - 2, mTextHeight + 2);

   curYPos += 2;

   for(unsigned int i = 0; i < mNumTasks; i++)
   {
      std::ostringstream oss;
      oss << "Task " << i;
      curYPos += mTextHeight + 2;
      CEGUI::Window *text = CreateText(oss.str(), mHUDOverlay, "",
         12, curYPos, taskTextWidth - 2, mTextHeight + 2);

      mTaskTextList.push_back(text);
      text->hide();
   }
  
   mHUDBackground->hide();
}

void HUDComponent::BuildEndMenu()
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();
   
   mDebriefBackground = wm->createWindow("WindowsLook/StaticImage", "DebriefBackgroundImage");
   mMainWindow->addChildWindow(mDebriefBackground);
   mDebriefBackground->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)));
   mDebriefBackground->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim(1.0f)));
   mDebriefBackground->setProperty("BackgroundEnabled", "false");
   mDebriefBackground->setProperty("FrameEnabled", "false");
   mDebriefBackground->setProperty("Image", "set:BackgroundImage image:BackgroundImage");

   mDebriefHeaderText = wm->createWindow("WindowsLook/StaticText", "DebriefHeaderText");
   mDebriefHeaderText->setText("Debriefing");
   mDebriefHeaderText->setProperty("BackgroundEnabled", "false");
   mDebriefHeaderText->setProperty("FrameEnabled", "false");
   mDebriefHeaderText->setSize(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim(0.1f)));
   mDebriefHeaderText->setPosition(CEGUI::UVector2(cegui_reldim(0.18f), cegui_reldim(0.01f)));
   mDebriefHeaderText->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mDebriefBackground->addChildWindow(mDebriefHeaderText);

   mCompleteOrFail = wm->createWindow("WindowsLook/StaticText", "CompleteOrFailText");
   mCompleteOrFail->setProperty("BackgroundEnabled", "false");
   mCompleteOrFail->setProperty("FrameEnabled", "false");
   mCompleteOrFail->setSize(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim(0.1f)));
   mCompleteOrFail->setPosition(CEGUI::UVector2(cegui_reldim(0.17f), cegui_reldim(0.1f)));
   mCompleteOrFail->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mDebriefBackground->addChildWindow(mCompleteOrFail);

   mFailReason = wm->createWindow("WindowsLook/StaticText", "reasonText");
   mFailReason->setProperty("BackgroundEnabled", "false");
   mFailReason->setProperty("FrameEnabled", "false");
   mFailReason->setSize(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim(0.1f)));
   mFailReason->setPosition(CEGUI::UVector2(cegui_reldim(0.1f), cegui_reldim(0.2f)));
   mFailReason->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mDebriefBackground->addChildWindow(mFailReason);

   mReturnToMenu = static_cast<CEGUI::PushButton*>(wm->createWindow("WindowsLook/Button", "ReturnToMenuButton"));
   mReturnToMenu->setText("Return to Menu");
   mReturnToMenu->setSize(CEGUI::UVector2(cegui_reldim(0.3f), cegui_reldim(0.1f)));
   mReturnToMenu->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.8f)));
   mReturnToMenu->setMouseCursor(NULL);
   mReturnToMenu->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mDebriefBackground->addChildWindow(mReturnToMenu);

   mReturnToMenu->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&HUDComponent::OnReturnToMenu, this));

   float curYPos       = 40.0f;
   float mTextHeight   = 25.0f;
   float taskTextWidth = 500.0f;

   curYPos += 2;

   for(unsigned int i = 0; i < mNumTasks; i++)
   {
      std::ostringstream oss;
      oss << "Debrief Task " << i;
      curYPos += mTextHeight + 2;
      CEGUI::Window *text = CreateText(oss.str(), mDebriefBackground, "",
         12, curYPos, taskTextWidth - 2, mTextHeight + 2);

      mDebriefList.push_back(text);
      text->hide();
   }
   
   mDebriefBackground->hide();
} 

void HUDComponent::BuildIntroMenu()
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();
   
   mIntroBackground = wm->createWindow("WindowsLook/StaticImage", "IntroBackgroundImage");
   mMainWindow->addChildWindow(mIntroBackground);
   mIntroBackground->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)));
   mIntroBackground->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim(1.0f)));
   mIntroBackground->setProperty("BackgroundEnabled", "false");
   mIntroBackground->setProperty("FrameEnabled", "false");

   mIntroText = wm->createWindow("WindowsLook/StaticText", "IntroText");
   mIntroText->setText("Press N to skip");
   mIntroText->setProperty("BackgroundEnabled", "false");
   mIntroText->setProperty("FrameEnabled", "false");
   mIntroText->setSize(CEGUI::UVector2(cegui_reldim(0.4f), cegui_reldim(0.1f)));
   mIntroText->setPosition(CEGUI::UVector2(cegui_reldim(0.125f), cegui_reldim(0.01f)));
   mIntroText->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mIntroText->setProperty("TextColours", CEGUI::PropertyHelper::colourToString(CEGUI::colour(1.0f, 1.0f, 1.0f)));
   mIntroBackground->addChildWindow(mIntroText);

   mIntroBackground->hide();
}

bool HUDComponent::OnStartWithObjectives(const CEGUI::EventArgs &e)
{
   mShowObjectives = true;
   SendGameStateChangedMessage(GameState::STATE_MENU, GameState::STATE_INTRO);
   return true;
}

bool HUDComponent::OnStart(const CEGUI::EventArgs &e)
{
   mShowObjectives = false;
   SendGameStateChangedMessage(GameState::STATE_MENU, GameState::STATE_INTRO);
   return true;
}

bool HUDComponent::OnQuit(const CEGUI::EventArgs &e)
{
   GetGameManager()->GetApplication().Quit();
   return true;
}

bool HUDComponent::OnReturnToMenu(const CEGUI::EventArgs &e)
{
   SendGameStateChangedMessage(GameState::STATE_DEBRIEF, GameState::STATE_MENU);
   return true;
}

bool HUDComponent::OnHelpWindowClosed(const CEGUI::EventArgs &e)
{
   RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(MessageType::HELP_WINDOW_CLOSED);
   GetGameManager()->SendMessage(*msg);
   return true;
}

void HUDComponent::SendGameStateChangedMessage(GameState &oldState, GameState &newState)
{
   RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(MessageType::GAME_STATE_CHANGED);
   GameStateChangedMessage &gscm = static_cast<GameStateChangedMessage&>(*msg);
   gscm.SetOldState(oldState);
   gscm.SetNewState(newState);
   LOG_ALWAYS("Changing game state to: " + newState.GetName());
   GetGameManager()->SendMessage(gscm);
}

void HUDComponent::Refresh()
{
   if(*mCurrentState == GameState::STATE_MENU)
   {
      ShowMainMenu();
   }
   else if(*mCurrentState == GameState::STATE_INTRO)
   {
      ShowIntroMenu();
   }
   else if(*mCurrentState == GameState::STATE_RUNNING)
   {
      ShowHUD();
   }
   else if(*mCurrentState == GameState::STATE_DEBRIEF)
   {
      ShowEndMenu();
   } 
   else 
   {
      LOG_ERROR("Cannot switch to invalid state:" + mCurrentState->GetName());
   }
}

void HUDComponent::ShowMouse(bool enable)
{
   GetGameManager()->GetApplication().GetWindow()->ShowCursor(enable);
}

void HUDComponent::AddItemToHUD(GameItemActor *item)
{
   if(dynamic_cast<FireSuitActor*>(item) != NULL)
   {
      mFireSuitIcon->show();
   }
   else if(dynamic_cast<FireHoseActor*>(item) != NULL)
   {
      mFireHoseIcon->show();
   }
   else if(dynamic_cast<SCBAActor*>(item) != NULL)
   {
      mSCBAIcon->show();
   }
 
   // Since we now have items in the HUD, show the select icon
   static bool hackIsFirstTime = true;
   if(hackIsFirstTime)
   {
      SetSelectedItem(item);
      hackIsFirstTime = false;
   }
   mInventorySelectIcon->show();
}

void HUDComponent::SetSelectedItem(GameItemActor *item)
{
   if(dynamic_cast<FireSuitActor*>(item) != NULL)
   {
      mInventorySelectIcon->setPosition(CEGUI::UVector2(mFireSuitIconPos.d_x, mFireSuitIconPos.d_y - cegui_reldim(0.025f)));
   }
   else if(dynamic_cast<FireHoseActor*>(item) != NULL)
   {
      mInventorySelectIcon->setPosition(CEGUI::UVector2(mFireHoseIconPos.d_x, mFireHoseIconPos.d_y - cegui_reldim(0.025f)));
   }
   else if(dynamic_cast<SCBAActor*>(item) != NULL)
   {
      mInventorySelectIcon->setPosition(CEGUI::UVector2(mSCBAIconPos.d_x, mSCBAIconPos.d_y - cegui_reldim(0.025f)));
   }
   else
   {
      LOG_ERROR("Could not set the selected item. Unable to cast the parameter");
   }
}

void HUDComponent::SetActivatedItem(GameItemActor *item)
{
   if(dynamic_cast<FireSuitActor*>(item) != NULL)
   {
      mInventoryUseFireSuitIcon->show();
   }
   else if(dynamic_cast<FireHoseActor*>(item) != NULL)
   {
      mInventoryUseFireHoseIcon->show();
   }
   else if(dynamic_cast<SCBAActor*>(item) != NULL)
   {
      mInventoryUseSCBAIcon->show();
   }
   else 
   {
      LOG_ERROR("Could not set the activated item. Unable to cast the parameter");
   }

   // Update the main background
   if(mInventoryUseFireHoseIcon->isVisible())
   {
      if(mInventoryUseSCBAIcon->isVisible())
      {
         mHUDOverlay->setProperty("Image", "set:HUD_SCBA_FirehoseImage image:HUD_SCBA_FirehoseImage");
      }
      else
      {
         mHUDOverlay->setProperty("Image", "set:HUD_FirehoseImage image:HUD_FirehoseImage");
      }
   }
   else if(mInventoryUseSCBAIcon->isVisible())
   {
      mHUDOverlay->setProperty("Image", "set:HUD_SCBAImage image:HUD_SCBAImage");
   }
   else
   {
      mHUDOverlay->setProperty("Image", "set:AlphaImage image:AlphaImage");
   }
}

void HUDComponent::SetDeactivatedItem(GameItemActor *item)
{
   if(dynamic_cast<FireSuitActor*>(item) != NULL)
   {
      mInventoryUseFireSuitIcon->hide();
   }
   else if(dynamic_cast<FireHoseActor*>(item) != NULL)
   {
      mInventoryUseFireHoseIcon->hide();
   }
   else if(dynamic_cast<SCBAActor*>(item) != NULL)
   {
      mInventoryUseSCBAIcon->hide();
   }
   else 
   {
      LOG_ERROR("Could not set the deactivated item. Unable to cast the parameter");
   }

   // Update the main background
   if(mInventoryUseFireHoseIcon->isVisible())
   {
      if(mInventoryUseSCBAIcon->isVisible())
      {
         mHUDOverlay->setProperty("Image", "set:HUD_SCBA_FirehoseImage image:HUD_SCBA_FirehoseImage");
      }
      else
      {
         mHUDOverlay->setProperty("Image", "set:HUD_FirehoseImage image:HUD_FirehoseImage");
      }
   }
   else if(mInventoryUseSCBAIcon->isVisible())
   {
      mHUDOverlay->setProperty("Image", "set:HUD_SCBAImage image:HUD_SCBAImage");
   }
   else
   {
      mHUDOverlay->setProperty("Image", "set:AlphaImage image:AlphaImage");
   }
}

void HUDComponent::UpdateMediumDetailData(CEGUI::Window *parent)
{
   if(parent->isVisible())
   {
      std::ostringstream oss;
      std::vector<dtCore::RefPtr<dtGame::GameActorProxy> > tasks;
      unsigned int numAdded = 0;
      unsigned int numComplete = 0;

      dtGame::GMComponent *comp = GetGameManager()->GetComponentByName("LMSComponent");
      dtGame::TaskComponent *mTaskComponent = static_cast<dtGame::TaskComponent*>(comp);
      
      mTaskComponent->GetTopLevelTasks(tasks);

      // start our recursive method on each top level task
      for(unsigned int i = 0; i < tasks.size(); i++)
      {
         dtActors::TaskActorProxy *taskProxy = dynamic_cast<dtActors::TaskActorProxy*>(tasks[i].get());
         numAdded += RecursivelyAddTasks("", numAdded, taskProxy, numComplete, parent);
      }

      // blank out any of our placeholder task text controls that were left over
      if(parent == mHUDBackground)
      {
         for(unsigned int i = numAdded; i < mTaskTextList.size(); i++)
            UpdateStaticText(mTaskTextList[i], "");
      }
      else
      {
         for(unsigned int i = numAdded; i < mDebriefList.size(); i++)
            UpdateStaticText(mDebriefList[i], "");
      }

      // update our task header
      oss << "Tasks (" << numComplete << " of " << numAdded << ")";
      if(numComplete < numAdded)
         UpdateStaticText(mTasksHeaderText, oss.str(), 1.0f, 1.0f, 1.0f);
      else
         UpdateStaticText(mTasksHeaderText, oss.str(), 0.0f, 1.0f, 0.0f);
   }
}

unsigned int HUDComponent::RecursivelyAddTasks(const std::string &indent, 
                                               unsigned int curIndex,
                                               const dtActors::TaskActorProxy *taskProxy, 
                                               unsigned int &numCompleted, 
                                               CEGUI::Window *parent)
{
   std::ostringstream oss;
   oss.setf(std::ios_base::fixed, std::ios_base::floatfield);
   oss.precision(2);

   unsigned int totalNumAdded = 0;
   if(curIndex < mTaskTextList.size())
   {
      // update the text for this task
      const dtActors::TaskActor *task = dynamic_cast<const dtActors::TaskActor*>(taskProxy->GetActor());
      if(task->IsComplete())
      {
         numCompleted++;

         const dtActors::TaskActorGameEvent *tage = dynamic_cast<const dtActors::TaskActorGameEvent*>(task);
         if(tage != NULL)
         {
            dtDAL::GameEvent *event = tage->GetGameEvent();
            oss << indent << event->GetDescription();
         }
         else
            oss << indent << task->GetDescription();
         
         if(parent == mHUDBackground)
            UpdateStaticText(mTaskTextList[curIndex + totalNumAdded], oss.str(), 0.0f, 1.0f, 0.0f);
         else
            UpdateStaticText(mDebriefList[curIndex + totalNumAdded], oss.str(), 0.0f, 1.0f, 0.0f);
      }
      else
      {
         const dtActors::TaskActorGameEvent *tage = dynamic_cast<const dtActors::TaskActorGameEvent*>(task);
         if(tage != NULL)
         {
            dtDAL::GameEvent *event = tage->GetGameEvent();
            oss << indent << event->GetDescription();
         }
         else
            oss << indent << task->GetDescription();

         if(parent == mHUDBackground)
         {
            if(&task->GetGameActorProxy() == mFailedProxy)
               UpdateStaticText(mTaskTextList[curIndex + totalNumAdded], oss.str(), 1.0f, 0.0f, 0.0f);
            else
               UpdateStaticText(mTaskTextList[curIndex + totalNumAdded], oss.str(), 1.0f, 1.0f, 1.0f);
         }
         else
         {
            if(&task->GetGameActorProxy() == mFailedProxy)
               UpdateStaticText(mDebriefList[curIndex + totalNumAdded], oss.str(), 1.0f, 0.0f, 0.0f);
            else
               UpdateStaticText(mDebriefList[curIndex + totalNumAdded], oss.str(), 1.0f, 1.0f, 1.0f);
         }
      }

      totalNumAdded++;

      // recurse for each child
      const std::vector<dtCore::RefPtr<dtActors::TaskActorProxy> > &children = taskProxy->GetAllSubTasks();
      if(!children.empty())
      {
         for(unsigned int i = 0; i < children.size(); i++)
         {
            const dtActors::TaskActorProxy *childProxy = dynamic_cast<const dtActors::TaskActorProxy*>(children[i].get());
            totalNumAdded += RecursivelyAddTasks(indent + "     ", curIndex + totalNumAdded, childProxy, numCompleted, parent);
         }
      }
   }

   return totalNumAdded;
}

void HUDComponent::UpdateStaticText(CEGUI::Window *textControl, const std::string &newText,
                                    float red, float green, float blue, float x, float y)
{
   
   if(textControl != NULL)
   {
      // text and color
      if(!newText.empty())
      {
         if(textControl->getText() != newText)
            textControl->setText(newText);
         if(red >= 0.0f && blue >= 0.0f && green >= 0.0f)
            textControl->setProperty("TextColours", CEGUI::PropertyHelper::colourToString(CEGUI::colour(red, green, blue)));
      }
      // position
      if(x > 0.0f && y > 0.0f)
      {
         CEGUI::UVector2 position = textControl->getPosition();
         CEGUI::UVector2 newPos(cegui_reldim(x), cegui_reldim(y));
         if(position != newPos)
            textControl->setPosition(newPos);
      }
      if(*mCurrentState == GameState::STATE_RUNNING) 
      {
         if(mHUDBackground->isVisible())
         {
            if(mShowObjectives)
               textControl->show(); 
            else
               textControl->hide();
         }
      }   
      else
      {
         if(mDebriefBackground->isVisible())
            textControl->show();
      }
   }
}

CEGUI::Window* HUDComponent::CreateText(const std::string &name, 
                                        CEGUI::Window *parent, 
                                        const std::string &text,
                                        float x, float y, float width, float height)
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();

   // create base window and set our default attribs
   CEGUI::Window *result = wm->createWindow("WindowsLook/StaticText", name);
   parent->addChildWindow(result);
   result->setText(text);
   result->setPosition(CEGUI::UVector2(cegui_absdim(x), cegui_absdim(y)));
   result->setSize(CEGUI::UVector2(cegui_absdim(width), cegui_absdim(height)));
   result->setProperty("FrameEnabled", "false");
   result->setProperty("BackgroundEnabled", "false");
   result->setHorizontalAlignment(CEGUI::HA_LEFT);
   result->setVerticalAlignment(CEGUI::VA_TOP);

   return result;
}

void HUDComponent::RefreshDebriefScreen()
{
   mCompleteOrFail->setText(mMissionComplete ? "Mission Completed" : "Mission Failed");
   mCompleteOrFail->setProperty("TextColours", mMissionComplete ? 
      CEGUI::PropertyHelper::colourToString(CEGUI::colour(0.0f, 1.0f, 0.0f)) : 
      CEGUI::PropertyHelper::colourToString(CEGUI::colour(1.0f, 0.0f, 0.0f)));
   
   UpdateMediumDetailData(mDebriefBackground);
}
