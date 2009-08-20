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
#include <dtQt/dialoglistselection.h>

#include <QtGui/QCloseEvent>
#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>

#include <dtDAL/map.h>
#include <dtDAL/project.h>
#include <dtDAL/propertycontainer.h>
#include <dtDAL/actorproperty.h>
#include "aipropertyeditor.h"

#include <dtAI/aiplugininterface.h>
#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointrenderinfo.h>

#include <set>

static const std::string ORG_NAME("delta3d.org");
static const std::string APP_NAME("AIUtility");
static const std::string PROJECT_CONTEXT_SETTING("ProjectContext");
static const std::string CURRENT_MAP_SETTING("CurrentMap");

//////////////////////////////////////////////
MainWindow::MainWindow(QWidget& mainWidget)
   : mUi(new Ui::MainWindow)
   , mCentralWidget(mainWidget)
   , mPropertyEditor(*new AIPropertyEditor(*this))
   , mPluginInterface(NULL)
{
   mUi->setupUi(this);

   setCentralWidget(&mCentralWidget);
   setWindowTitle(tr("AI Utility"));

   connect(mUi->mActionOpenMap, SIGNAL(triggered()), this, SLOT(OnOpenMap()));
   connect(mUi->mActionCloseMap, SIGNAL(triggered()), this, SLOT(OnCloseMap()));
   connect(mUi->mChangeContextAction, SIGNAL(triggered()), this, SLOT(ChangeProjectContext()));
   connect(mUi->mActionRenderingOptions, SIGNAL(triggered()), this, SLOT(SelectRenderingOptions()));

   connect(&mPropertyEditor, SIGNAL(SignalPropertyChangedFromControl(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)),
            this, SLOT(PropertyChangedFromControl(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)));

   addDockWidget(Qt::LeftDockWidgetArea, &mPropertyEditor);
}

//////////////////////////////////////////////
MainWindow::~MainWindow()
{
   delete mUi;
   mUi = NULL;
}
//////////////////////////////////////////////
void MainWindow::showEvent(QShowEvent* e)
{
   if (!e->spontaneous())
   {
      QSettings settings(ORG_NAME.c_str(), APP_NAME.c_str());
      QString projectContext = settings.value(PROJECT_CONTEXT_SETTING.c_str()).toString();


      if (!projectContext.isEmpty())
      {
         emit ProjectContextChanged(projectContext.toStdString());
      }

      ChangeMap(settings.value(CURRENT_MAP_SETTING.c_str()).toString());

      EnableOrDisableControls();
   }
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
      EnableOrDisableControls();
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnOpenMap()
{
   dtQt::DialogListSelection openMapDialog(this, tr("Open Existing Map"), tr("Available Maps"));

   QStringList listItems;
   const std::set<std::string>& mapNames = dtDAL::Project::GetInstance().GetMapNames();
   for (std::set<std::string>::const_iterator i = mapNames.begin(); i != mapNames.end(); ++i)
   {
      listItems << i->c_str();
   }

   openMapDialog.SetListItems(listItems);
   if (openMapDialog.exec() == QDialog::Accepted)
   {
      ChangeMap(openMapDialog.GetSelectedItem());
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::ChangeMap(const QString& newMap)
{
   QApplication::setOverrideCursor(Qt::WaitCursor);
   if (newMap.isEmpty())
   {
      emit CloseMapSelected();
   }
   else
   {
      emit MapSelected(newMap.toStdString());
   }
   QApplication::restoreOverrideCursor();

   mCurrentMapName = newMap;
   QSettings settings(ORG_NAME.c_str(), APP_NAME.c_str());
   settings.setValue(CURRENT_MAP_SETTING.c_str(), mCurrentMapName);
   settings.sync();
   EnableOrDisableControls();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnCloseMap()
{
   if (QMessageBox::question(this, tr("Close Map"), tr("Do you want to close the currently opened map?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
   {
      ChangeMap(tr(""));
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::EnableOrDisableControls()
{
   mUi->mActionOpenMap->setEnabled(!dtDAL::Project::GetInstance().GetContext().empty());
   mUi->mActionCloseMap->setEnabled(!mCurrentMapName.isEmpty());
   // Stop from changing context unless the map is closed. It works around a bug.
   // since the map doesn't change immediately in the GM, we can't just change maps.
   mUi->mChangeContextAction->setEnabled(mCurrentMapName.isEmpty());
}

////////////////////////////////////////////////////////////////////////////////
dtAI::AIPluginInterface* MainWindow::GetAIPluginInterface()
{
   return mPluginInterface;
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::SetAIPluginInterface(dtAI::AIPluginInterface* interface)
{
   mPluginInterface = interface;
   EnableOrDisableControls();
   // clear the selection
   dtQt::BasePropertyEditor::PropertyContainerRefPtrVector selected;
   mPropertyEditor.HandlePropertyContainersSelected(selected);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::SelectRenderingOptions()
{
   if (mPluginInterface != NULL)
   {
      dtQt::BasePropertyEditor::PropertyContainerRefPtrVector selected;
      dtAI::WaypointRenderInfo& ri = mPluginInterface->GetDebugDrawable()->GetRenderInfo();
      selected.push_back(&ri);
      mPropertyEditor.HandlePropertyContainersSelected(selected);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::PropertyChangedFromControl(dtDAL::PropertyContainer& pc, dtDAL::ActorProperty& ap)
{
   dtAI::WaypointRenderInfo& ri = mPluginInterface->GetDebugDrawable()->GetRenderInfo();
   if (&pc == &ri)
   {
      mPluginInterface->GetDebugDrawable()->OnRenderInfoChanged();
   }
}

