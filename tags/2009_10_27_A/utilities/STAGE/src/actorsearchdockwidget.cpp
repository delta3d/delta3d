/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
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
#include <prefix/dtstageprefix-src.h>
#include "dtEditQt/actorsearchdockwidget.h"
#include <dtEditQt/configurationmanager.h>
#include "dtEditQt/tabcontainer.h"
#include "dtEditQt/tabwrapper.h"
#include "dtEditQt/actorbrowser.h"
#include "dtEditQt/actorsearcher.h"
#include "dtEditQt/actorglobalbrowser.h"
#include "dtEditQt/editoractions.h"
#include <dtEditQt/prefabbrowser.h>

#include <QtGui/QWidget>
#include <QtGui/QAction>

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   ActorSearchDockWidget::ActorSearchDockWidget(QWidget* parent)
      : QDockWidget(parent)
      , mTabActorSearch(NULL)
      , mTabGlobalActor(NULL)
      , mActorSearchWidget(NULL)
      , mActorGlobalWidget(NULL)
   {
      setWindowTitle(tr("Search"));

      // container
      mTabC = new TabContainer(this);

      //Note: tabs and widgets are different

      if (ConfigurationManager::GetInstance().GetVariable(ConfigurationManager::LAYOUT, CONF_MGR_SHOW_ACTOR_SEARCH) != "false")
      {
         mTabActorSearch  = new TabWrapper(this);
         mActorSearchWidget  = new ActorSearcher(this);
      }

      if (ConfigurationManager::GetInstance().GetVariable(ConfigurationManager::LAYOUT, CONF_MGR_SHOW_GLOBAL_ACTORS) != "false")
      {
         mTabGlobalActor  = new TabWrapper(this);
         mActorGlobalWidget  = new ActorGlobalBrowser(this);
      }

      addTabs();

      setWidget(mTabC->getWidget());
   }

   /////////////////////////////////////////////////////////////////////////////////
   ActorSearchDockWidget::~ActorSearchDockWidget() {}

   /////////////////////////////////////////////////////////////////////////////////
   void ActorSearchDockWidget::addTabs()
   {
      if (mActorSearchWidget != NULL)
      {
         // Actor Search tab
         mTabActorSearch->setWidget(mActorSearchWidget);
         mTabActorSearch->setName("Actor Search");
         mTabC->addTab(mTabActorSearch);
      }

      if (mActorGlobalWidget != NULL)
      {
         // Global Actors
         mTabGlobalActor->setWidget(mActorGlobalWidget);
         mTabGlobalActor->setName("Global Actors");
         mTabC->addTab(mTabGlobalActor);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* ActorSearchDockWidget::getWidget()
   {
      return mTabC->getWidget();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ActorSearchDockWidget::closeEvent(QCloseEvent* e)
   {
      EditorActions::GetInstance().mActionWindowsActorSearch->setChecked(false);
   }

} // namespace dtEditQt
