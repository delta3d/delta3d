/* -*-c++-*-
 * Delta3D
 * Copyright 2007-2008, Alion Science and Technology
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
 * david
 */

#include "waypointbrowser.h"
#include "waypointselection.h"
#include "waypointbrowsertreeitem.h"
#include "objecttypetreewidget.h"
#include "mainwindow.h"

#include <ui_waypointbrowser.h>
#include <dtAI/aiplugininterface.h>
#include <dtUtil/log.h>

#include <QtGui/QDoubleValidator>
#include <QtGui/QScrollBar>
#include <QtCore/QSettings>

///////////////////////////////////////////////////
WaypointBrowser::WaypointBrowser(QWidget& parent)
: QDockWidget(&parent)
, mAIPluginInterface(NULL)
{
   mUi = new Ui::WaypointBrowser();
   mUi->setupUi(this);

   connect(mUi->mBtnCreateWaypoint, SIGNAL(pressed()), this, SLOT(OnCreate()));
   connect(mUi->mBtnDeleteWaypoint, SIGNAL(pressed()), this, SLOT(OnDelete()));
   connect(mUi->mBtnGotoWaypoint, SIGNAL(pressed()), this, SLOT(OnGoto()));
   connect(mUi->mBtnSearchRefresh, SIGNAL(pressed()), this, SLOT(ResetWaypointResult()));

   connect(mUi->mWaypointList, SIGNAL(itemSelectionChanged()), this, SLOT(WaypointsSelectedFromBrowser()));
   connect(mUi->mObjectTypeTree, SIGNAL(itemSelectionChanged()), this, SLOT(EnableDisable()));

   connect(&WaypointSelection::GetInstance(), SIGNAL(WaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>&)),
           this, SLOT(OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>&)));

   QDoubleValidator* validator = new QDoubleValidator(mUi->mDistanceEdit);
   validator->setDecimals(8);
   mUi->mDistanceEdit->setValidator(validator);

   EnableDisable();
}

///////////////////////////////////////////////////
WaypointBrowser::~WaypointBrowser()
{
   delete mUi;
   mUi = NULL;
}

///////////////////////////////////////////////////
void WaypointBrowser::SetPluginInterface(dtAI::AIPluginInterface* plugin)
{
   mAIPluginInterface = plugin;

   ResetTypesTree();
   ResetWaypointResult();

   EnableDisable();
}

//////////////////////////////////////////////////
void WaypointBrowser::ResetTypesTree()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   dtUtil::tree<QString> storageTree;
   int scrollbar;

   MarkCurrentExpansion(*mUi->mObjectTypeTree, storageTree, scrollbar);

   mUi->mObjectTypeTree->clear();

   if (mAIPluginInterface != NULL)
   {
      std::vector<dtCore::RefPtr<const dtDAL::ObjectType> > objectTypes;
      mAIPluginInterface->GetSupportedWaypointTypes(objectTypes);

      ObjectTypeTreeWidget* root = new ObjectTypeTreeWidget(mUi->mObjectTypeTree, tr("Waypoint Types"));
      // iterate through the actor types and create all the internal nodes.
      for (unsigned int i = 0; i < objectTypes.size(); ++i)
      {
         QString fullCategory(tr(objectTypes[i]->GetCategory().c_str()));

         if (!fullCategory.isEmpty())
         {
            QStringList subCategories = fullCategory.split(tr(ObjectTypeTreeWidget::CATEGORY_SEPARATOR.c_str()),
                     QString::SkipEmptyParts);
            QMutableStringListIterator* listIterator = new QMutableStringListIterator(subCategories);
            root->RecursivelyAddCategoryAndObjectTypeAsChildren(listIterator, objectTypes[i]);
            delete listIterator;
         }
      }

      RestorePreviousExpansion(*mUi->mObjectTypeTree, storageTree, scrollbar);
   }
   QApplication::restoreOverrideCursor();

}
/////////////////////////////////////////////////////////////////////////////////
void WaypointBrowser::MarkCurrentExpansion(QTreeWidget& tree, dtUtil::tree<QString>& storageTree, int& scrollBarLocation)
{
   if (tree.children().size() > 0)
   {
      // start recursion
      RecurseMarkCurrentExpansion(tree, tree.invisibleRootItem(), storageTree);

      // also store the last location of the scroll bar... so that they go back
      // to where they were next time.
      scrollBarLocation = tree.verticalScrollBar()->sliderPosition();
   }
}

