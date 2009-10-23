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
 * William E. Johnson II
 */

#include <prefix/dtstageprefix-src.h>
#include <dtQt/projectcontextdialog.h>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QFileDialog>
#include <QtGui/QLabel>

#include <dtDAL/project.h>


namespace dtQt
{

   ProjectContextDialog::ProjectContextDialog(QWidget* parent) : QDialog(parent)
   {
      QVBoxLayout* vLay   = new QVBoxLayout(this);
      //vLay->setSizeConstraint(QLayout::SetMinimumSize);

      setWindowTitle(tr("Select a project context"));

      QGroupBox*   group  = new QGroupBox(tr("Properties"),this);
      QGridLayout* grid   = new QGridLayout(group);
      QHBoxLayout* buttons= new QHBoxLayout;
      QPushButton* select = new QPushButton(tr("..."),group);
      QPushButton* cancel = new QPushButton(tr("Cancel"),this);
      pathEdit            = new QLineEdit(group);
      applyButton         = new QPushButton(tr("Apply"),this);

      QLabel* desc = new QLabel(tr("A project holds all related maps, files, and "
         "resources.  Please select a directory where your project files will be "
         "stored."), group);

      desc->setWordWrap(true);
      grid->addWidget(new QLabel(tr("Path:")), 0, 0);
      grid->addWidget(pathEdit,0,1);
      grid->addWidget(select,0,2);
      grid->addWidget(desc,1,0,1,3,Qt::AlignTop);
      vLay->addWidget(group);

      buttons->addStretch(1);
      buttons->addWidget(applyButton);
      buttons->addWidget(cancel);
      buttons->addStretch(1);

      vLay->addLayout(buttons);

      connect(select, SIGNAL(clicked()), this, SLOT(spawnFileBrowser()));
      connect(applyButton, SIGNAL(clicked()), this, SLOT(accept()));
      connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

      applyButton->setEnabled(false);
      pathEdit->setDisabled(true);
      pathEdit->setText(tr(dtDAL::Project::GetInstance().GetContext().c_str()));
      setMinimumSize(530, 100);
   }

   ProjectContextDialog::~ProjectContextDialog()
   {

   }

   QString ProjectContextDialog::getProjectPath() const
   {
      return pathEdit->text();
   }

   ///////////////////////// SLOTS ////////////////////////////////////
   void ProjectContextDialog::spawnFileBrowser()
   {
      QString dir = QFileDialog::getExistingDirectory(this, tr("Select a project context"));

      if (dir.isEmpty())
      {
         applyButton->setEnabled(false);
         return;
      }

      std::string strippedName = dir.toStdString();
      if ((*strippedName.rbegin()) == '\\' || (*strippedName.rbegin()) == '/')
      {
         strippedName = strippedName.substr(0, strippedName.size() - 1);
      }

      pathEdit->setText(tr(strippedName.c_str()));
      applyButton->setEnabled(true);
   }

}
