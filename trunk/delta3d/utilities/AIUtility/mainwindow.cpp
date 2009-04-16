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

#include <QtGui/QCloseEvent>

MainWindow::MainWindow(QWidget& mainWidget)
: mUi(new Ui::MainWindow)
, mCentralWidget(mainWidget)
{
   mUi->setupUi(this);
   mStepper.Start();

   setCentralWidget(&mCentralWidget);
   setWindowTitle(tr("AI Utility"));
}

MainWindow::~MainWindow()
{
   delete mUi;
   mUi = NULL;
}

void MainWindow::closeEvent(QCloseEvent* e)
{
   mStepper.Stop();
   //Disconnect the central widget because OSG wants to close it itself.
   mCentralWidget.setParent(NULL);
   QApplication::quit();
}