/////////////////////////////////////////////////////////////////////////////////
void WaypointBrowser::RecurseMarkCurrentExpansion(QTreeWidget& tree, QTreeWidgetItem* parent,
         dtUtil::tree<QString>& storageTree)
{
   for (int i = 0; i < parent->childCount(); ++i)
   {
      QTreeWidgetItem* child = parent->child(i);

      // if we have children, then we could potentially be expanded...
      if (child != NULL && child->childCount() > 0)
      {
         if (tree.isItemExpanded(child))
         {
            // add it to our list
            dtUtil::tree<QString>& insertedItem = storageTree.insert(child->text(0)).tree_ref();

            // recurse on the child with the new tree
            RecurseMarkCurrentExpansion(tree, child, insertedItem);
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////
void WaypointBrowser::RestorePreviousExpansion(QTreeWidget& tree, const dtUtil::tree<QString>& storageTree, int scrollBarLocation)
{
   if (tree.children().size() > 0)
   {
      RecurseRestorePreviousExpansion(tree, tree.invisibleRootItem(), storageTree);

      // Put the scroll bar back where it was last time
      tree.verticalScrollBar()->setSliderPosition(scrollBarLocation);
   }
}

/////////////////////////////////////////////////////////////////////////////////
void WaypointBrowser::RecurseRestorePreviousExpansion(QTreeWidget& tree, QTreeWidgetItem* parent,
   const dtUtil::tree<QString>& currentTree)
{
   // walk through the children...
   for (dtUtil::tree<QString>::const_iterator iter = currentTree.in(); iter != currentTree.end(); ++iter)
   {
      QString name = (*iter);

      // Try to find a control with this name in our model
      for (int i = 0; i < parent->childCount(); ++i)
      {
         QTreeWidgetItem* child = parent->child(i);
         // found a match!  expand it
         if (child->text(0) == name)
         {
            tree.expandItem(child);

            // recurse over the children of this object
            RecurseRestorePreviousExpansion(tree, child, iter.tree_ref());
         }
      }
   }
}

///////////////////////////////////////////////////
void WaypointBrowser::ResetWaypointResult()
{
   QSettings settings(MainWindow::ORG_NAME.c_str(), MainWindow::APP_NAME.c_str());
//   settings.setValue(CURRENT_MAP_SETTING.c_str(), mCurrentMapName);
//   settings.sync();

   mUi->mWaypointList->clear();

   if (mAIPluginInterface != NULL)
   {
      osg::Vec3 pos;
      mCameraTransform.GetTranslation(pos);
      dtAI::AIPluginInterface::WaypointArray waypoints;
      QString text = mUi->mSearchRange->text();
      bool ok = false;
      double range = text.toDouble(&ok);

      if (ok && range > 0.0)
      {
         if (range > 0.0)
         {
            mAIPluginInterface->GetWaypointsAtRadius(pos, float(range), waypoints);
         }

         dtAI::AIPluginInterface::WaypointArray::iterator i, iend;
         i = waypoints.begin();
         iend = waypoints.end();
         for (; i != iend; ++i)
         {
            dtAI::WaypointInterface* wpi = *i;
            new WaypointBrowserTreeItem(mUi->mWaypointList->invisibleRootItem(), *wpi);
         }
      }
   }

   // update the selection to take into account selected waypoints that were not previously shown
   OnWaypointSelectionChanged(WaypointSelection::GetInstance().GetWaypointList());
}

///////////////////////////////////////////////////
void WaypointBrowser::OnCreate()
{
   dtCore::RefPtr<const dtDAL::ObjectType> objectTypeSelected;

   QList<QTreeWidgetItem*> list = mUi->mObjectTypeTree->selectedItems();

   ObjectTypeTreeWidget* selectedItem = NULL;
   if (!list.isEmpty())
   {
      selectedItem = dynamic_cast<ObjectTypeTreeWidget*>(list[0]);
   }

   if (selectedItem != NULL && selectedItem->IsLeafNode())
   {
      objectTypeSelected = selectedItem->GetObjectType();
   }

   if (objectTypeSelected.valid())
   {

      osg::Vec3 forward, pos;
      mCameraTransform.GetTranslation(pos);
      mCameraTransform.GetRow(1, forward);

      QString distanceText = mUi->mDistanceEdit->text();

      pos += forward * GetCreateAndGotoDistance();

      mAIPluginInterface->CreateWaypoint(pos, *objectTypeSelected);
      ResetWaypointResult();
   }
}

///////////////////////////////////////////////////
void WaypointBrowser::OnDelete()
{
   const QList<QTreeWidgetItem*>& list = mUi->mWaypointList->selectedItems();

   bool deletedSomething = false;
   QList<QTreeWidgetItem*>::const_iterator i, iend;
   i = list.begin();
   iend = list.end();
   for (; i != iend; ++i)
   {
      WaypointBrowserTreeItem* item = dynamic_cast<WaypointBrowserTreeItem*>(*i);
      if (item != NULL && item->GetWaypoint() != NULL)
      {
         // Deselect the waypoint first
         WaypointSelection::GetInstance().DeselectWaypoint(item->GetWaypoint());

         mAIPluginInterface->RemoveWaypoint(item->GetWaypoint());
         deletedSomething = true;
      }
   }

   if (deletedSomething)
   {
      ResetWaypointResult();
   }
}

///////////////////////////////////////////////////
void WaypointBrowser::OnGoto()
{
   const QList<QTreeWidgetItem*>& list = mUi->mWaypointList->selectedItems();
   if (!list.isEmpty())
   {
      WaypointBrowserTreeItem* item = dynamic_cast<WaypointBrowserTreeItem*>(list[0]);
      if (item != NULL)
      {
         osg::Vec3 pos = item->GetWaypoint()->GetPosition();
         dtCore::Transform xform = mCameraTransform;
         osg::Vec3 forward;
         xform.GetRow(1, forward);

         pos -= forward * GetCreateAndGotoDistance();

         xform.SetTranslation(pos);
         emit RequestCameraTransformChange(xform);
      }
   }
}

///////////////////////////////////////////////////
void WaypointBrowser::GetCameraTransform(dtCore::Transform& xform)
{
   xform = mCameraTransform;
}

///////////////////////////////////////////////////
void WaypointBrowser::SetCameraTransform(const dtCore::Transform& xform)
{
   mCameraTransform = xform;
}

///////////////////////////////////////////////////
void WaypointBrowser::EnableDisable()
{
   bool waypointSelected = !mUi->mWaypointList->selectedItems().isEmpty();
   mUi->mBtnDeleteWaypoint->setEnabled(waypointSelected);
   mUi->mBtnGotoWaypoint->setEnabled(waypointSelected);

   QList<QTreeWidgetItem*> list = mUi->mObjectTypeTree->selectedItems();

   ObjectTypeTreeWidget* selectedItem = NULL;
   if (!list.isEmpty())
   {
      selectedItem = dynamic_cast<ObjectTypeTreeWidget*>(list[0]);
   }
   mUi->mBtnCreateWaypoint->setEnabled(selectedItem != NULL && selectedItem->IsLeafNode());
}

///////////////////////////////////////////////////
void WaypointBrowser::WaypointsSelectedFromBrowser()
{
   // Prevent an infinite loop
   disconnect(&WaypointSelection::GetInstance(), SIGNAL(WaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>&)),
      this, SLOT(OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>&)));

   WaypointSelection::GetInstance().DeselectAllWaypoints();

   QList<QTreeWidgetItem*> list = mUi->mWaypointList->selectedItems();

   QList<QTreeWidgetItem*>::const_iterator i, iend;
   i = list.begin();
   iend = list.end();
   for (; i != iend; ++i)
   {
      WaypointBrowserTreeItem* item = dynamic_cast<WaypointBrowserTreeItem*>(*i);
      if (item != NULL && item->GetWaypoint() != NULL)
      {
         // It would be more efficient to pass this as a whole list rather than 
         // one at a time due to all the listeners who process in response
         if (!WaypointSelection::GetInstance().HasWaypoint(item->GetWaypoint()))
         {
            WaypointSelection::GetInstance().AddWaypointToSelection(item->GetWaypoint());            
         }
      }
   }

   EnableDisable();

   // resubscribe
   connect(&WaypointSelection::GetInstance(), SIGNAL(WaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>&)),
           this, SLOT(OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>&)));  
}

