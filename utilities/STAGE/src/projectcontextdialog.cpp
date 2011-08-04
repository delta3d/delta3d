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
 * William E. Johnson II
 */
#include <prefix/stageprefix.h>
#include "dtEditQt/projectcontextdialog.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QCloseEvent>
#include <QtGui/QLabel>

#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/viewportmanager.h"
#include "dtCore/project.h"
#include "dtCore/map.h"

#include <osgDB/FileNameUtils>

namespace dtEditQt
{

   ProjectContextDialog::ProjectContextDialog(QWidget* parent)
      : QDialog(parent)
   {
      QVBoxLayout* vLay   = new QVBoxLayout(this);
      //vLay->setSizeConstraint(QLayout::SetMinimumSize);

      setWindowTitle(tr("Select a project context"));

      QGroupBox*   group  = new QGroupBox(tr("Properties"),this);
      QGridLayout* grid   = new QGridLayout(group);
      QHBoxLayout* buttons= new QHBoxLayout;
      QPushButton* select = new QPushButton(tr("..."),group);
      QPushButton* cancel = new QPushButton(tr("Cancel"),this);
      mPathEdit           = new QLineEdit(group);
      mApplyButton        = new QPushButton(tr("Apply"),this);

      QLabel*      desc   = new QLabel(tr("A project holds all related maps, files, and "
                                          "resources.  Please select a directory where your project files will be "
                                          "stored."),group);

      desc->setWordWrap(true);
      grid->addWidget(new QLabel(tr("Path:")), 0, 0);
      grid->addWidget(mPathEdit, 0, 1);
      grid->addWidget(select, 0, 2);
      grid->addWidget(desc, 1, 0, 1, 3, Qt::AlignTop);
      vLay->addWidget(group);

      buttons->addStretch(1);
      buttons->addWidget(mApplyButton);
      buttons->addWidget(cancel);
      buttons->addStretch(1);

      vLay->addLayout(buttons);

      connect(select,       SIGNAL(clicked()), this, SLOT(spawnFileBrowser()));
      connect(mApplyButton, SIGNAL(clicked()), this, SLOT(accept()));
      connect(cancel,       SIGNAL(clicked()), this, SLOT(reject()));

      mApplyButton->setEnabled(false);
      mPathEdit->setDisabled(true);
      mPathEdit->setText(tr(EditorData::GetInstance().getCurrentProjectContext().c_str()));
      setMinimumSize(530, 100);
   }

   ProjectContextDialog::~ProjectContextDialog()
   {

   }

   QString ProjectContextDialog::getProjectPath() const
   {
      return mPathEdit->text();
   }

   ///////////////////////// SLOTS ////////////////////////////////////
   void ProjectContextDialog::spawnFileBrowser()
   {
      const std::string currentContext = EditorData::GetInstance().getCurrentProjectContext();

      QString dir = QFileDialog::getExistingDirectory(this,
                                  tr("Select a project context"),
                                  QString::fromStdString(currentContext));

      if (dir.isEmpty())
      {
         mApplyButton->setEnabled(false);
         return;
      }

      std::string strippedName = dir.toStdString();
      if ((*strippedName.rbegin()) == '\\' || (*strippedName.rbegin()) == '/')
      {
         strippedName = strippedName.substr(0, strippedName.size() - 1);
      }

      mPathEdit->setText(tr(strippedName.c_str()));
      mApplyButton->setEnabled(true);
   }

} // namespace dtEditQt
