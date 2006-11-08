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
#include <dtCore/globals.h>
#include <dtCore/deltawin.h>
#include <dtActors/taskactor.h>
#include <dtUtil/fileutils.h>
#include <dtABC/application.h>
#include <dtGUI/ceuidrawable.h>
#include <dtGame/taskcomponent.h>
#include <iomanip>

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
   mTaskHeaderText(NULL),
   mIntroText(NULL),
   mShowObjectives(false), 
   mCurrentState(&GameState::STATE_UNKNOWN), 
   mFireSuitIconPos(0.525f, 0.8f), 
   mFireHoseIconPos(0.688f, 0.8f), 
   mSCBAIconPos(0.85f, 0.8f), 
   mTasksHeaderText(NULL), 
   mNumTasks(11)
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
      CEGUI::System::getSingleton().setDefaultFont("Tahoma-12");
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
      UpdateMediumDetailData();
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
   mHUDBackground->show();
}

void HUDComponent::ShowEndMenu()
{
   HideMenus();
   ShowMouse(true);
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

   mWindowBackground = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "MenuBackgroundImage"));
   mMainWindow->addChildWindow(mWindowBackground);
   mWindowBackground->setPosition(CEGUI::Point(0.0f, 0.0f));
   mWindowBackground->setSize(CEGUI::Size(1.0f, 1.0f));
   mWindowBackground->setBackgroundEnabled(false);
   mWindowBackground->setFrameEnabled(false);
   mWindowBackground->setImage("BackgroundImage", "BackgroundImage");

   mAppHeader = static_cast<CEGUI::StaticText*>(wm->createWindow("WindowsLook/StaticText", "applicationHeaderText"));
   mAppHeader->setText("  Fire Fighter");
   mAppHeader->setBackgroundEnabled(false);
   mAppHeader->setFrameEnabled(false);
   mAppHeader->setSize(CEGUI::Size(0.2f, 0.1f));
   mAppHeader->setPosition(CEGUI::Point(0, 0.1));
   mAppHeader->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mWindowBackground->addChildWindow(mAppHeader);

   mStartWithObjectives = static_cast<CEGUI::PushButton*>(wm->createWindow("WindowsLook/Button", "startWithObjectivesButton"));
   mStartWithObjectives->setText("Start With Objectives");
   mStartWithObjectives->setSize(CEGUI::Size(0.3f, 0.1f));
   mStartWithObjectives->setPosition(CEGUI::Point(0, 0.4));
   mStartWithObjectives->setMouseCursor(NULL);
   mStartWithObjectives->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mWindowBackground->addChildWindow(mStartWithObjectives);

   mStart = static_cast<CEGUI::PushButton*>(wm->createWindow("WindowsLook/Button", "startButton"));
   mStart->setText("Start");
   mStart->setSize(CEGUI::Size(0.3f, 0.1f));
   mStart->setPosition(CEGUI::Point(0, 0.3));
   mStart->setMouseCursor(NULL);
   mStart->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mWindowBackground->addChildWindow(mStart);

   mQuit = static_cast<CEGUI::PushButton*>(wm->createWindow("WindowsLook/Button", "quitButton"));
   mQuit->setText("Quit");
   mQuit->setSize(CEGUI::Size(0.3f, 0.1f));
   mQuit->setPosition(CEGUI::Point(0, 0.8));
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

   mHUDBackground = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "HUDBackgroundImage"));
   mMainWindow->addChildWindow(mHUDBackground);
   mHUDBackground->setPosition(CEGUI::Point(0.0f, 0.0f));
   mHUDBackground->setSize(CEGUI::Size(1.0f, 1.0f));
   mHUDBackground->setBackgroundEnabled(false);
   mHUDBackground->setFrameEnabled(false);
   
   mGameItemImage = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "gameItemImage"));
   mGameItemImage->setSize(CEGUI::Size(0.15f, 0.15f));
   mGameItemImage->setPosition(CEGUI::Point(0.6f, 0.4f));
   mGameItemImage->setBackgroundEnabled(false);
   mGameItemImage->setFrameEnabled(false);
   mGameItemImage->setImage("GameItemImage", "GameItemImage");
   mHUDBackground->addChildWindow(mGameItemImage);
   mGameItemImage->hide();

   mFireSuitIcon = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "fireSuitIcon"));
   mFireSuitIcon->setSize(CEGUI::Size(0.15f, 0.15f));
   mFireSuitIcon->setPosition(mFireSuitIconPos);
   mFireSuitIcon->setBackgroundEnabled(false);
   mFireSuitIcon->setFrameEnabled(false);
   mFireSuitIcon->setImage("FireSuitImage", "FireSuitImage");
   mHUDBackground->addChildWindow(mFireSuitIcon);
   mFireSuitIcon->hide();

   mFireHoseIcon = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "fireHoseIcon"));
   mFireHoseIcon->setSize(CEGUI::Size(0.15f, 0.15f));
   mFireHoseIcon->setPosition(mFireHoseIconPos);
   mFireHoseIcon->setBackgroundEnabled(false);
   mFireHoseIcon->setFrameEnabled(false);
   mFireHoseIcon->setImage("FireHoseImage", "FireHoseImage");
   mHUDBackground->addChildWindow(mFireHoseIcon);
   mFireHoseIcon->hide();

   mSCBAIcon = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "SCBAIcon"));
   mSCBAIcon->setSize(CEGUI::Size(0.15f, 0.15f));
   mSCBAIcon->setPosition(mSCBAIconPos);
   mSCBAIcon->setBackgroundEnabled(false);
   mSCBAIcon->setFrameEnabled(false);
   mSCBAIcon->setImage("SCBAImage", "SCBAImage");
   mHUDBackground->addChildWindow(mSCBAIcon);
   mSCBAIcon->hide();

   mInventoryUseFireSuitIcon = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "InventoryUseFireSuitIcon"));
   mInventoryUseFireSuitIcon->setSize(CEGUI::Size(0.15f, 0.15f));
   mInventoryUseFireSuitIcon->setPosition(CEGUI::Point(mFireSuitIconPos.d_x, mFireSuitIconPos.d_y + 0.025f));
   mInventoryUseFireSuitIcon->setBackgroundEnabled(false);
   mInventoryUseFireSuitIcon->setFrameEnabled(false);
   mInventoryUseFireSuitIcon->setImage("InventoryUseImage", "InventoryUseImage");
   mHUDBackground->addChildWindow(mInventoryUseFireSuitIcon);
   mInventoryUseFireSuitIcon->hide();

   mInventoryUseFireHoseIcon = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "InventoryUseFireHoseIcon"));
   mInventoryUseFireHoseIcon->setSize(CEGUI::Size(0.15f, 0.15f));
   mInventoryUseFireHoseIcon->setPosition(CEGUI::Point(mFireHoseIconPos.d_x, mFireHoseIconPos.d_y + 0.025f));
   mInventoryUseFireHoseIcon->setBackgroundEnabled(false);
   mInventoryUseFireHoseIcon->setFrameEnabled(false);
   mInventoryUseFireHoseIcon->setImage("InventoryUseImage", "InventoryUseImage");
   mHUDBackground->addChildWindow(mInventoryUseFireHoseIcon);
   mInventoryUseFireHoseIcon->hide();

   mInventoryUseSCBAIcon = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "InventoryUseSCBAIcon"));
   mInventoryUseSCBAIcon->setSize(CEGUI::Size(0.15f, 0.15f));
   mInventoryUseSCBAIcon->setPosition(CEGUI::Point(mSCBAIconPos.d_x, mSCBAIconPos.d_y + 0.025f));
   mInventoryUseSCBAIcon->setBackgroundEnabled(false);
   mInventoryUseSCBAIcon->setFrameEnabled(false);
   mInventoryUseSCBAIcon->setImage("InventoryUseImage", "InventoryUseImage");
   mHUDBackground->addChildWindow(mInventoryUseSCBAIcon);
   mInventoryUseSCBAIcon->hide();

   mInventorySelectIcon = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "InventorySelectIcon"));
   mInventorySelectIcon->setSize(CEGUI::Size(0.15f, 0.15f));
   mInventorySelectIcon->setBackgroundEnabled(false);
   mInventorySelectIcon->setFrameEnabled(false);
   mInventorySelectIcon->setImage("InventorySelectImage", "InventorySelectImage");
   mHUDBackground->addChildWindow(mInventorySelectIcon);
   mInventorySelectIcon->hide();

   mTargetIcon = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "TargetIcon"));
   mTargetIcon->setSize(CEGUI::Size(0.15f, 0.15f));
   mTargetIcon->setPosition(CEGUI::Point(0.45f, 0.45f));
   mTargetIcon->setBackgroundEnabled(false);
   mTargetIcon->setFrameEnabled(false);
   mTargetIcon->setImage("TargetImage", "TargetImage");
   mHUDBackground->addChildWindow(mTargetIcon);

   float curYPos       = 70.0f;
   float mTextHeight   = 20.0f;
   float taskTextWidth = 300.0f;

   mTasksHeaderText = CreateText("Task Header", mHUDBackground, "Tasks:", 
      4, curYPos, taskTextWidth - 2, mTextHeight + 2);

   curYPos += 2;

   // 11 placeholders for tasks
   for(int i = 0; i < mNumTasks; i++)
   {
      std::ostringstream oss;
      oss << "Task " << i;
      curYPos += mTextHeight + 2;
      CEGUI::StaticText *text = CreateText(oss.str(), mHUDBackground, "",
         12, curYPos, taskTextWidth - 2, mTextHeight + 2);

      mTaskTextList.push_back(text);
      text->hide();
   }
  
   mHUDBackground->hide();
}

