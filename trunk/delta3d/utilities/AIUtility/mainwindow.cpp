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
#include "aipropertyeditor.h"
#include "aiutilitypreferencesdialog.h"
#include "qtglframe.h"
#include "waypointbrowser.h"
#include "waypointselection.h"
#include "undocommands.h"

#include <ui_mainwindow.h>
#include <dtQt/deltastepper.h>
#include <dtQt/projectcontextdialog.h>
#include <dtQt/dialoglistselection.h>

#include <QtGui/QCloseEvent>
#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QMessageBox>
#include <QtGui/QUndoStack>
#include <QtGui/QDoubleSpinBox>
#include <QtCore/QSettings>
#include <dtCore/transform.h>
#include <dtDAL/map.h>
#include <dtDAL/project.h>
#include <dtDAL/propertycontainer.h>
#include <dtDAL/actorproperty.h>

#include <dtAI/aiplugininterface.h>
#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointrenderinfo.h>
#include <dtAI/waypointpropertycontainer.h>
#include <dtAI/waypointpropertycache.h>

#include <set>
#include <algorithm>

const std::string MainWindow::ORG_NAME("delta3d.org");
const std::string MainWindow::APP_NAME("AIUtility");
const std::string MainWindow::PROJECT_CONTEXT_SETTING("ProjectContext");
const std::string MainWindow::CURRENT_MAP_SETTING("CurrentMap");
const std::string MainWindow::WINDOW_SETTINGS("WindowSettings");