////////////////////////////////////////////////////////////////////////////////
void WaypointBrowser::OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>& selectedWaypoints)
{
   // Prevent loops
   disconnect(mUi->mWaypointList, SIGNAL(itemSelectionChanged()), this, SLOT(WaypointsSelectedFromBrowser()));

   // Redo them all unless there is a perf reason not too
   mUi->mWaypointList->clearSelection();

   for (size_t waypointIndex = 0; waypointIndex < selectedWaypoints.size(); ++waypointIndex)
   {
      QString id = QString("%1").arg(selectedWaypoints[waypointIndex]->GetID());
      QList<QTreeWidgetItem*> itemList = mUi->mWaypointList->findItems(id, Qt::MatchExactly);

      if (itemList.count() == 1)
      {
         mUi->mWaypointList->setItemSelected(itemList.front(), true);
      }
      else
      {
         LOG_ERROR("Duplicate waypoint ID's exist in the browser!");
      }
   }

   EnableDisable();
   
   connect(mUi->mWaypointList, SIGNAL(itemSelectionChanged()), this, SLOT(WaypointsSelectedFromBrowser()));
}

///////////////////////////////////////////////////
float WaypointBrowser::GetCreateAndGotoDistance() const
{
   QString distanceText = mUi->mDistanceEdit->text();

   bool okay;
   float distance = distanceText.toFloat(&okay);
   if (!okay)
   {
      distance = 5.0;
   }
   return distance;
}