void HUDComponent::BuildEndMenu()
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();
   
   mDebriefBackground = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "DebriefBackgroundImage"));
   mMainWindow->addChildWindow(mDebriefBackground);
   mDebriefBackground->setPosition(CEGUI::Point(0.0f, 0.0f));
   mDebriefBackground->setSize(CEGUI::Size(1.0f, 1.0f));
   mDebriefBackground->setBackgroundEnabled(false);
   mDebriefBackground->setFrameEnabled(false);
   mDebriefBackground->setImage("BackgroundImage", "BackgroundImage");

   mTaskHeaderText = static_cast<CEGUI::StaticText*>(wm->createWindow("WindowsLook/StaticText", "taskHeaderText"));
   mTaskHeaderText->setText("Task Status");
   mTaskHeaderText->setBackgroundEnabled(false);
   mTaskHeaderText->setFrameEnabled(false);
   mTaskHeaderText->setSize(CEGUI::Size(0.3f, 0.1f));
   mTaskHeaderText->setPosition(CEGUI::Point(0, 0.1));
   mTaskHeaderText->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mDebriefBackground->addChildWindow(mTaskHeaderText);

   mDebriefBackground->hide();
} 

void HUDComponent::BuildIntroMenu()
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();
   
   mIntroBackground = static_cast<CEGUI::StaticImage*>(wm->createWindow("WindowsLook/StaticImage", "IntroBackgroundImage"));
   mMainWindow->addChildWindow(mIntroBackground);
   mIntroBackground->setPosition(CEGUI::Point(0.0f, 0.0f));
   mIntroBackground->setSize(CEGUI::Size(1.0f, 1.0f));
   mIntroBackground->setBackgroundEnabled(false);
   mIntroBackground->setFrameEnabled(false);

   mIntroText = static_cast<CEGUI::StaticText*>(wm->createWindow("WindowsLook/StaticText", "IntroText"));
   mIntroText->setText("Press N to skip");
   mIntroText->setBackgroundEnabled(false);
   mIntroText->setFrameEnabled(false);
   mIntroText->setSize(CEGUI::Size(0.4f, 0.1f));
   mIntroText->setPosition(CEGUI::Point(0.125, 0.01));
   mIntroText->setHorizontalAlignment(CEGUI::HA_CENTRE);
   mIntroText->setTextColours(CEGUI::colour(1.0f, 1.0f, 1.0f));
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
   SendGameStateChangedMessage(GameState::STATE_MENU, GameState::STATE_INTRO);
   return true;
}

