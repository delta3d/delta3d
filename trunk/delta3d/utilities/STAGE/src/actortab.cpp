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
#include "dtEditQt/actortab.h"
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
   ActorTab::ActorTab(QWidget* parent)
      : QDockWidget(parent)
   {
      setWindowTitle(tr("Actors"));

      // container
      mTabC = new TabContainer(this);

      // tabs
      mTabActorBrowser = new TabWrapper(this);
      mTabActorSearch  = new TabWrapper(this);
      mTabGlobalActor  = new TabWrapper(this);
      mTabPrefabBrowser= new TabWrapper(this);

      // widgets
      mActorBrowserWidget = new ActorBrowser(this);
      mActorSearchWidget  = new ActorSearcher(this);
      mActorGlobalWidget  = new ActorGlobalBrowser(this);
      mPrefabBrowserWidget= new PrefabBrowser(this);

      addTabs();

      setWidget(mTabC->getWidget());
   }

   /////////////////////////////////////////////////////////////////////////////////
   ActorTab::~ActorTab() {}

   /////////////////////////////////////////////////////////////////////////////////
   void ActorTab::addTabs()
   {
      // Actor Browser Tab
      mTabActorBrowser->setWidget(mActorBrowserWidget);
      mTabActorBrowser->setName("Actors");
      mTabC->addTab(mTabActorBrowser);

      // Actor Search tab
      mTabActorSearch->setWidget(mActorSearchWidget);
      mTabActorSearch->setName("Actor Search");
      mTabC->addTab(mTabActorSearch);

      // Global Actors
      mTabGlobalActor->setWidget(mActorGlobalWidget);
      mTabGlobalActor->setName("Global Actors");
      mTabC->addTab(mTabGlobalActor);

      // Prefabs Tab
      mTabPrefabBrowser->setWidget(mPrefabBrowserWidget);
      mTabPrefabBrowser->setName("Prefabs");
      mTabC->addTab(mTabPrefabBrowser);
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* ActorTab::getWidget()
   {
      return mTabC->getWidget();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ActorTab::closeEvent(QCloseEvent* e)
   {
      EditorActions::GetInstance().mActionWindowsActorSearch->setChecked(false);
   }

} // namespace dtEditQt
