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
#include <prefix/stageprefix.h>
#include <QtGui/QComboBox>
#include <QtGui/QDirModel>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtCore/QStringList>
#include <QtGui/QTreeView>
#include <dtCore/actorproxy.h>
#include <dtCore/actortype.h>
#include <dtCore/actorfactory.h>
#include <dtCore/map.h>
#include <dtEditQt/actorglobalbrowser.h>
#include <dtEditQt/actorresultstable.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   ActorGlobalBrowser::ActorGlobalBrowser(QWidget* parent)
      : QWidget(parent)
   {
      QVBoxLayout* vbox = new QVBoxLayout(this);
      vbox->addWidget(resultsGroup(), 1, 0);
      vbox->setSpacing(0);
      vbox->setMargin(3);

      // connect
      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryImported()),
         this, SLOT(refreshAll()));
      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryRemoved()),
         this, SLOT(refreshAll()));
      connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
         this, SLOT(refreshAll()));
      connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
         this, SLOT(refreshAll()));
      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryAboutToBeRemoved()),
         this, SLOT(refreshAll()));
      connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyCreated(dtCore::ActorPtr, bool)),
         this, SLOT(onActorProxyCreated(dtCore::ActorPtr, bool)));
      connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyDestroyed(dtCore::ActorPtr)),
         this, SLOT(onActorProxyDestroyed(dtCore::ActorPtr)));

      connect(&EditorEvents::GetInstance(),
         SIGNAL(actorPropertyChanged(dtCore::ActorPtr, ActorPropertyRefPtr)),
         this,
         SLOT(onActorPropertyChanged(dtCore::ActorPtr, ActorPropertyRefPtr)));

      connect(&EditorEvents::GetInstance(),
         SIGNAL(ProxyNameChanged(dtCore::BaseActorObject&, std::string)),
         this,
         SLOT(onActorProxyNameChanged(dtCore::BaseActorObject&, std::string)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorGlobalBrowser::~ActorGlobalBrowser()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* ActorGlobalBrowser::resultsGroup()
   {
      QGroupBox* groupBox = new QGroupBox(tr("Global Actors"),this);

      mResultsTable = new ActorResultsTable(true, false, groupBox);

      QVBoxLayout* vbox = new QVBoxLayout(groupBox);
      vbox->addWidget(mResultsTable, 1, 0); // take the rest of the space
      vbox->setSpacing(2);
      vbox->setMargin(2);

      return groupBox;
   }

   ///////////////////////////////////////////////////////////////////////////////
   // SLOTS
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   void ActorGlobalBrowser::refreshAll()
   {
      EditorData::GetInstance().getMainWindow()->startWaitCursor();

      std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > globalProxies;
      dtCore::Map* map = EditorData::GetInstance().getCurrentMap();

      // empty out our table, just in case - Must happen BEFORE libraries are removed
      mResultsTable->clearAll();

      if (map != NULL)
      {
         map->FindProxies(globalProxies, "", "", "", "", dtCore::Map::NotPlaceable);

         mResultsTable->addProxies(globalProxies);
      }

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorGlobalBrowser::onActorProxyCreated(dtCore::RefPtr<dtCore::BaseActorObject> proxy, bool /*forceNoAdjustments*/)
   {
      if (!proxy->IsPlaceable())
      {
         mResultsTable->addProxy(proxy);
      }

      //refreshAll();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorGlobalBrowser::onActorProxyDestroyed(dtCore::RefPtr<dtCore::BaseActorObject> proxy)
   {
      if (!proxy->IsPlaceable())
      {
         mResultsTable->actorProxyAboutToBeDestroyed(proxy);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorGlobalBrowser::onActorPropertyChanged(dtCore::ActorPtr proxy, ActorPropertyRefPtr /*property*/)
   {
      mResultsTable->HandleProxyUpdated(proxy);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorGlobalBrowser::onActorProxyNameChanged(dtCore::BaseActorObject& proxy, std::string /*oldName*/)
   {
      mResultsTable->HandleProxyUpdated(&proxy);
   }

} // namespace dtEditQt
