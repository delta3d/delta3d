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
 * Teague Coonan
 */
#include <prefix/stageprefix.h>
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
   ProjectBrowser::ProjectBrowser(QWidget* parent)
      : QWidget(parent)
   {
      QVBoxLayout* mainLayout = new QVBoxLayout(this);

      // Add refresh button to main box layout
      refreshButton = new QPushButton(tr("&Refresh"), this);
      connect(refreshButton, SIGNAL(clicked()), this, SLOT(refreshButtonClicked()));
      mainLayout->addWidget(refreshButton, 0, Qt::AlignRight);

      // Add the grid layout to the remaining part of the box layout
      QGridLayout* grid = new QGridLayout();
      mainLayout->addLayout(grid);
      grid->addWidget(projectGroup(), 0, 0);
      //mainLayout->addWidget(projectGroup());

   }

   ///////////////////////////////////////////////////////////////////////////////
   ProjectBrowser::~ProjectBrowser() {}

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* ProjectBrowser::projectGroup()
   {
      // Tree View of current project which shows the current resources available
      QGroupBox* groupBox = new QGroupBox(tr("Browse Project"));

      //QDirModel *model = new QDirModel(QDir::root());
      QDirModel* model = new QDirModel();

      QTreeView* tree = new QTreeView;

      tree->setModel(model);
      tree->header()->hide();

      QVBoxLayout* vbox = new QVBoxLayout(groupBox);
      vbox->addWidget(tree);

      return groupBox;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ProjectBrowser::refreshButtonClicked()
   {
      // do something interesting
   }
   ///////////////////////////////////////////////////////////////////////////////

} // namespace dtEditQt