//////////////////////////////////////////////
MainWindow::MainWindow(QWidget& mainWidget)
: mUi(new Ui::MainWindow)
, mCentralWidget(mainWidget)
, mPropertyEditor(*new AIPropertyEditor(*this))
, mWaypointBrowser(NULL)
, mPluginInterface(NULL)
{
   mUi->setupUi(this);

   QtGLFrame* centerFrame = new QtGLFrame();
   QGridLayout* frameLayout = new QGridLayout();
   frameLayout->setContentsMargins(0, 0, 0, 0);
   frameLayout->setSpacing(0);
   centerFrame->setLayout(frameLayout);
   frameLayout->addWidget(&mCentralWidget);

   setCentralWidget(centerFrame);

   mCurrentCameraTransform.MakeIdentity();

   mUndoStack = new QUndoStack(this);
   mUi->undoView->setStack(mUndoStack);
   mUi->undoView->setCleanIcon(QIcon(":/images/ok.png"));
   connect(mUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(OnModifiedChanged()));
   connect(mUndoStack, SIGNAL(indexChanged(int)), this, SLOT(OnModifiedChanged()));

   QAction *undoAction = mUndoStack->createUndoAction(this);
   QAction *redoAction = mUndoStack->createRedoAction(this);
   undoAction->setIcon(QIcon(":/images/undo.png"));
   redoAction->setIcon(QIcon(":/images/redo.png"));

   mUi->menuEdit->addSeparator();
   mUi->menuEdit->addAction(undoAction);
   mUi->menuEdit->addAction(redoAction);

   mUi->toolBar->addSeparator();
   mUi->toolBar->addAction(undoAction);
   mUi->toolBar->addAction(redoAction);

   mWaypointBrowser = new WaypointBrowser(this);
   connect(mWaypointBrowser, SIGNAL(UndoCommandGenerated(QUndoCommand*)), this, SLOT(OnUndoCommandCreated(QUndoCommand*)));

   mUi->menuWindows->addAction(mPropertyEditor.toggleViewAction());
   mUi->menuWindows->addAction(mWaypointBrowser->toggleViewAction());
   mUi->menuWindows->addAction(mUi->undoStack->toggleViewAction());
   mUi->menuWindows->addAction(mUi->toolBar->toggleViewAction());

   QActionGroup* waypointSelectionModeGroup = new QActionGroup(this);
   waypointSelectionModeGroup->addAction(mUi->mActionSelectPointMode);
   waypointSelectionModeGroup->addAction(mUi->mActionSelectionBrushMode);

   mUi->selectModeToolBar->addAction(mUi->mActionSelectPointMode);
   mUi->selectModeToolBar->addAction(mUi->mActionSelectionBrushMode);

   //waypoint selection brush size
   QDoubleSpinBox* brushSize = new QDoubleSpinBox(this);
   brushSize->setToolTip(tr("Size of selection brush"));
   brushSize->setRange(0.1, 100.0);
   brushSize->setDecimals(2);
   brushSize->setSingleStep(1.0);
   connect(brushSize, SIGNAL(valueChanged(double)), this, SLOT(OnWaypointBrushSizeChanged(double)));
   brushSize->setValue(1.0);
   mUi->selectModeToolBar->addWidget(brushSize);

   connect(mUi->mActionOpenMap, SIGNAL(triggered()), this, SLOT(OnOpenMap()));
   connect(mUi->mActionCloseMap, SIGNAL(triggered()), this, SLOT(OnCloseMap()));
   connect(mUi->mActionSave, SIGNAL(triggered()), this, SLOT(OnSave()));
   connect(mUi->mChangeContextAction, SIGNAL(triggered()), this, SLOT(ChangeProjectContext()));
   connect(mUi->mActionRenderingOptions, SIGNAL(triggered()), this, SLOT(SelectRenderingOptions()));
   connect(mUi->mActionPreferences, SIGNAL(triggered()), this, SLOT(OnPreferences()));
   connect(mUi->mActionAddEdge, SIGNAL(triggered()), this, SLOT(OnAddEdge()));
   connect(mUi->mActionRemoveEdge, SIGNAL(triggered()), this, SLOT(OnRemoveEdge()));
   connect(mUi->mActionDeleteSelectedWaypoints, SIGNAL(triggered()), mWaypointBrowser, SLOT(OnDelete()));
   connect(mUi->mActionSelectAllWaypoints, SIGNAL(triggered()), this, SLOT(OnSelectAllWaypoints()));
   connect(mUi->mActionDeselectAllWaypoints, SIGNAL(triggered()), this, SLOT(OnDeselectAllWaypoints()));
   connect(mUi->mActionSelectInverseWaypoints, SIGNAL(triggered()), this, SLOT(OnSelectInverseWaypoints()));

   connect(mPropertyEditor.toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(OnPropertyEditorShowHide(bool)));
   connect(mWaypointBrowser->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(OnWaypointBrowserShowHide(bool)));

   connect(&mPropertyEditor, SIGNAL(SignalPropertyChangedFromControl(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)),
            this, SLOT(PropertyChangedFromControl(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)));

   connect(mWaypointBrowser, SIGNAL(RequestCameraTransformChange(const dtCore::Transform&)),
            this, SLOT(OnChildRequestCameraTransformChange(const dtCore::Transform&)));

   connect(&WaypointSelection::GetInstance(), SIGNAL(WaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>&)),
           this, SLOT(OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>&)));

   connect(mUi->mActionSelectPointMode, SIGNAL(triggered()), this, SLOT(OnSelectWaypointPointMode()));
   connect(mUi->mActionSelectionBrushMode, SIGNAL(triggered()), this, SLOT(OnSelectWaypontBrushMode()));

   addDockWidget(Qt::LeftDockWidgetArea, &mPropertyEditor);
   addDockWidget(Qt::RightDockWidgetArea, mWaypointBrowser);

   mPropertyEditor.installEventFilter(this);
   mWaypointBrowser->installEventFilter(this);
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

      restoreGeometry(settings.value(WINDOW_SETTINGS.c_str()).toByteArray());

      QString projectContext = settings.value(PROJECT_CONTEXT_SETTING.c_str()).toString();


      if (!projectContext.isEmpty())
      {
         emit ProjectContextChanged(projectContext.toStdString());
      }

      // The map could induce crashes so don't autoload.
      // We need to provide an alternative way to get around maps
      // that crash on load other than modifying this code or the registry.

      //if(dtDAL::Project::GetInstance().IsContextValid())
      //{
      //   ChangeMap(settings.value(CURRENT_MAP_SETTING.c_str()).toString());
      //}

      EnableOrDisableControls();
   }
}

