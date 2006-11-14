/* 
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
* Copyright (C) 2005, BMH Associates, Inc. 
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free 
* Software Foundation; either version 2 of the License, or (at your option) 
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
* details.
*
* You should have received a copy of the GNU General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
* Curtiss Murphy
*/
#include <prefix/dtstageprefix-src.h>
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
#include <dtDAL/actorproxy.h>
#include <dtDAL/actortype.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>
#include <dtEditQt/actorglobalbrowser.h>
#include <dtEditQt/actorresultstable.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/typedefs.h>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    ActorGlobalBrowser::ActorGlobalBrowser(QWidget *parent)
        :QWidget(parent)
    {
        QVBoxLayout *vbox = new QVBoxLayout(this);
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
        connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyCreated(ActorProxyRefPtr, bool)), 
            this, SLOT(onActorProxyCreated(ActorProxyRefPtr, bool)));   
        connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyDestroyed(ActorProxyRefPtr)), 
            this, SLOT(onActorProxyDestroyed(ActorProxyRefPtr)));
    }

    ///////////////////////////////////////////////////////////////////////////////
    ActorGlobalBrowser::~ActorGlobalBrowser()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *ActorGlobalBrowser::resultsGroup()
    {
        QGroupBox *groupBox = new QGroupBox(tr("Global Actors"),this);

        resultsTable = new ActorResultsTable(true, false, groupBox);

        QVBoxLayout *vbox = new QVBoxLayout(groupBox);
        vbox->addWidget(resultsTable, 1, 0); // take the rest of the space
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

        std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > globalProxies;
        dtDAL::Map *map = EditorData::GetInstance().getCurrentMap();

        // empty out our table, just in case - Must happen BEFORE libraries are removed
        resultsTable->clearAll();

        if (map != NULL) {
            map->FindProxies(globalProxies, "", "", "", "", dtDAL::Map::NotPlaceable);

            resultsTable->addProxies(globalProxies);
        }

        EditorData::GetInstance().getMainWindow()->endWaitCursor();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorGlobalBrowser::onActorProxyCreated(dtCore::RefPtr<dtDAL::ActorProxy> proxy, bool forceNoAdjustments)
    {
        if (!proxy->IsPlaceable())
            resultsTable->addProxy(proxy);
    }

    void ActorGlobalBrowser::onActorProxyDestroyed(dtCore::RefPtr<dtDAL::ActorProxy> proxy)
    {
        if(!proxy->IsPlaceable())
            refreshAll();
    }
}
