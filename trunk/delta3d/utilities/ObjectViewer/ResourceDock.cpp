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

#include "ResourceDock.h"
#include "ObjectViewerData.h"
#include "TextEdit.h"

#include <QtGui/QAction>
#include <QtGui/QTabWidget>
#include <QtGui/QColorDialog>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>

#include <QtCore/QDir>

#include <osg/LightSource>
#include <dtCore/globals.h>
#include <dtCore/shadermanager.h>

#include <dtCore/infinitelight.h>
#include <dtCore/positionallight.h>
#include <dtCore/spotlight.h>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
ShaderTree::ShaderTree(QWidget* parent)
   : QTreeWidget(parent)
{
   CreateContextActions();
   CreateContextMenus();
}

////////////////////////////////////////////////////////////////////////////////
ShaderTree::~ShaderTree()
{
}

////////////////////////////////////////////////////////////////////////////////
void ShaderTree::SetShaderSourceEnabled(bool vertexEnabled, bool fragmentEnabled)
{
   mOpenVertexSource->setEnabled(vertexEnabled);
   mOpenFragmentSource->setEnabled(fragmentEnabled);
}

////////////////////////////////////////////////////////////////////////////////
void ShaderTree::CreateContextActions()
{
   // Definition Actions
   mEditShaderDef = new QAction(tr("&Edit"), this);
   mEditShaderDef->setCheckable(false);
   mEditShaderDef->setStatusTip(tr("Edits the Shader Definition File (Not implemented yet)"));
   mEditShaderDef->setEnabled(false);
   connect(mEditShaderDef, SIGNAL(triggered()), parent(), SLOT(OnEditShaderDef()));

   mRemoveShaderDef = new QAction(tr("&Remove"), this);
   mRemoveShaderDef->setCheckable(false);
   mRemoveShaderDef->setStatusTip(tr("Removes the Shader Definition from the list"));
   connect(mRemoveShaderDef, SIGNAL(triggered()), parent(), SLOT(OnRemoveShaderDef()));

   // Program Actions
   mOpenVertexSource = new QAction(tr("&Open Vertex Source"), this);
   mOpenVertexSource->setCheckable(false);
   mOpenVertexSource->setStatusTip(tr("Open Current Vertex Shader"));
   connect(mOpenVertexSource, SIGNAL(triggered()), parent(), SLOT(OnOpenCurrentVertexShaderSources()));

   mOpenFragmentSource = new QAction(tr("&Open Fragment Source"), this);
   mOpenFragmentSource->setCheckable(false);
   mOpenFragmentSource->setStatusTip(tr("Open Current Fragment Shader"));
   connect(mOpenFragmentSource, SIGNAL(triggered()), parent(), SLOT(OnOpenCurrentFragmentShaderSources()));
}

////////////////////////////////////////////////////////////////////////////////
void ShaderTree::CreateContextMenus()
{
   // Definition Context
   mDefinitionContext = new QMenu(this);
   mDefinitionContext->addAction(mEditShaderDef);
   mDefinitionContext->addSeparator();
   mDefinitionContext->addAction(mRemoveShaderDef);

   // Program Context
   mProgramContext = new QMenu(this);
   mProgramContext->addAction(mOpenVertexSource);
   mProgramContext->addAction(mOpenFragmentSource);
}

////////////////////////////////////////////////////////////////////////////////
void ShaderTree::contextMenuEvent(QContextMenuEvent *contextEvent)
{
   QTreeWidgetItem* clickedItem = this->itemAt(contextEvent->pos());
   if (!clickedItem)
   {
      contextEvent->ignore();
      return;
   }

   // De-select all items.
   QList<QTreeWidgetItem*> itemList = selectedItems();
   for (int selectedIndex = 0; selectedIndex < itemList.size(); selectedIndex++)
   {
      QTreeWidgetItem* treeItem = itemList.at(selectedIndex);
      treeItem->setSelected(false);
   }

   // Select our clicked item.
   clickedItem->setSelected(true);

   // Iterate through each definition to see if they were selected.
   for (int itemIndex = 0; itemIndex < topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem* fileItem = topLevelItem(itemIndex);

      if (fileItem == clickedItem)
      {
         // Show Shader Definition Context menu.
         mDefinitionContext->exec(contextEvent->globalPos());
         return;
      }

      // Iterate through each program to see if they were selected.
      for (int groupIndex = 0; groupIndex < fileItem->childCount(); groupIndex++)
      {
         QTreeWidgetItem* groupItem = fileItem->child(groupIndex);

         for (int programIndex = 0; programIndex < groupItem->childCount(); programIndex++)
         {
            QTreeWidgetItem* programItem = groupItem->child(programIndex);

            if (programItem == clickedItem)
            {
               mProgramContext->exec(contextEvent->globalPos());
               return;
            }
         }
      }
   }

   contextEvent->ignore();
}

