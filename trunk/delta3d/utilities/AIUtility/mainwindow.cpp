/* -*-c++-*-
 * Delta3D
 * Copyright 2009, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <dtQt/deltastepper.h>
#include <dtQt/projectcontextdialog.h>

#include <QtGui/QCloseEvent>
#include <QtGui/QDialog>
#include <QtCore/QSettings>

static const std::string ORG_NAME("delta3d.org");
static const std::string APP_NAME("AIUtility");
static const std::string PROJECT_CONTEXT_SETTING("ProjectContext");


//////////////////////////////////////////////
MainWindow::MainWindow(QWidget& mainWidget)
: mUi(new Ui::MainWindow)
, mCentralWidget(mainWidget)
{
   mUi->setupUi(this);

   setCentralWidget(&mCentralWidget);
   setWindowTitle(tr("AI Utility"));

   connect(mUi->mChangeContextAction, SIGNAL(triggered()), this, SLOT(ChangeProjectContext()));
}

//////////////////////////////////////////////
MainWindow::~MainWindow()
{
   delete mUi;
   mUi = NULL;
}

//////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent* e)
{
   //Disconnect the central widget because OSG wants to close it itself.
   mCentralWidget.setParent(NULL);
   QApplication::quit();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::ChangeProjectContext()
{
   dtQt::ProjectContextDialog dialog(this);

   if (dialog.exec() == QDialog::Accepted)
   {
      QSettings settings(ORG_NAME.c_str(), APP_NAME.c_str());

      emit ProjectContextChanged(dialog.getProjectPath().toStdString());

      settings.setValue(PROJECT_CONTEXT_SETTING.c_str(), dialog.getProjectPath());
      settings.sync();
   }
}