//////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent* e)
{

   if (MaybeSave())
   {
      //Disconnect the central widget because OSG wants to close it itself.
      mCentralWidget.setParent(NULL);

      QSettings settings(ORG_NAME.c_str(), APP_NAME.c_str());
      settings.setValue(WINDOW_SETTINGS.c_str(), saveGeometry());
      settings.sync();
      e->accept();

      QApplication::quit();
   }
   else
   {
      e->ignore();
   }

}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnError(const std::string& message)
{
   QMessageBox::critical(this, "Error", tr(message.c_str()));
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
   if (MaybeSave() == false)
   {
      return;
   }

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
   mUndoStack->clear();
   OnModifiedChanged();

   EnableOrDisableControls();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnCloseMap()
{
   if (QMessageBox::question(this, tr("Close Map"), tr("Do you want to close the currently opened map?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
   {
      if (MaybeSave())
      {
         ChangeMap(tr(""));
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnSave()
{
   if (mPluginInterface != NULL)
   {
      // save the one that was last opened.  It would be nice to have a save as.
      if (mPluginInterface->SaveWaypointFile())
      {
         mUndoStack->setClean();  //purge the list of undo's
      }
      else
      {
         QMessageBox::critical(this, "Error Saving", "Saving the waypoint file failed for an unknown reason");
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnPreferences()
{
   AIUtilityPreferencesDialog dlg(this);
   if (dlg.exec()== QDialog::Accepted)
   {
      emit PreferencesUpdated();
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

   mWaypointBrowser->toggleViewAction()->setChecked(mWaypointBrowser->isVisible());
   mPropertyEditor.toggleViewAction()->setChecked(mPropertyEditor.isVisible());

   // Update Add/Remove Edge actions
   mUi->mActionAddEdge->setEnabled(false);
   mUi->mActionRemoveEdge->setEnabled(false);

   // Only allow the delete action when waypoint(s) are selected
   mUi->mActionDeleteSelectedWaypoints->setEnabled(WaypointSelection::GetInstance().GetNumberSelected() != 0);

   if (WaypointSelection::GetInstance().GetNumberSelected() == 2) // There must be exactly two waypoints selected
   {
      dtAI::WaypointInterface* waypointA = WaypointSelection::GetInstance().GetWaypointList()[0];
      dtAI::WaypointInterface* waypointB = WaypointSelection::GetInstance().GetWaypointList()[1];

      if (DoesEdgeExistBetweenWaypoints(waypointA, waypointB))
      {
         // Enable Remove Edge
         mUi->mActionRemoveEdge->setEnabled(true);
      }
      else
      {
         // Enable Add Edge
         mUi->mActionAddEdge->setEnabled(true);
      }
   }
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
   mWaypointBrowser->SetPluginInterface(interface);

   if (interface == NULL && !mCurrentMapName.isEmpty())
   {
      if (QMessageBox::question(this, tr("AI Interface Actor"), tr("No AI interface actor was found in the map that was opened.\n"
               "Would you like to add the default one and re-save the map?\n"
               "(No will close the map)"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
      {
         emit AddAIInterfaceToMap(mCurrentMapName.toStdString());
      }
      else
      {
         // Close the map.
         ChangeMap(tr(""));
      }
   }
}


////////////////////////////////////////////////////////////////////////////////
void MainWindow::SelectRenderingOptions()
{
   if (mPluginInterface != NULL)
   {
      //deselect any waypoints since changing the WaypointRenderInfo could conflict
      //with selected waypoint rendering.
      WaypointSelection::GetInstance().DeselectAllWaypoints();

      dtQt::BasePropertyEditor::PropertyContainerRefPtrVector selected;
      dtAI::WaypointRenderInfo& ri = mPluginInterface->GetDebugDrawable()->GetRenderInfo();

      selected.push_back(&ri);
      mPropertyEditor.HandlePropertyContainersSelected(selected);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnAddEdge()
{
   if(WaypointSelection::GetInstance().GetNumberSelected() == 2)
   {
      // Update NavMesh
      dtAI::WaypointInterface* waypointA = WaypointSelection::GetInstance().GetWaypointList()[0];
      dtAI::WaypointInterface* waypointB = WaypointSelection::GetInstance().GetWaypointList()[1];
      mPluginInterface->AddEdge(waypointA->GetID(), waypointB->GetID());

      // Update UI
      mPluginInterface->GetDebugDrawable()->AddEdge(waypointA, waypointB);

      //Undo'ing the AddEdge doesn't always remove the geometry from the AIDebugDrawable, for some reason
      //OnUndoCommandCreated(new AddEdgeCommand(*waypointA, *waypointB, mPluginInterface));

      EnableOrDisableControls();
   }
   else
   {
      LOG_ERROR("Trying to add edge with too many or too few waypoints selected.");
   }

}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnRemoveEdge()
{
   if(WaypointSelection::GetInstance().GetNumberSelected() == 2)
   {
      // Update NavMesh
      dtAI::WaypointInterface* waypointA = WaypointSelection::GetInstance().GetWaypointList()[0];
      dtAI::WaypointInterface* waypointB = WaypointSelection::GetInstance().GetWaypointList()[1];

      if (mPluginInterface->RemoveEdge(waypointA->GetID(), waypointB->GetID()))
      {
         // Update UI
         mPluginInterface->GetDebugDrawable()->RemoveEdge(waypointA, waypointB);
         OnUndoCommandCreated(new RemoveEdgeCommand(*waypointA, *waypointB, mPluginInterface));
         EnableOrDisableControls();
      }
      else
      {
         LOG_ERROR("Could not remove edge");
      }
   }
   else
   {
      LOG_ERROR("Trying to remove edge with too many or too few waypoints selected.");
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

   dtAI::WaypointPropertyBase *wpb = dynamic_cast<dtAI::WaypointPropertyBase*>(&pc);
   if (wpb)
   {
      if (wpb->Get())
      {
         //adjust the graphical representation of the waypoint to match the new property changes
         mPluginInterface->GetDebugDrawable()->InsertWaypoint(*wpb->Get());
      }

      OnModifiedChanged();
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnCameraTransformChanged(const dtCore::Transform& xform)
{
   mCurrentCameraTransform = xform;
   mWaypointBrowser->SetCameraTransform(xform);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnChildRequestCameraTransformChange(const dtCore::Transform& xform)
{
   emit RequestCameraTransformChange(xform);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>& selectedWaypoints)
{
   if (mPluginInterface == NULL)
   {
      return;
   }

   const osg::Vec4 kSelectedColor(1.f, 0.1f, 0.1f, 1.f);
   mPluginInterface->GetDebugDrawable()->ResetWaypointColorsToDefault();

   if (mPropertyEditor.isVisible())
   {
      RefreshPropertyEditor(selectedWaypoints);
   }

   for (size_t i = 0; i < selectedWaypoints.size(); ++i)
   {
      dtAI::WaypointInterface* wpi = selectedWaypoints[i];

      if (wpi)
      {
         mPluginInterface->GetDebugDrawable()->SetWaypointColor(*wpi, kSelectedColor);
      }
   }

   EnableOrDisableControls();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnPropertyEditorShowHide(bool checked)
{
   mPropertyEditor.setVisible(checked);

   if (mPropertyEditor.isVisible())
   {
      RefreshPropertyEditor(WaypointSelection::GetInstance().GetWaypointList());
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::RefreshPropertyEditor(std::vector<dtAI::WaypointInterface*>& selectedWaypoints)
{
   std::vector<dtCore::RefPtr<dtDAL::PropertyContainer> > propertyContainers;
   propertyContainers.reserve(selectedWaypoints.size());

   for (size_t i = 0; i < selectedWaypoints.size(); ++i)
   {
      dtAI::WaypointInterface* wpi = selectedWaypoints[i];

      if (wpi)
      {
         propertyContainers.push_back(mPluginInterface->CreateWaypointPropertyContainer(wpi->GetWaypointType(), wpi));
      }
   }

   mPropertyEditor.HandlePropertyContainersSelected(propertyContainers);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnWaypointBrowserShowHide(bool checked)
{
   mWaypointBrowser->setVisible(checked);

   if (mWaypointBrowser->isVisible())
   {
      mWaypointBrowser->OnWaypointSelectionChanged(WaypointSelection::GetInstance().GetWaypointList());
   }
}

////////////////////////////////////////////////////////////////////////////////
bool MainWindow::DoesEdgeExistBetweenWaypoints(dtAI::WaypointInterface* waypointStart,
   dtAI::WaypointInterface* waypointEnd)
{
   if (mPluginInterface == NULL)
   {
      return false;
   }

   dtAI::AIPluginInterface::ConstWaypointArray edgeList;
   mPluginInterface->GetAllEdgesFromWaypoint(waypointStart->GetID(), edgeList);
   for (size_t edgeIndex = 0; edgeIndex < edgeList.size(); ++edgeIndex)
   {
      if (edgeList[edgeIndex] == waypointEnd)
      {
         return true;
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnModifiedChanged()
{
   mUi->mActionSave->setEnabled(!mUndoStack->isClean());
   this->setWindowModified(!mUndoStack->isClean());
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnSelectAllWaypoints()
{
   if (mPluginInterface == NULL)
   {
      return;
   }

   dtAI::AIPluginInterface::WaypointArray waypoints;
   mPluginInterface->GetWaypoints(waypoints);

   WaypointSelection::GetInstance().SetWaypointSelectionList(waypoints);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnDeselectAllWaypoints()
{
   WaypointSelection::GetInstance().DeselectAllWaypoints();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnSelectInverseWaypoints()
{
   //find the waypoints that aren't currently selected, and select them

   dtAI::AIPluginInterface::WaypointArray allWaypoints;
   mPluginInterface->GetWaypoints(allWaypoints);
   std::vector<dtAI::WaypointInterface*> selectedWaypoints = WaypointSelection::GetInstance().GetWaypointList();

   //containers must be sorted for set_difference to work
   std::sort(allWaypoints.begin(), allWaypoints.end());
   std::sort(selectedWaypoints.begin(), selectedWaypoints.end());


   std::vector<dtAI::WaypointInterface*>::iterator endItr;
   std::vector<dtAI::WaypointInterface*> v(allWaypoints.size()); //NULLs
   endItr = std::set_difference(allWaypoints.begin(), allWaypoints.end(),
                                selectedWaypoints.begin(), selectedWaypoints.end(), v.begin());

   std::vector<dtAI::WaypointInterface*> inverseSelected;

   std::vector<dtAI::WaypointInterface*>::iterator newlySelectedItr = v.begin();
   while (newlySelectedItr != endItr)
   {
      inverseSelected.push_back(*newlySelectedItr);
      ++newlySelectedItr;
   }

   WaypointSelection::GetInstance().SetWaypointSelectionList(inverseSelected);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnSelectWaypointPointMode()
{
   emit WaypointBrushSelectMode(false);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnSelectWaypontBrushMode()
{
   emit WaypointBrushSelectMode(true);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnWaypointBrushSizeChanged(double value)
{
   emit WaypointBrushSizeChanged(value);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnUndoCommandCreated(QUndoCommand* undoCommand)
{
   mUndoStack->push(undoCommand);
}

////////////////////////////////////////////////////////////////////////////////
bool MainWindow::MaybeSave()
{
   if (this->isWindowModified())
   {
      QMessageBox::StandardButton ret;
      ret = QMessageBox::warning(this, tr("Save"),
                  tr("The nav mesh has been modified.\n"
                  "Do you want to save your changes?"),
                  QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

      if (ret == QMessageBox::Save)
      {
         OnSave();
         return true;
      }
      else if (ret == QMessageBox::Cancel)
      {
         return false;
      }
   }

   return true;
}
