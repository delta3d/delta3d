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

#include "dtEditQt/projectbrowser.h"

#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QGroupBox>
#include <QtGui/QTreeView>
#include <QtGui/QDirModel>
#include <QtGui/QHeaderView>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    ProjectBrowser::ProjectBrowser(QWidget *parent)
        :QWidget(parent)
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // Add refresh button to main box layout
        refreshButton = new QPushButton(tr("&Refresh"), this);
        connect(refreshButton, SIGNAL(clicked()), this, SLOT(refreshButtonClicked()));
        mainLayout->addWidget(refreshButton,0,Qt::AlignRight);

        // Add the grid layout to the remaining part of the box layout
        QGridLayout *grid = new QGridLayout();
        mainLayout->addLayout(grid);
        grid->addWidget(projectGroup(), 0, 0);
        //mainLayout->addWidget(projectGroup());

    }
    ///////////////////////////////////////////////////////////////////////////////
    ProjectBrowser::~ProjectBrowser(){}
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *ProjectBrowser::projectGroup()
    {
        // Tree View of current project which shows the current resources available
        QGroupBox *groupBox = new QGroupBox(tr("Browse Project"));

        //QDirModel *model = new QDirModel(QDir::root());
        QDirModel *model = new QDirModel();

        QTreeView *tree = new QTreeView;

        tree->setModel(model);
        tree->header()->hide();

        QVBoxLayout *vbox = new QVBoxLayout(groupBox);
        vbox->addWidget(tree);

        return groupBox;
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ProjectBrowser::refreshButtonClicked()
    {
        // do something interesting
    }
    ///////////////////////////////////////////////////////////////////////////////
}