///////////////////////////////////////////////////////////////////////////////

ResourceDock::ResourceDock()
  : QDockWidget()
  , mTabs(NULL)
{
   setWindowTitle(tr("Resources"));
   setMouseTracking(true);
  
   mGeometryTreeWidget = new QTreeWidget(this);
   mShaderTreeWidget   = new ShaderTree(this);  
   mLightTreeWidget    = new QTreeWidget(this);  

   mGeometryTreeWidget->headerItem()->setText(0, "");
   mShaderTreeWidget->headerItem()->setText(0, "");
   mLightTreeWidget->headerItem()->setText(0, "");

   connect(mGeometryTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
           this, SLOT(OnGeometryItemChanged(QTreeWidgetItem*, int))); 

   connect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
      this, SLOT(OnShaderItemChanged(QTreeWidgetItem*, int))); 

   connect(mShaderTreeWidget, SIGNAL(itemSelectionChanged()),
      this, SLOT(OnShaderSelectionChanged()));

   connect(mLightTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
           this, SLOT(OnLightItemClicked(QTreeWidgetItem*, int)));

   connect(mLightTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
           this, SLOT(OnLightItemChanged(QTreeWidgetItem*, int))); 

   mTabs = new QTabWidget;
   mTabs->addTab(mGeometryTreeWidget, tr("Geometry"));
   mTabs->addTab(mShaderTreeWidget, tr("Shaders"));
   mTabs->addTab(mLightTreeWidget, tr("Lights"));

   setWidget(mTabs);

   CreateLightItems();
}

///////////////////////////////////////////////////////////////////////////////
ResourceDock::~ResourceDock(){}