bool HUDComponent::OnQuit(const CEGUI::EventArgs &e)
{
   GetGameManager()->GetApplication().Quit();
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
   else
   {
      LOG_ERROR("Failed to add the item to the HUD. Unable to cast the parameter");
      return;
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
      mInventorySelectIcon->setPosition(CEGUI::Point(mFireSuitIconPos.d_x, mFireSuitIconPos.d_y - 0.025f));
   }
   else if(dynamic_cast<FireHoseActor*>(item) != NULL)
   {
      mInventorySelectIcon->setPosition(CEGUI::Point(mFireHoseIconPos.d_x, mFireHoseIconPos.d_y - 0.025f));
   }
   else if(dynamic_cast<SCBAActor*>(item) != NULL)
   {
      mInventorySelectIcon->setPosition(CEGUI::Point(mSCBAIconPos.d_x, mSCBAIconPos.d_y - 0.025f));
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
}

void HUDComponent::UpdateMediumDetailData()
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
      numAdded += RecursivelyAddTasks("", numAdded, taskProxy, numComplete);
   }

   // blank out any of our placeholder task text controls that were left over
   for(unsigned int i = numAdded; i < mTaskTextList.size(); i++)
      UpdateStaticText(mTaskTextList[i], "");

   // update our task header
   oss << "Tasks (" << numComplete << " of " << numAdded << ")";
   if(numComplete < numAdded)
      UpdateStaticText(mTasksHeaderText, oss.str(), 1.0, 1.0, 1.0);
   else
      UpdateStaticText(mTasksHeaderText, oss.str(), 0.1, 1.0, 0.1);
}

