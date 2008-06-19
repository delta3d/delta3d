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
*/

#include <ResourceDock.h>
#include <QtGui/QAction>
#include <QtGui/QTabWidget>
#include <QtGui/QTreeWidget>

#include <algorithm>
#include <cctype>

///////////////////////////////////////////////////////////////////////////////
ResourceDock::ResourceDock()
  : QDockWidget()
  , mTabs(NULL)
{
   setWindowTitle(tr("Resources"));
   setMouseTracking(true);
  
   mGeometryTreeWidget = new QTreeWidget(this);
   mShaderTreeWidget = new QTreeWidget(this);   

   mGeometryTreeWidget->headerItem()->setText(0, "");
   mShaderTreeWidget->headerItem()->setText(0, "");

   connect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), 
           this, SLOT(OnShaderItemChanged(QTreeWidgetItem *, int))); 

   connect(mGeometryTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), 
           this, SLOT(OnGeometryItemChanged(QTreeWidgetItem *, int))); 

   mTabs = new QTabWidget;
   mTabs->addTab(mGeometryTreeWidget, tr("Geometry"));
   mTabs->addTab(mShaderTreeWidget, tr("Shaders"));

   setWidget(mTabs);  
}

///////////////////////////////////////////////////////////////////////////////
ResourceDock::~ResourceDock(){}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::SetGeometry(const std::string &fullName, bool shouldDisplay) const
{  
   QTreeWidgetItem *geometryItem = FindGeometryItem(fullName);
   
   if (geometryItem)
   {
      SetGeometry(geometryItem, shouldDisplay);
   }   
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::SetGeometry(QTreeWidgetItem *geometryItem, bool shouldDisplay) const
{
   std::string fullName = geometryItem->toolTip(0).toStdString();
   geometryItem->setCheckState(0, (shouldDisplay) ? Qt::Checked : Qt::Unchecked);
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnNewShader(const std::string &shaderGroup, const std::string &shaderProgram)
{
   //// We don't want this signal emitted when we're adding a shader
   disconnect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), 
              this, SLOT(OnShaderItemChanged(QTreeWidgetItem *, int)));

   QTreeWidgetItem *shaderItem = FindShaderGroupItem(shaderGroup);
   QTreeWidgetItem *programItem = new QTreeWidgetItem();

   // If the group doesn't exist, create a new one
   if (shaderItem == NULL)
   {
      shaderItem = new QTreeWidgetItem;
      mShaderTreeWidget->addTopLevelItem(shaderItem); 
   }

   shaderItem->setText(0, shaderGroup.c_str());
   programItem->setText(0, shaderProgram.c_str());

   shaderItem->setFlags(Qt::ItemIsSelectable |
                        Qt::ItemIsUserCheckable |
                        Qt::ItemIsEnabled);

   // The shader itself should have a checkbox
   programItem->setCheckState(0, Qt::Unchecked);
   
   shaderItem->addChild(programItem);

   connect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), 
           this, SLOT(OnShaderItemChanged(QTreeWidgetItem *, int)));
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnShaderItemChanged(QTreeWidgetItem *item, int column)
{ 
   if (column == 0)
   {
      QString programName = item->text(0);
      QString groupName   = item->parent()->text(0);

      if (item->checkState(0) == Qt::Checked)
      {
         emit ApplyShader(groupName.toStdString(), programName.toStdString());         
      }
      else if (item->checkState(0) == Qt::Unchecked)
      {
         emit RemoveShader();
      }
   }  
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnNewGeometry(const std::string &path, const std::string &filename)
{
   QTreeWidgetItem *geometryItem = new QTreeWidgetItem();
   geometryItem->setText(0, filename.c_str());
   geometryItem->setToolTip(0, (path + "/" + filename).c_str());

   geometryItem->setFlags(Qt::ItemIsSelectable |
                          Qt::ItemIsUserCheckable |
                          Qt::ItemIsEnabled);
   
   geometryItem->setCheckState(0, Qt::Unchecked);

   mGeometryTreeWidget->addTopLevelItem(geometryItem); 
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnGeometryItemChanged(QTreeWidgetItem *item, int column)
{
   if (column == 0)
   {
      // The full path is stored in the tooltip
      QString geomName = item->toolTip(0);

      if (item->checkState(0) == Qt::Checked)
      {
         emit LoadGeometry(geomName.toStdString());         
      }
      else if (item->checkState(0) == Qt::Unchecked)
      {
         emit UnloadGeometry();
      }
   }  
}

///////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::FindGeometryItem(const std::string &fullName) const
{
   for (int itemIndex = 0; itemIndex < mGeometryTreeWidget->topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem *childItem = mGeometryTreeWidget->topLevelItem(itemIndex);

      std::string currentPath = childItem->toolTip(0).toStdString();
      std::string findPath = fullName;

      std::transform(currentPath.begin(), currentPath.end(), currentPath.begin(), std::tolower);
      std::transform(findPath.begin(), findPath.end(), findPath.begin(), std::tolower);

      // Case insensitive comparison
      if (currentPath == findPath)
      {
         return childItem; 
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::FindShaderGroupItem(const std::string &name) const
{
   for (int itemIndex = 0; itemIndex < mShaderTreeWidget->topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem *childItem = mShaderTreeWidget->topLevelItem(itemIndex);
      
      if (name == childItem->text(0).toStdString())
      {
         return childItem; 
      }
   }

   return NULL;
}