///////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::FindGeometryItem(const std::string& fullName) const
{
   for (int itemIndex = 0; itemIndex < mGeometryTreeWidget->topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem *childItem = mGeometryTreeWidget->topLevelItem(itemIndex);

      std::string currentPath = childItem->toolTip(0).toStdString();
      std::string findPath = fullName;

      std::transform(currentPath.begin(), currentPath.end(), currentPath.begin(), (int(*)(int))std::tolower);
      std::transform(findPath.begin(), findPath.end(), findPath.begin(), (int(*)(int))std::tolower);

      // Case insensitive comparison
      if (currentPath == findPath)
      {
         return childItem; 
      }
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::FindShaderFileItem(const std::string& filename) const
{
   for (int itemIndex = 0; itemIndex < mShaderTreeWidget->topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem* fileItem = mShaderTreeWidget->topLevelItem(itemIndex);

      if (filename == fileItem->toolTip(0).toStdString())
      {
         return fileItem;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::FindShaderGroupItem(const std::string& groupName, const QTreeWidgetItem* fileItem) const
{
   for (int itemIndex = 0; itemIndex < fileItem->childCount(); ++itemIndex)
   {
      QTreeWidgetItem* shaderGroup = fileItem->child(itemIndex);

      if (groupName == shaderGroup->text(0).toStdString())
      {
         return shaderGroup;
      }
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
bool ResourceDock::FindShaderFileEntryName(const std::string& entryName) const
{
   for (int itemIndex = 0; itemIndex < mShaderTreeWidget->topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem* fileItem = mShaderTreeWidget->topLevelItem(itemIndex);

      if (entryName == fileItem->text(0).toStdString())
      {
         return true;
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
void ResourceDock::ReselectCurrentShaderItem()
{
   // Get the file item.
   QTreeWidgetItem* fileItem = FindShaderFileItem(mCurrentShaderFile.toStdString());

   // If the file no longer exists, clear the data and return none
   if (fileItem == NULL)
   {
      mCurrentShaderFile.clear();
      mCurrentShaderGroup.clear();
      mCurrentShaderProgram.clear();
      return;
   }
   fileItem->setExpanded(true);

   // Get the group item.
   QTreeWidgetItem* groupItem = FindShaderGroupItem(mCurrentShaderGroup.toStdString(), fileItem);

   // If the group doesn't exist, clear the data and return none
   if (groupItem == NULL)
   {
      mCurrentShaderFile.clear();
      mCurrentShaderGroup.clear();
      mCurrentShaderProgram.clear();
      return;
   }
   groupItem->setExpanded(true);

   // Now find the program.
   for (int childIndex = 0; childIndex < groupItem->childCount(); childIndex++)
   {
      QTreeWidgetItem* programItem = groupItem->child(childIndex);

      if (mCurrentShaderProgram == programItem->text(0))
      {
         mCurrentShaderFile.clear();
         mCurrentShaderGroup.clear();
         mCurrentShaderProgram.clear();
         programItem->setCheckState(0, Qt::Checked);
         programItem->setSelected(true);
         return;
      }
   }

   // If we get here, it means the program doesn't exist.
   mCurrentShaderFile.clear();
   mCurrentShaderGroup.clear();
   mCurrentShaderProgram.clear();
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::SetGeometry(const std::string& fullName, bool shouldDisplay) const
{  
   QTreeWidgetItem* geometryItem = FindGeometryItem(fullName);
   
   if (geometryItem)
   {
      SetGeometry(geometryItem, shouldDisplay);
   }   
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::SetGeometry(QTreeWidgetItem* geometryItem, bool shouldDisplay) const
{
   std::string fullName = geometryItem->toolTip(0).toStdString();
   geometryItem->setCheckState(0, (shouldDisplay) ? Qt::Checked : Qt::Unchecked);
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnNewShader(const std::string& filename, const std::string& shaderGroup, const std::string& shaderProgram)
{
   // We don't want this signal emitted when we're adding a shader
   disconnect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
              this, SLOT(OnShaderItemChanged(QTreeWidgetItem*, int)));

   // Get the file item.
   QTreeWidgetItem* fileItem = FindShaderFileItem(filename);
   // If the file doesn't exist, create a new one.
   if (fileItem == NULL)
   {
      QFileInfo fileInfo(filename.c_str());

      std::string baseEntryName = fileInfo.baseName().toStdString().c_str();
      std::string entryName = baseEntryName;
      // If the entry name already exists, add a number to it instead.
      int entryIndex = 1;
      while (FindShaderFileEntryName(entryName))
      {
         entryName = baseEntryName;
         entryName += " (";

         char number[10];
         sprintf(number, "%d", entryIndex);
         entryName += number;

         entryName += ")";
         entryIndex++;
      }

      fileItem = new QTreeWidgetItem;
      fileItem->setText(0, entryName.c_str());
      fileItem->setToolTip(0, filename.c_str());

      mShaderTreeWidget->addTopLevelItem(fileItem);
   }

   // Get the group item.
   QTreeWidgetItem* groupItem = FindShaderGroupItem(shaderGroup, fileItem);

   // If the group doesn't exist, create a new one.
   if (groupItem == NULL)
   {
      groupItem = new QTreeWidgetItem;
      groupItem->setText(0, shaderGroup.c_str());
      groupItem->setFlags(Qt::ItemIsSelectable |
         Qt::ItemIsUserCheckable |
         Qt::ItemIsEnabled);

      fileItem->addChild(groupItem);
   }

   // If the program already exists, don't add it again.
   for (int childIndex = 0; childIndex < groupItem->childCount(); childIndex++)
   {
      QTreeWidgetItem* programItem = groupItem->child(childIndex);

      if (shaderProgram == programItem->text(0).toStdString())
      {
         connect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
            this, SLOT(OnShaderItemChanged(QTreeWidgetItem*, int)));
         // The program already exists, so we don't want to add it again.
         return;
      }
   }

   // Create the program item.
   QTreeWidgetItem* programItem = new QTreeWidgetItem();
   programItem->setText(0, shaderProgram.c_str());

   // The shader itself should have a checkbox
   programItem->setCheckState(0, Qt::Unchecked);

   groupItem->addChild(programItem);

   connect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
           this, SLOT(OnShaderItemChanged(QTreeWidgetItem*, int)));
}

////////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnShaderSelectionChanged()
{
   dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();

   QList<QTreeWidgetItem*> itemList = mShaderTreeWidget->selectedItems();

   if (itemList.size() > 0)
   {
      QTreeWidgetItem* selectedItem = itemList.at(0);

      // Now determine if the selected item is a program.
      for (int itemIndex = 0; itemIndex < mShaderTreeWidget->topLevelItemCount(); ++itemIndex)
      {
         QTreeWidgetItem* fileItem = mShaderTreeWidget->topLevelItem(itemIndex);

         if (fileItem == selectedItem)
         {
            ToggleVertexShaderSources(false);
            ToggleFragmentShaderSources(false);
            mShaderTreeWidget->SetShaderSourceEnabled(false, false);
            return;
         }

         // Iterate through each program to see if they were selected.
         for (int groupIndex = 0; groupIndex < fileItem->childCount(); groupIndex++)
         {
            QTreeWidgetItem* groupItem = fileItem->child(groupIndex);

            if (groupItem == selectedItem)
            {
               ToggleVertexShaderSources(false);
               ToggleFragmentShaderSources(false);
               mShaderTreeWidget->SetShaderSourceEnabled(false, false);
               return;
            }

            for (int programIndex = 0; programIndex < groupItem->childCount(); programIndex++)
            {
               QTreeWidgetItem* programItem = groupItem->child(programIndex);

               if (programItem == selectedItem)
               {
                  dtCore::ShaderProgram *program = 
                     shaderManager.FindShaderPrototype(programItem->text(0).toStdString(), groupItem->text(0).toStdString());
                  if (program)
                  {
                     const std::vector<std::string>& vertShaderList = program->GetVertexShaders();
                     const std::vector<std::string>& fragShaderList = program->GetFragmentShaders();
                     
                     bool vertexEnabled = vertShaderList.size()? true: false;
                     bool fragmentEnabled = fragShaderList.size()? true: false;
                     
                     ToggleVertexShaderSources(vertexEnabled);
                     ToggleFragmentShaderSources(fragmentEnabled);
                     mShaderTreeWidget->SetShaderSourceEnabled(vertexEnabled, fragmentEnabled);
                  }  
                  return;
               }
            }
         }
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnShaderItemChanged(QTreeWidgetItem* item, int column)
{ 
   if (column == 0)
   {
      dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();

      QString programName = item->text(0);
      QString groupName   = item->parent()->text(0);
      QString fileName    = item->parent()->parent()->toolTip(0);

      if (item->checkState(0) == Qt::Checked)
      {
         // Now load the shader file if it isn't already loaded.
         if (mCurrentShaderFile != fileName)
         {
            shaderManager.Clear();
            shaderManager.LoadShaderDefinitions(fileName.toStdString());
         }

         // Store so we know where the source files can be found
         mCurrentShaderFile    = fileName;
         mCurrentShaderGroup   = groupName;
         mCurrentShaderProgram = programName;

         QTreeWidgetItemIterator treeIter(mShaderTreeWidget);

         // Uncheck the previously checked item
         while (*treeIter)
         {
            if ((*treeIter)->checkState(0) == Qt::Checked && (*treeIter) != item)
            {
               (*treeIter)->setCheckState(0, Qt::Unchecked);
            }

            ++treeIter;
         }

         emit ApplyShader(groupName.toStdString(), programName.toStdString());
      }
      else if (item->checkState(0) == Qt::Unchecked)
      {
         // Only clear the shader program if we are unchecking the current one.
         if (fileName == mCurrentShaderFile &&
            groupName == mCurrentShaderGroup &&
            programName == mCurrentShaderProgram)
         {
            mCurrentShaderGroup.clear();
            mCurrentShaderProgram.clear();
         }

         emit RemoveShader();
      }
   }  
}

////////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnLightItemChanged(QTreeWidgetItem* item, int column)
{
   if (column == 0)
   {
      if (item->checkState(0) == Qt::Checked)
      {
         int lightID = GetLightIDFromItem(item);

         QTreeWidgetItemIterator treeIter(mLightTreeWidget);

         // Uncheck the previously checked item
         while (*treeIter)
         {
            if ((*treeIter)->checkState(0) == Qt::Checked && (*treeIter) != item)
            {
               (*treeIter)->setCheckState(0, Qt::Unchecked);
            }

            ++treeIter;
         }

         emit SetCurrentLight(lightID);
      }
      else if (item->checkState(0) == Qt::Unchecked)
      {
         emit SetCurrentLight(-1);
      }
   }  
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnReloadShaderFiles()
{
   QList<QString> fileNames;

   // Get our full list of files.
   for (int itemIndex = 0; itemIndex < mShaderTreeWidget->topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem* fileItem = mShaderTreeWidget->topLevelItem(itemIndex);

      fileNames.append(fileItem->toolTip(0));
   }

   for (int itemIndex = 0; itemIndex < mShaderTreeWidget->topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem* fileItem = mShaderTreeWidget->topLevelItem(itemIndex);

      DeleteTreeItem(fileItem);
   }
   mShaderTreeWidget->clear();

   for (int fileIndex = 0; fileIndex < fileNames.size(); fileIndex++)
   {
      dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();
      shaderManager.Clear();
      shaderManager.LoadShaderDefinitions(fileNames.at(fileIndex).toStdString());

      std::vector<dtCore::RefPtr<dtCore::ShaderGroup> > shaderGroupList;
      shaderManager.GetAllShaderGroupPrototypes(shaderGroupList);

      // Emit all shader groups and their individual shaders
      for (size_t groupIndex = 0; groupIndex < shaderGroupList.size(); ++groupIndex)
      {
         std::vector<dtCore::RefPtr<dtCore::ShaderProgram> > programList;
         shaderGroupList[groupIndex]->GetAllShaders(programList);

         const std::string& groupName = shaderGroupList[groupIndex]->GetName();

         for (size_t programIndex = 0; programIndex < programList.size(); ++programIndex)
         {
            OnNewShader(fileNames.at(fileIndex).toStdString(), groupName, programList[programIndex]->GetName());
         }
      }
   }

   // Now reload the most recent shader.
   ReselectCurrentShaderItem();
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnNewGeometry(const std::string& path, const std::string& filename)
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
void ResourceDock::OnGeometryItemChanged(QTreeWidgetItem* item, int column)
{
   if (column == 0)
   {
      // The full path is stored in the tooltip
      QString geomName = item->toolTip(0);

      if (item->checkState(0) == Qt::Checked)
      {
         QTreeWidgetItemIterator treeIter(mGeometryTreeWidget);

         // Uncheck the previously checked item
         while (*treeIter)
         {
            if ((*treeIter)->checkState(0) == Qt::Checked && (*treeIter) != item)
            {
               (*treeIter)->setCheckState(0, Qt::Unchecked);
            }

            ++treeIter;
         }

         emit LoadGeometry(geomName.toStdString());         
      }
      else if (item->checkState(0) == Qt::Unchecked)
      {
         emit UnloadGeometry();
      }
   }  
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnLightUpdate(const LightInfo& lightInfo)
{
   // disconnect the item changed trigger so we don't get infinite item changed events.
   disconnect(mLightTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
      this, SLOT(OnLightItemChanged(QTreeWidgetItem*, int))); 

   int lightIndex = lightInfo.light->GetNumber();  

   const osg::LightSource* osgSource = lightInfo.light->GetLightSource();
   const osg::Light* osgLight = osgSource->getLight();

   bool enabled = lightInfo.light->GetEnabled();
   if (enabled)
   {
      mLightItems[lightIndex].light->setText(1, "Enabled");
   }
   else
   {
      mLightItems[lightIndex].light->setText(1, "Disabled");
   }

   const osg::Vec3& position = lightInfo.transform->GetTranslation();
   osg::Vec3 right, up, rotation;
   lightInfo.transform->GetOrientation(right, up, rotation);
   const osg::Vec4& ambient  = osgLight->getAmbient();
   const osg::Vec4& diffuse  = osgLight->getDiffuse();
   const osg::Vec4& specular = osgLight->getSpecular();

   // Get the type of light.
   QString typeString;
   dtCore::InfiniteLight* infiniteLight = dynamic_cast<dtCore::InfiniteLight*>(lightInfo.light);
   if (infiniteLight)
   {
      typeString = "Infinite";
      float azimuth = 0.0f;
      float elevation = 0.0f;
      infiniteLight->GetAzimuthElevation(azimuth, elevation);

      if (!mLightItems[lightIndex].custom)
      {
         mLightItems[lightIndex].custom = CreateTreeItem(tr("Custom"), tr(""), Qt::ItemIsEnabled, mLightItems[lightIndex].light);

         CreateTreeItem(tr("Azimuth"), QString("%1").arg(azimuth), Qt::ItemIsEnabled | Qt::ItemIsEditable, mLightItems[lightIndex].custom);
         CreateTreeItem(tr("Elevation"), QString("%1").arg(elevation), Qt::ItemIsEnabled | Qt::ItemIsEditable, mLightItems[lightIndex].custom);
      }

      mLightItems[lightIndex].custom->child(0)->setText(1, QString("%1").arg(azimuth));
      mLightItems[lightIndex].custom->child(1)->setText(1, QString("%1").arg(elevation));
   }
   else
   {
      dtCore::PositionalLight* positionalLight = dynamic_cast<dtCore::PositionalLight*>(lightInfo.light);
      if (positionalLight)
      {
         dtCore::SpotLight* spotLight = dynamic_cast<dtCore::SpotLight*>(lightInfo.light);
         if (spotLight)
         {
            typeString = "Spot";
            float cutoff = spotLight->GetSpotCutoff();
            float exponent = spotLight->GetSpotExponent();

            if (!mLightItems[lightIndex].custom)
            {
               mLightItems[lightIndex].custom = CreateTreeItem(tr("Custom"), tr(""), Qt::ItemIsEnabled, mLightItems[lightIndex].light);

               CreateTreeItem(tr("Cut off"), QString("%1").arg(cutoff), Qt::ItemIsEnabled | Qt::ItemIsEditable, mLightItems[lightIndex].custom);
               CreateTreeItem(tr("Exponent"), QString("%1").arg(exponent), Qt::ItemIsEnabled | Qt::ItemIsEditable, mLightItems[lightIndex].custom);
            }

            mLightItems[lightIndex].custom->child(0)->setText(1, QString("%1").arg(cutoff));
            mLightItems[lightIndex].custom->child(1)->setText(1, QString("%1").arg(exponent));
         }
         else
         {
            typeString = "Positional";
            float constant = 0.0f;
            float linear = 0.0f;
            float quadratic = 0.0f;
            positionalLight->GetAttenuation(constant, linear, quadratic);

            if (!mLightItems[lightIndex].custom)
            {
               mLightItems[lightIndex].custom = CreateTreeItem(tr("Attenuation"), tr(""), Qt::ItemIsEnabled, mLightItems[lightIndex].light);

               CreateTreeItem(tr("Constant"), QString("%1").arg(constant), Qt::ItemIsEnabled | Qt::ItemIsEditable, mLightItems[lightIndex].custom);
               CreateTreeItem(tr("Linear"), QString("%1").arg(linear), Qt::ItemIsEnabled | Qt::ItemIsEditable, mLightItems[lightIndex].custom);
               CreateTreeItem(tr("Quadratic"), QString("%1").arg(quadratic), Qt::ItemIsEnabled | Qt::ItemIsEditable, mLightItems[lightIndex].custom);
            }

            mLightItems[lightIndex].custom->child(0)->setText(1, QString("%1").arg(constant));
            mLightItems[lightIndex].custom->child(1)->setText(1, QString("%1").arg(linear));
            mLightItems[lightIndex].custom->child(2)->setText(1, QString("%1").arg(quadratic));
         }
      }
   }
   mLightItems[lightIndex].type->setText(1, typeString);

   SetPositionItem(mLightItems[lightIndex].position, position);
   SetPositionItem(mLightItems[lightIndex].rotation, rotation);
   SetColorItem(mLightItems[lightIndex].ambient, ambient);
   SetColorItem(mLightItems[lightIndex].diffuse, diffuse);
   SetColorItem(mLightItems[lightIndex].specular, specular);

   //std::ostringstream oss;
   //oss << "light #" << lightNumber << ": (" 
   //    << position.x() << ", " << position.y() << ", " << position.z() 
   //    << ")";

   //std::cout << oss.str() << std::endl;   

   // Reconnect the item changed event.
   connect(mLightTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
      this, SLOT(OnLightItemChanged(QTreeWidgetItem*, int))); 
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnOpenCurrentVertexShaderSources()
{
   dtCore::ShaderManager &shaderManager = dtCore::ShaderManager::GetInstance();

   QList<QTreeWidgetItem*> itemList = mShaderTreeWidget->selectedItems();

   if (itemList.size() > 0)
   {
      QTreeWidgetItem* currentItem = itemList.at(0);

      dtCore::ShaderProgram *program = 
         shaderManager.FindShaderPrototype(currentItem->text(0).toStdString(), currentItem->parent()->text(0).toStdString());
      
      if (program)
      {
         const std::vector<std::string>& vertShaderList = program->GetVertexShaders();
         OpenFilesInTextEditor(vertShaderList);     
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnOpenCurrentFragmentShaderSources()
{
   dtCore::ShaderManager &shaderManager = dtCore::ShaderManager::GetInstance();

   QList<QTreeWidgetItem*> itemList = mShaderTreeWidget->selectedItems();

   if (itemList.size() > 0)
   {
      QTreeWidgetItem* currentItem = itemList.at(0);

      dtCore::ShaderProgram *program = 
         shaderManager.FindShaderPrototype(currentItem->text(0).toStdString(), currentItem->parent()->text(0).toStdString());

      if (program)
      {
         const std::vector<std::string>& fragShaderList = program->GetFragmentShaders();
         OpenFilesInTextEditor(fragShaderList);     
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::CreateLightItems()
{
   QStringList headerLables;
   headerLables << "Property" << "Value";

   mLightTreeWidget->setHeaderLabels(headerLables);

   for (int lightIndex = 0; lightIndex < dtCore::MAX_LIGHTS; ++lightIndex)
   {
      std::ostringstream oss;
      oss << "Light" << lightIndex;

      QTreeWidgetItem* newLightItem = new QTreeWidgetItem;
      newLightItem->setText(0, oss.str().c_str());  
      newLightItem->setText(1, "Disabled");
      newLightItem->setFlags(Qt::ItemIsSelectable |
                             Qt::ItemIsUserCheckable |
                             Qt::ItemIsEnabled);
      newLightItem->setCheckState(0, Qt::Unchecked);

      QTreeWidgetItem* type = new QTreeWidgetItem(newLightItem);
      type->setText(0, "Type");  
      type->setText(1, "Infinite");    
      type->setFlags(Qt::ItemIsEnabled);

      mLightTreeWidget->addTopLevelItem(newLightItem);

      mLightItems[lightIndex].light     = newLightItem;
      mLightItems[lightIndex].type      = type;
      mLightItems[lightIndex].position  = CreatePositionItem(newLightItem);
      mLightItems[lightIndex].rotation  = CreateRotationItem(newLightItem);
      mLightItems[lightIndex].ambient   = CreateColorItem("Ambient", newLightItem);
      mLightItems[lightIndex].diffuse   = CreateColorItem("Diffuse", newLightItem);
      mLightItems[lightIndex].specular  = CreateColorItem("Specular", newLightItem);
      mLightItems[lightIndex].custom    = NULL;
   }

   // Set the default light to "on"
   QTreeWidgetItem* light0 = mLightItems[0].light;
   light0->setText(1, "Enabled");   
   light0->setCheckState(0, Qt::Checked);
}

///////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::CreatePositionItem(QTreeWidgetItem* parent)
{
   QTreeWidgetItem* position = new QTreeWidgetItem(parent);
   position->setText(0, "Position");  
   position->setFlags(Qt::ItemIsEnabled);

   QTreeWidgetItem* xItem = new QTreeWidgetItem(position);
   QTreeWidgetItem* yItem = new QTreeWidgetItem(position);
   QTreeWidgetItem* zItem = new QTreeWidgetItem(position);

   xItem->setText(0, "x");
   yItem->setText(0, "y");
   zItem->setText(0, "z");       

   return position;
}

///////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::CreateRotationItem(QTreeWidgetItem* parent)
{
   QTreeWidgetItem* rotation = new QTreeWidgetItem(parent);
   rotation->setText(0, "Rotation");  
   rotation->setFlags(Qt::ItemIsEnabled);

   QTreeWidgetItem* xItem = new QTreeWidgetItem(rotation);
   QTreeWidgetItem* yItem = new QTreeWidgetItem(rotation);
   QTreeWidgetItem* zItem = new QTreeWidgetItem(rotation);

   xItem->setText(0, "x");
   yItem->setText(0, "y");
   zItem->setText(0, "z");

   return rotation;
}

///////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::CreateColorItem(const std::string& name, QTreeWidgetItem* parent)
{
   QTreeWidgetItem* colorItem = new QTreeWidgetItem(parent);
   colorItem->setText(0, name.c_str());
   colorItem->setFlags(Qt::ItemIsEnabled);

   QColor color;
   QPixmap colorPicker(16, 16);
   colorPicker.fill(color);
   colorItem->setIcon(1, colorPicker);

   QTreeWidgetItem* redItem   = new QTreeWidgetItem(colorItem);
   QTreeWidgetItem* greenItem = new QTreeWidgetItem(colorItem);
   QTreeWidgetItem* blueItem  = new QTreeWidgetItem(colorItem);
   QTreeWidgetItem* alphaItem = new QTreeWidgetItem(colorItem);

   redItem->setText(0, "r");
   greenItem->setText(0, "g");
   blueItem->setText(0, "b");
   alphaItem->setText(0, "a");

   return colorItem;
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::SetPositionItem(QTreeWidgetItem* item, const osg::Vec3& position)
{
   QString xString = QString("%1").arg(position.x());
   QString yString = QString("%1").arg(position.y());
   QString zString = QString("%1").arg(position.z());

   QString fullString = QString("[%1, %2, %3]").arg(xString, yString, zString);

   item->setText(1, fullString);
   item->child(0)->setText(1, xString);
   item->child(1)->setText(1, yString);
   item->child(2)->setText(1, zString);
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::SetColorItem(QTreeWidgetItem* item, const osg::Vec4& color)
{
   QColor qtColor(color.x() * 255, 
                  color.y() * 255,
                  color.z() * 255,
                  color.w() * 255);

//   QBrush newBrush(QColor::fromRgbF(color.x(), color.y(), color.z(), color.a()));    
//   item->setBackground(1, newBrush);

   QPixmap colorFill(16, 16);
   colorFill.fill(qtColor);
   item->setIcon(1, colorFill);

   QString rString = QString("%1").arg(color.x());
   QString gString = QString("%1").arg(color.y());
   QString bString = QString("%1").arg(color.z());
   QString aString = QString("%1").arg(color.w());

   item->child(0)->setText(1, rString);
   item->child(1)->setText(1, gString);
   item->child(2)->setText(1, bString);
   item->child(3)->setText(1, aString);
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OpenFilesInTextEditor(const std::vector<std::string>& fileList)
{   
   for (size_t fileIndex = 0; fileIndex < fileList.size(); ++fileIndex)
   {
      std::string fileName = dtCore::FindFileInPathList(fileList[fileIndex]);      

      TextEdit* editor = new TextEdit;
      editor->resize(700, 800);
      editor->load(fileName.c_str());
      editor->show();
   }      
}

///////////////////////////////////////////////////////////////////////////////
int ResourceDock::GetLightIDFromItem(QTreeWidgetItem* item)
{
   for (int lightIndex = 0; lightIndex < dtCore::MAX_LIGHTS; ++lightIndex)
   {
      LightItems& light = mLightItems[lightIndex];
      if (light.light    == item ||
          light.type     == item ||
          light.position == item ||
          light.ambient  == item ||
          light.diffuse  == item ||
          light.specular == item ||
          light.custom   == item ||
          light.position->indexOfChild(item) != -1 ||
          light.ambient->indexOfChild(item) != -1  ||
          light.diffuse->indexOfChild(item) != -1  ||
          light.specular->indexOfChild(item) != -1 ||
          light.custom->indexOfChild(item) != -1)
      {
         return lightIndex;
      }
   }

   return -1;
}

////////////////////////////////////////////////////////////////////////////////
void ResourceDock::DeleteTreeItem(QTreeWidgetItem* item)
{
   if (!item)
   {
      return;
   }

   // First delete all children of this item.
   for (int childIndex = 0; childIndex < item->childCount(); childIndex++)
   {
      DeleteTreeItem(item->child(childIndex));
   }

   // Now delete the item itself.
   delete item;
}

////////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::CreateTreeItem(const QString& name, const QString& value, Qt::ItemFlags flags, QTreeWidgetItem* parent)
{
   QTreeWidgetItem* item = new QTreeWidgetItem(parent);
   item->setText(0, name);
   item->setText(1, value);
   item->setFlags(flags);
   return item;
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnLightItemClicked(QTreeWidgetItem* item, int column)
{
   if (column == 1)
   {
      int lightID = GetLightIDFromItem(item);

      if (lightID != -1)
      {
         LightItems& light = mLightItems[lightID];

         // Toggle the enabled status of the light.
         if (item == light.light)
         {
            bool isEnabled = item->text(1) == QString("Enabled");
            emit SetLightEnabled(lightID, !isEnabled);
         }

         // Change the type of the light.
         if (item == light.type)
         {
            if (item->text(1) == QString("Infinite"))
            {
               emit SetLightType(lightID, 1);
            }
            else if (item->text(1) == QString("Positional"))
            {
               emit SetLightType(lightID, 2);
            }
            else if (item->text(1) == QString("Spot"))
            {
               emit SetLightType(lightID, 0);
            }

            // Delete the custom light data so it can be refreshed with new data.
            DeleteTreeItem(mLightItems[lightID].custom);
            mLightItems[lightID].custom = NULL;
         }

         // Change a color attribute of the light.
         if (item == light.ambient ||
             item == light.diffuse ||
             item == light.specular ||
             light.ambient->indexOfChild(item) != -1 ||
             light.diffuse->indexOfChild(item) != -1 ||
             light.specular->indexOfChild(item) != -1)
         {
            QColor color;

            // Get the current color.
            if (item == light.ambient || light.ambient->indexOfChild(item) != -1)
            {
               color.setRedF(light.ambient->child(0)->text(1).toFloat());
               color.setGreenF(light.ambient->child(1)->text(1).toFloat());
               color.setBlueF(light.ambient->child(2)->text(1).toFloat());
               color.setAlphaF(light.ambient->child(3)->text(1).toFloat());
            }
            if (item == light.diffuse || light.diffuse->indexOfChild(item) != -1)
            {
               color.setRedF(light.diffuse->child(0)->text(1).toFloat());
               color.setGreenF(light.diffuse->child(1)->text(1).toFloat());
               color.setBlueF(light.diffuse->child(2)->text(1).toFloat());
               color.setAlphaF(light.diffuse->child(3)->text(1).toFloat());
            }
            if (item == light.specular || light.specular->indexOfChild(item) != -1)
            {
               color.setRedF(light.specular->child(0)->text(1).toFloat());
               color.setGreenF(light.specular->child(1)->text(1).toFloat());
               color.setBlueF(light.specular->child(2)->text(1).toFloat());
               color.setAlphaF(light.specular->child(3)->text(1).toFloat());
            }

            // Bring up the color picker
            bool clickedOk = false;
            QRgb pickedColor = QColorDialog::getRgba(qRgba(color.red(), color.green(), color.blue(), color.alpha()), &clickedOk);

            // If the ok button was pressed
            if (clickedOk)
            {
               // Get the normalized color values
               float red   = qRed(pickedColor) / 255.0f;
               float green = qGreen(pickedColor) / 255.0f;
               float blue  = qBlue(pickedColor) / 255.0f;
               float alpha = qAlpha(pickedColor) / 255.0f;

               osg::Vec4 lightColor(red, green, blue, alpha);

               if (item == light.ambient || light.ambient->indexOfChild(item) != -1)
               {
                  emit SetAmbient(lightID, lightColor);
               }
               else if (item == light.diffuse || light.diffuse->indexOfChild(item) != -1)
               {
                  emit SetDiffuse(lightID, lightColor);
               }
               else if (item == light.specular || light.specular->indexOfChild(item) != -1)
               {
                  emit SetSpecular(lightID, lightColor);
               }
            }         
         }
      }
   }   
}

////////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnEditShaderDef()
{
   // TODO: Open the Shader Definition file in an XML editor.
}

////////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnRemoveShaderDef()
{
   // Iterate through each file item and find the one we want to remove.
   for (int itemIndex = 0; itemIndex < mShaderTreeWidget->topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem* fileItem = mShaderTreeWidget->topLevelItem(itemIndex);

      if (fileItem->isSelected())
      {
         mShaderTreeWidget->takeTopLevelItem(itemIndex);

         emit RemoveShaderDef(fileItem->toolTip(0).toStdString());

         OnReloadShaderFiles();
         return;
      }
   }
}

