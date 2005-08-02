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
* @author Teague Coonan
*/
#include <QDir>
#include <QHeaderView>

#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QString>

#include <QGroupBox>

#include <QPushButton>
#include <QCheckBox>

#include <QAction>
#include <QContextMenuEvent>

#include "dtEditQt/resourcetreewidget.h"
#include "dtDAL/project.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/mainwindow.h"

#include "dtEditQt/uiresources.h"

#include "dtEditQt/terrainbrowser.h"

namespace dtEditQt
{
    ///////////////////////////////////////////////////////////////////////////////
    TerrainBrowser::TerrainBrowser(dtDAL::DataType &type,QWidget *parent)
        : ResourceAbstractBrowser(&type,parent)
    {
        // This sets our resource icon that is visible on leaf nodes
        resourceIcon = new QIcon();
        resourceIcon->addPixmap(QPixmap(UIResources::ICON_TERRAIN_RESOURCE.c_str()));
        ResourceAbstractBrowser::resourceIcon = *resourceIcon;

        // setup right mouse click context menu
        createActions();
        createContextMenu();

        connect(&EditorEvents::getInstance(),SIGNAL(currentMapChanged()),
            this,SLOT(selectionChanged()));

        QGridLayout *grid = new QGridLayout(this);
        grid->addWidget(listGroup(), 0, 0);
        grid->addWidget(standardButtons(QString("Resource Tools")), 1, 0, Qt::AlignCenter);
    }
    ///////////////////////////////////////////////////////////////////////////////
    TerrainBrowser::~TerrainBrowser(){}
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *TerrainBrowser::listGroup()
    {
        QGroupBox *group = new QGroupBox(this);
        QGridLayout *grid = new QGridLayout(group);

        grid->addWidget(tree,0,0);

        return group;
    }
    ///////////////////////////////////////////////////////////////////////////////
    // Keyboard Event filter
    ///////////////////////////////////////////////////////////////////////////////
    bool TerrainBrowser::eventFilter(QObject *obj, QEvent *e)
    {
        if (obj == tree)
        {
            //For some reason, KeyPress is getting defined by something...
            //Without this undef, it will not compile under Linux..
            //It would be great if someone could figure out exactly what's
            //going on.
#undef KeyPress
            if (e->type() == QEvent::KeyPress)
            {
                QKeyEvent *keyEvent = (QKeyEvent *)e;
                switch(keyEvent->key())
                {
                case Qt::Key_Return :
                    if(selection->isResource())
                    {
                        selectionChanged();
                    }
                    break;
                case Qt::Key_Enter:
                    if(selection->isResource())
                    {
                        selectionChanged();
                    }
                    break;
                default:
                    return tree->eventFilter(obj,e);
                }
            }
            else
            {
                // pass the event on to the parent class
                return tree->eventFilter(obj, e);
            }
        }
        return false;
    }
    ///////////////////////////////////////////////////////////////////////////////
    void TerrainBrowser::selectionChanged()
    {
        ResourceAbstractBrowser::selectionChanged();
    }
    ///////////////////////////////////////////////////////////////////////////////
}