unsigned int HUDComponent::RecursivelyAddTasks(const std::string &indent, 
                                               unsigned int curIndex,
                                               const dtActors::TaskActorProxy *taskProxy, 
                                               unsigned int &numCompleted)
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

         oss << indent << task->GetName() << " Y " << task->GetScore();
         UpdateStaticText(mTaskTextList[curIndex + totalNumAdded], oss.str(), 0.0, 1.0, 0.0);
      }
      else
      {
         oss << indent << task->GetName() << " N " << task->GetScore();
         UpdateStaticText(mTaskTextList[curIndex + totalNumAdded], oss.str(), 1.0, 1.0, 1.0);
      }

      totalNumAdded++;

      // recurse for each child
      const std::vector<dtCore::RefPtr<dtActors::TaskActorProxy> > &children = taskProxy->GetAllSubTasks();
      if(!children.empty())
      {
         for(unsigned int i = 0; i < children.size(); i++)
         {
            const dtActors::TaskActorProxy *childProxy = dynamic_cast<const dtActors::TaskActorProxy*>(children[i].get());
            totalNumAdded += RecursivelyAddTasks(indent + "     ", curIndex + totalNumAdded, childProxy, numCompleted);
         }
      }
   }

   return totalNumAdded;
}

void HUDComponent::UpdateStaticText(CEGUI::StaticText *textControl, 
                                    const std::string &newText,
                                    float red, 
                                    float blue, 
                                    float green,
                                    float x, 
                                    float y)
{
   if(textControl != NULL)
   {
      // text and color
      if(!newText.empty() && textControl->getText() != newText)
      {
         textControl->setText(newText);
         if(red >= 0.00 && blue >= 0.0 && green >= 0.0)
            textControl->setTextColours(CEGUI::colour(red, blue, green));
      }
      // position
      if(x > 0.0 && y > 0.0)
      {
         CEGUI::Point position = textControl->getPosition();
         CEGUI::Point newPos(x, y);
         if(position != newPos)
            textControl->setPosition(newPos);
      }
      mShowObjectives ? textControl->show() : textControl->hide();
   }
}

CEGUI::StaticText* HUDComponent::CreateText(const std::string &name, 
                                            CEGUI::StaticImage *parent, 
                                            const std::string &text,
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

   return result;
}